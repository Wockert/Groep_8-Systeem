#include "LijnVolgenToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "StartToestand.h"
#include "KruispuntToestand.h"
#include "GroeneLijnToestand.h"

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void LijnVolgenToestand::enter() {
  robot.getHardware().print("Lijn volgen");
  vorigeFout  = 0;
  dGefilterd  = 0;
  foutSom     = 0;
  basis       = RobotConfig::SNELHEID_LAAG;   // rustig wegrijden, ramp daarna op
  aLosgelaten = false;   // de startdruk op A telt niet meteen als stop
  inBocht     = false;
  bochtKant   = 0;
  randTeller  = 0;
  grijsTeller = 0;
  groenTeller = 0;
}

void LijnVolgenToestand::update() {
  // --- Nog een keer A = stoppen ---
  // A is ook de startknop, dus hij moet eerst losgelaten zijn; pas een
  // NIEUWE druk stopt de robot en gaat terug naar het startscherm.
  const SensorData& s = robot.getSensorData();
  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    robot.setState(new StartToestand(robot));
    return;   // oude toestand is hierna verwijderd
  }

  // EEN meting per ronde: alles komt uit de snapshot (gekalibreerd:
  // 0=wit..1000=zwart per sensor, positie 0..4000 met 2000 = midden).
  int positie = s.linePosition;
  for (int i = 0; i < 5; i++) sensorWaarden[i] = (unsigned int)s.lineValues[i];

  // Sterkste sensor + index: ziet er nog iets de lijn, en zo ja, waar?
  unsigned int maxWaarde = 0;
  int          maxIndex  = 0;
  for (int i = 0; i < 5; i++)
    if (sensorWaarden[i] > maxWaarde) { maxWaarde = sensorWaarden[i]; maxIndex = i; }

  const unsigned int DREMPEL_LIJN = 150;  // < dit => geen lijn onder de sensoren
                                          // (150: ruim onder de groenband van 250+,
                                          // anders flippert vaal groen naar "lijn kwijt")

  // --- Zijgeheugen bijhouden ---
  // ALLEEN als de lijn op een BUITENSTE sensor ligt (0 of 4): dan knikt de
  // lijn echt die kant op. Sensor 1/3 telt niet — dat is gewoon licht
  // slingeren, en dat mag de stippellijn-overbrugging niet verdringen
  // (vrijwel elk gat begint anders met een "recent zij-sample" -> pivot
  // i.p.v. rechtdoor oversteken).
  if (maxWaarde >= DREMPEL_LIJN && (maxIndex == 0 || maxIndex == 4)) {
    zwartZijKant = (maxIndex == 0) ? -1 : +1;
    zwartZijCm   = s.distanceCm;
  }

  // --- Grijze markering gezien? Kruispunt neemt over ---
  // Streng, om valse detecties te voorkomen:
  //   1) NIET midden in een scherpe-bocht-pivot (geometrie scheef).
  //   2) De lijn moet duidelijk zichtbaar EN ongeveer in het MIDDEN liggen.
  //      In een bocht ligt de lijn scheef en ziet de buitenrand een grijs-
  //      achtige gradient -> dat is GEEN markering. Alleen op (bijna) recht.
  //   3) Grijs moet GRIJS_BEVESTIG_LIJN metingen ACHTER ELKAAR gezien zijn
  //      (debounce tegen losse ruispieken).
  // BAANPLAN-GATING: grijs telt alleen als een KRUISPUNT aan de beurt is.
  // Dat haalt de valse grijs-detecties (bochtranden!) er structureel uit.
  bool kruispuntVerwacht = robot.getBaanPlan().isVerwacht(CP_KRUISPUNT, s.distanceCm);
  LineSensorAnalyse& lijn = robot.getLijnAnalyse();
  bool grijsL = RobotConfig::GRIJS_ACTIEF && kruispuntVerwacht && lijn.grijsTapeLinks();
  bool grijsR = RobotConfig::GRIJS_ACTIEF && kruispuntVerwacht && lijn.grijsTapeRechts();
  bool lijnRecht = (maxWaarde >= DREMPEL_LIJN)
                && (abs(positie - 2000) < RobotConfig::GRIJS_LIJN_MIDDEN);
  if (!inBocht && lijnRecht && (grijsL || grijsR)) {
    if (++grijsTeller >= RobotConfig::GRIJS_BEVESTIG_LIJN) {
      // Richting MEEGEVEN: de markering is zo meteen al gepasseerd, dus
      // KruispuntToestand mag niet op eigen herdetectie hoeven leunen.
      robot.setState(new KruispuntToestand(robot, grijsL ? -1 : +1));
      return;   // oude toestand is hierna verwijderd
    }
  } else {
    grijsTeller = 0;   // reeks onderbroken -> opnieuw beginnen
  }

  // --- Bezig een scherpe bocht (90 graden of meer) uit te draaien? ---
  // Blijf naar de bochtkant pivoteren tot de lijn weer NETJES in het midden
  // ligt. Lijn even kwijt midden in de draai? Gewoon doordraaien — NIET de
  // PD laten overnemen, want die draait dan de verkeerde kant op en hij
  // keert om.
  if (inBocht) {
    // Odometrie: hoeveel graden zijn we al gedraaid sinds de pivot-start?
    // Bij een pivot leggen beide banden dezelfde boog af: hoek (rad) =
    // afgelegde cm / (halve wielbasis).
    long dL = robot.getHardware().getTicksLinks()  - bochtTicksL;
    long dR = robot.getHardware().getTicksRechts() - bochtTicksR;
    float boogCm = ((abs(dL) + abs(dR)) / 2.0f) * RobotConfig::CM_PER_PULSE;
    int   graden = (int)(boogCm / (RobotConfig::WIELBASIS_CM / 2.0f) * 57.296f);

    // Voorbij BOCHT_MAX_GRADEN gedraaid zonder de lijn netjes te vinden?
    // Dan zijn we de uitgaande tak voorbij — nog verder draaien betekent de
    // INKOMENDE lijn pakken en via dezelfde kant terugrijden. Afbreken en
    // rustig vooruit; de normale logica pakt het hieronder weer op.
    if (graden > RobotConfig::BOCHT_MAX_GRADEN) {
      inBocht       = false;
      vorigeFout    = 0;
      dGefilterd    = 0;          // ook hier: geen oude D-restanten meenemen
      foutSom       = 0;
      basis         = RobotConfig::SNELHEID_LAAG;
      randZwartCm   = -1000.0f;   // geen nieuwe pivot op deze (oude) rand-detectie
      naBochtCm     = s.distanceCm;   // cooldown: even geen nieuwe bocht-triggers
      robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_LAAG,
                                         RobotConfig::SNELHEID_LAAG);
      return;
    }

    int p = RobotConfig::SNELHEID_BOCHT;
    bool klaar;
    if (bochtKant < 0) {                         // bocht naar links
      robot.getHardware().setMotorSpeeds(-p, p);
      klaar = (maxWaarde >= DREMPEL_LIJN && maxIndex >= 2);
    } else {                                     // bocht naar rechts
      robot.getHardware().setMotorSpeeds(p, -p);
      klaar = (maxWaarde >= DREMPEL_LIJN && maxIndex <= 2);
    }
    // Pas "klaar" accepteren na BOCHT_MIN_GRADEN: direct bij de pivot-start
    // ligt de OUDE lijn nog onder de sensoren en die telt niet.
    klaar = klaar && (graden >= RobotConfig::BOCHT_MIN_GRADEN);
    if (!klaar) return;
    inBocht    = false;             // hoek rond -> hieronder weer normaal volgen
    // Regelaar "warm" herstarten, anders overcorrigeert hij uit de bocht:
    vorigeFout = positie - 2000;    // ECHTE huidige fout als referentie — met
                                    // 0 zag de D-term een grote nep-sprong en
                                    // gaf hij direct een stuurklap
    dGefilterd = 0;                 // D-filter leeg (geen oude bocht-restanten)
    foutSom    = 0;                 // I-term schoon beginnen na de hoek
    basis      = RobotConfig::SNELHEID_LAAG;   // rustig de bocht uit optrekken,
                                               // niet op volle snelheid de
                                               // restdraai in
    naBochtCm  = s.distanceCm;      // start cooldown (zie hieronder)
    randZwartCm = -1000.0f;         // oude aankondiging is verbruikt
  }

  // --- Na-bocht-cooldown ---
  // Vlak na een pivot zwiept de lijn door de restdraai nog langs de
  // sensoren. Zou de bocht-detectie nu meekijken, dan ziet hij dat aan voor
  // een NIEUWE bocht en start hij een tegenpivot: pingpong = natrillen.
  // Daarom even (NABOCHT_CM) geen nieuwe bocht-triggers en gedempt sturen.
  bool naBocht = (naBochtCm >= 0.0f) &&
                 (s.distanceCm - naBochtCm < RobotConfig::NABOCHT_CM);

  // --- Scherpe bocht AANKONDIGEN (nog niet draaien!) ---
  // Bij 90 graden+ raakt de buitenrand-sensor (0 of 4) de dwarslijn al een
  // paar cm VOOR de knik. Direct pivoteren is dan te vroeg: hij draait naast
  // de hoek, vindt niks en mist de bocht. Daarom alleen ONTHOUDEN dat er een
  // bocht aankomt (kant + tijdstip, na 2 metingen op rij ter bevestiging).
  // De robot blijft gewoon sturen en remt alvast af; de echte pivot start
  // pas zodra de lijn ECHT wegdraait (fout slaat uit, verderop) of
  // verdwijnt (lijn-kwijt-blok).
  bool randLinksZwart  = sensorWaarden[0] > (unsigned int)RobotConfig::BOCHT_RAND_ZWART;
  bool randRechtsZwart = sensorWaarden[4] > (unsigned int)RobotConfig::BOCHT_RAND_ZWART;
  if (!naBocht && (randLinksZwart != randRechtsZwart)) {
    int kant = randLinksZwart ? -1 : +1;
    randTeller = (kant == randZwartKant) ? randTeller + 1 : 1;
    randZwartKant = kant;
    // Bevestigd na 2 metingen — OF direct bij een bijna vol-zwarte rand:
    // op hoge snelheid raakt de dwarslijn de buitensensor soms maar 1 ronde,
    // en dan werd de bocht voorheen helemaal niet aangekondigd (= gemist).
    unsigned int randWaarde = randLinksZwart ? sensorWaarden[0] : sensorWaarden[4];
    if (randTeller >= 2 || randWaarde > (unsigned int)RobotConfig::BOCHT_RAND_DIRECT) {
      randZwartCm = s.distanceCm;   // bocht aanstaande
    }
  } else {
    randTeller = 0;
  }
  // Afstandsvenster (encoders): hetzelfde aantal cm, ongeacht de snelheid.
  bool bochtAanstaande = (randZwartCm >= 0.0f) &&
                         (s.distanceCm - randZwartCm < RobotConfig::BOCHT_AANKONDIG_CM);

  // --- Groene lijn? -> overschakelen na GROEN_BEVESTIG bevestigingen ---
  // NIET rond bochten checken (aanstaande/net gehad): daar leest een halve
  // lijnrand precies zo'n middenwaarde als groen en zou hij vals overgaan.
  // BAANPLAN-GATING: alleen als GROEN ook de eerstvolgende checkpoint is.
  if (RobotConfig::GROEN_ACTIEF && !bochtAanstaande && !naBocht
      && robot.getBaanPlan().isVerwacht(CP_GROEN, s.distanceCm)
      && maxWaarde >= DREMPEL_LIJN && robot.getLijnAnalyse().isGroeneLijn()) {
    if (++groenTeller >= RobotConfig::GROEN_BEVESTIG) {
      Serial.print(F("[GROEN] gedetecteerd (max="));
      Serial.print(maxWaarde);
      Serial.println(F(") -> GroeneLijnToestand"));
      robot.setState(new GroeneLijnToestand(robot));
      return;   // oude toestand is hierna verwijderd
    }
  } else {
    groenTeller = 0;
  }

  // --- Lijn helemaal kwijt? ---
  if (maxWaarde < DREMPEL_LIJN) {
    // BAANPLAN-GATING: rechtdoor oversteken mag ALLEEN als stippellijn de
    // verwachte checkpoint is (of we al in zo'n stippel-zone zitten). Valt de
    // lijn weg terwijl stippellijn NIET aan de beurt is, dan is het geen
    // hiaat maar een bocht -> pivoteren (net als grijs/groen).
    bool stippellijnVerwacht = robot.getBaanPlan().isVerwacht(CP_STIPPELLIJN, s.distanceCm);

    // --- Stippel-zone: meerdere streepjes overbruggen ---
    // In de zone (of: stippellijn verwacht en hier begint hij): rustig
    // rechtdoor. De zone overbrugt ALLE gaten tot de lijn weer doorlopend is.
    if (stippellijnVerwacht || inStippelZone) {
      if (!inStippelZone) {
        inStippelZone     = true;
        stippelZoneStartCm = s.distanceCm;
        Serial.println(F("[STIPPELLIJN] enter zone (streepjes overbruggen)"));
      }
      stippelGatCm = s.distanceCm;   // dit gat; einde-detectie meet hiervandaan

      // Vangnet: duurt de zone te lang zonder doorlopende lijn, dan was het
      // geen stippellijn maar een gemiste bocht -> zone afbreken en zoeken.
      if (s.distanceCm - stippelZoneStartCm > RobotConfig::STIPPEL_ZONE_MAX_CM) {
        inStippelZone = false;
        Serial.println(F("[STIPPELLIJN] zone te lang -> geen stippellijn, zoek-pivot"));
        // val door naar de pivot hieronder
      } else {
        robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_LAAG,
                                           RobotConfig::SNELHEID_LAAG);
        return;
      }
    }

    // Geen stippel(zone): lijn weg = bocht. Pivoteer naar de beste aanwijzing.
    bool zijNetGezien = (zwartZijKant != 0) && (zwartZijCm >= 0.0f) &&
                        (s.distanceCm - zwartZijCm < RobotConfig::LIJN_KWIJT_GEHEUGEN_CM);
    inBocht = true;
    if (bochtAanstaande)             bochtKant = randZwartKant;
    else if (zijNetGezien)           bochtKant = zwartZijKant;
    else if (abs(vorigeFout) > 1000) bochtKant = (vorigeFout < 0) ? -1 : +1;
    else                             bochtKant = (foutSom < 0) ? -1 : +1;  // trend
    bochtTicksL = robot.getHardware().getTicksLinks();    // nulpunt voor de
    bochtTicksR = robot.getHardware().getTicksRechts();   // draaihoek-meting
    int p = RobotConfig::SNELHEID_BOCHT;
    if (bochtKant < 0) robot.getHardware().setMotorSpeeds(-p, p);
    else               robot.getHardware().setMotorSpeeds(p, -p);
    return;
  }

  // --- Lijn in beeld ---
  // In een stippel-zone? Dan is de zone (en het checkpoint) klaar zodra de
  // lijn weer STIPPEL_EINDE_CM AANEENGESLOTEN zwart is: de streepjes zijn
  // voorbij. Tussendoor (kort zwart tussen twee streepjes) blijft de zone aan.
  if (inStippelZone) {
    if (s.distanceCm - stippelGatCm > RobotConfig::STIPPEL_EINDE_CM) {
      inStippelZone = false;
      Serial.println(F("[STIPPELLIJN] exit zone (lijn weer doorlopend)"));
      if (robot.getBaanPlan().isVerwacht(CP_STIPPELLIJN, s.distanceCm)) {
        robot.getBaanPlan().rondAf(s.distanceCm);
      }
    }
  }

  // --- Helling/wip via de pitch-sensor ---
  // Is OMHOOG of WIP de verwachte checkpoint en wijst de neus een paar
  // metingen achter elkaar duidelijk omhoog/omlaag, dan is hij gepasseerd.
  // (Geen aparte toestand nodig: hij blijft gewoon de lijn volgen.)
  // LET OP: de drempel en bevestiging staan bewust hoog — hard remmen leest
  // op de versnellingsmeter ook als ~10 graden kantelen, en dat mag de
  // checkpoint niet afvinken.
  Checkpoint vw = robot.getBaanPlan().verwacht();
  if ((vw == CP_OMHOOG || vw == CP_WIP) && fabs(s.pitch) > RobotConfig::PITCH_GRADEN) {
    if (++pitchTeller >= RobotConfig::PITCH_BEVESTIG) {
      robot.getBaanPlan().rondAf(s.distanceCm);
      pitchTeller = 0;
    }
  } else {
    pitchTeller = 0;
  }

  // --- PD-lijnvolgen (branch Lijnsensorvolgen-Verbeteren) ---
  // Belangrijk: ALLES hieronder is continu — geen dode zone, geen
  // if-sprongen. Elke abrupte overgang (ineens wel/niet sturen, ineens
  // andere snelheid) klappert op sensorruis heen en weer = trillen.
  int fout = positie - 2000;

  // --- Bocht aanstaande en de lijn draait nu echt weg? DAN pivoteren ---
  // Dit is het juiste moment: de robot staat op de knik (de rand kondigde
  // hem aan, en de lijnpositie slaat ver uit naar de bochtkant).
  if (!naBocht && bochtAanstaande && abs(fout) >= RobotConfig::BOCHT_START_FOUT) {
    inBocht   = true;
    bochtKant = randZwartKant;
    bochtTicksL = robot.getHardware().getTicksLinks();    // nulpunt voor de
    bochtTicksR = robot.getHardware().getTicksRechts();   // draaihoek-meting
    int p = RobotConfig::SNELHEID_BOCHT;
    if (bochtKant < 0) robot.getHardware().setMotorSpeeds(-p, p);   // naar links
    else               robot.getHardware().setMotorSpeeds(p, -p);   // naar rechts
    return;
  }

  int afgeleide = fout - vorigeFout;
  dGefilterd    = (dGefilterd + afgeleide) / 2;   // ruisfilter op de D-term

  // I-term: kleine aanhoudende afwijking optellen en wegsturen, zodat hij
  // PERFECT op het midden uitkomt. De som mag NIET resetten bij het passeren
  // van het midden: een scheve motor of sensor vraagt een blijvende
  // tegendruk, en die zat dan steeds opnieuw bij nul. In plaats daarvan
  // lekt de som langzaam weg (1/32 per ronde) — dat houdt hem stabiel
  // zonder de vaste compensatie kwijt te raken.
  foutSom += fout;
  foutSom -= foutSom / 32;
  foutSom = constrain(foutSom, -RobotConfig::FOUT_SOM_MAX, RobotConfig::FOUT_SOM_MAX);

  int correctie = (RobotConfig::KP * fout)
                + (RobotConfig::KI * foutSom)
                + (RobotConfig::KD * dGefilterd);

  // Progressief sturen: hoe verder de lijn van de MIDDELSTE sensor af raakt,
  // hoe onevenredig harder terugsturen. Drijft hij af richting een
  // buitensensor (fout ~1500), dan stuurt hij ~2x zo hard als lineair; rond
  // het midden verandert er niets (factor ~1), dus geen getril.
  correctie = (int)(correctie * (1.0f + abs(fout) / 1500.0f));

  // In de na-bocht-cooldown gedempt sturen: de lijn zwiept door de restdraai
  // nog over de sensoren en volle correcties daarop = natrillen. Begrensde
  // correctie trekt hem rustig recht.
  if (naBocht) {
    correctie = constrain(correctie, -RobotConfig::NABOCHT_CORRECTIE_MAX,
                                      RobotConfig::NABOCHT_CORRECTIE_MAX);
  }

  // Adaptieve snelheid, maar GELEIDELIJK: bepaal het doel uit |fout|
  // (recht stuk = SNELHEID_RECHT, scherpe bocht = SNELHEID_LAAG) en kruip
  // daar per ronde naartoe. Remmen mag sneller dan optrekken, zodat hij
  // voor een bocht wel direct gas terugneemt maar nooit schokt.
  int doel = map(abs(fout), 0, 2000,
                 RobotConfig::SNELHEID_RECHT, RobotConfig::SNELHEID_LAAG);
  if (bochtAanstaande || naBocht) doel = RobotConfig::SNELHEID_LAAG;   // bocht nabij/net gehad: langzaam
  if (doel > basis) basis = min(doel, basis + 4);    // rustig optrekken
  else              basis = max(doel, basis - 12);   // vlot afremmen

  int links  = constrain(basis + correctie, -400, 400);
  int rechts = constrain(basis - correctie, -400, 400);

  robot.getHardware().setMotorSpeeds(links, rechts);
  vorigeFout = fout;
}

void LijnVolgenToestand::exit() {
  robot.getHardware().stopMotors();
}
