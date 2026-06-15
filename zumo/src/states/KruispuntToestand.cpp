#include "KruispuntToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "../config/Debug.h"
#include "LijnVolgenToestand.h"
#include "StartToestand.h"

// De richting komt mee uit LijnVolgenToestand (die zag het grijs het eerst).
// Zo gaat de informatie niet verloren als de markering bij de overdracht al
// gepasseerd is — herdetecteren hier was de grootste bron van gemiste afslagen.
KruispuntToestand::KruispuntToestand(ZumoRobot& robot, int richting)
: RobotToestand(robot) {
  grijsLinks  = (richting < 0);
  grijsRechts = (richting > 0);
}

void KruispuntToestand::enter() {
  grijsCm = robot.getSensorData().distanceCm;   // start van het grijs-venster (in cm)
  robot.getHardware().print("Kruispunt");
  if (grijsLinks)  DBG.println(F("[GRIJS] LINKS (van lijnvolger)"));
  if (grijsRechts) DBG.println(F("[GRIJS] RECHTS (van lijnvolger)"));
}

void KruispuntToestand::update() {
  // --- Nog een keer A = stoppen (zelfde vangnet als de lijnvolger) ---
  const SensorData& s = robot.getSensorData();
  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    robot.setState(new StartToestand(robot));
    return;   // oude toestand is hierna verwijderd
  }

  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // 1) Bezig met afslaan? Draai op de plek naar de gekozen tak, tot de robot
  //    ver genoeg gedraaid is (AFSLAG_MIN_GRADEN) EN de lijn weer in het midden
  //    ligt. AFSLAG_MS is een vangnet als de lijn nooit teruggevonden wordt.
  if (bezigMetAfslaan) {
    int positie = robot.getSensorData().linePosition;
    // DRAAI OP DE PLEK (echte pivot): gelijke, tegengestelde snelheden — net als
    // de scherpe-bocht-pivot in de lijnvolger. Op een haaks PLUS-kruispunt is dit
    // betrouwbaarder dan een boog: het draaipunt blijft op het kruispunt staan,
    // dus de robot pakt netjes de DWARStak. Een boog-afslag (vooruit + sturen)
    // liet hem juist langs de verkeerde lijn schampen en "terugkeren".
    int p = RobotConfig::SNELHEID_BOCHT;
    if (afslagRichting < 0) robot.getHardware().setMotorSpeeds(-p, p);   // links
    else                    robot.getHardware().setMotorSpeeds(p, -p);   // rechts

    // Gedraaide hoek uit de encoders (zelfde boog-odometrie als de lijnvolger-
    // bocht): de afslag wordt op HOEK gestuurd, niet op tijd, zodat het gedrag
    // los staat van de snelheid. Twee fasen hieronder (A blind, B lijn zoeken).
    unsigned long verstreken = millis() - afslagTijd;
    long dL = robot.getHardware().getTicksLinks()  - afslagTicksL;
    long dR = robot.getHardware().getTicksRechts() - afslagTicksR;
    float boogCm = ((labs(dL) + labs(dR)) / 2.0f) * RobotConfig::CM_PER_PULSE;
    int   graden = (int)(boogCm / (RobotConfig::WIELBASIS_CM / 2.0f) * 57.296f);

    // FASE A — BLIND DOORDRAAIEN tot AFSLAG_MIN_GRADEN.
    // Zolang de robot nog niet ver genoeg gedraaid is, NEGEREN we de
    // sensoren volledig: hij draait gewoon door. Dit is de kern van de fix
    // tegen "terugkeren". De rechtdoor-lijn (en de inkomende lijn) liggen
    // vlak na de start nog in/bij het midden; zouden we daar al een lijn
    // accepteren, dan pakt hij rechtdoor. Door blind door te draaien tot de
    // robot ECHT voorbij de rechtdoor-lijn staat, kan hij die niet meer
    // pakken — pas daarna (fase B) mag hij een lijn als de dwarstak zien.
    if (graden < RobotConfig::AFSLAG_MIN_GRADEN) {
      // Wel het tijd-vangnet laten gelden voor een vastgelopen draai.
      if (verstreken > RobotConfig::AFSLAG_MS) {
        DBG.println(F("[AFSLAG] klaar (tijd-vangnet, hoek niet gehaald)"));
        robot.getBaanPlan().rondAf(robot.getSensorData().distanceCm);
        robot.setState(new LijnVolgenToestand(robot));
      }
      return;
    }

    // FASE B — voorbij de minimumhoek: zoek de DWARStak.
    // Nu ligt de rechtdoor-lijn achter ons; de eerste lijn die nu netjes in
    // het midden komt, IS de gekozen tak -> overgeven aan de lijnvolger.
    unsigned int maxWAfslag = 0;
    for (int i = 0; i < 5; i++)
      if ((unsigned int)s.lineValues[i] > maxWAfslag) maxWAfslag = (unsigned int)s.lineValues[i];
    bool lijnGevonden = (maxWAfslag >= 200) && (abs(positie - 2000) < RobotConfig::AFSLAG_MIDDEN);
    // Vangnet: tijd verlopen, OF doorgedraaid voorbij de maximale afslaghoek
    // (de tak is gemist) -> toch overgeven zodat de lijnvolger kan herstellen.
    bool klaarOpTijd  = verstreken > RobotConfig::AFSLAG_MS;
    bool klaarOpMax   = graden >= RobotConfig::AFSLAG_MAX_GRADEN;
    if (lijnGevonden || klaarOpTijd || klaarOpMax) {
      DBG.print(F("[AFSLAG] klaar ("));
      DBG.print(lijnGevonden ? F("dwarstak gevonden") : (klaarOpMax ? F("max-hoek") : F("tijd-vangnet")));
      DBG.print(F(") na "));
      DBG.print(graden);
      DBG.println(F(" graden"));
      robot.getBaanPlan().rondAf(robot.getSensorData().distanceCm);   // kruispunt-checkpoint afgerond
      robot.setState(new LijnVolgenToestand(robot));
    }
    return;   // (na setState is deze toestand verwijderd)
  }

  // 2) Lijn (bijna) kwijt? Zelfde drempel als de lijnvolger (gekalibreerd
  //    < 200) — de oude check (een sensor > DREMPEL_ZWART=700) vond een
  //    dunne/vale lijn al "kwijt" en gooide dan de onthouden richting weg.
  //    MET een richting betekent lijn-weg hier: we staan op de knik van de
  //    splitsing -> direct afslaan die kant op (de richting NIET verliezen).
  //    Zonder richting: terug naar de lijnvolger (die heeft het herstel).
  unsigned int maxW = 0;
  for (int i = 0; i < 5; i++)
    if ((unsigned int)s.lineValues[i] > maxW) maxW = (unsigned int)s.lineValues[i];
  if (maxW < 200) {
    if (grijsLinks || grijsRechts) {
      bezigMetAfslaan = true;
      afslagRichting  = grijsLinks ? -1 : +1;
      afslagTijd      = millis();
      afslagTicksL    = robot.getHardware().getTicksLinks();    // nulpunt draaihoek
      afslagTicksR    = robot.getHardware().getTicksRechts();
      DBG.println(F("[AFSLAG] start (lijn weg op de splitsing)"));
    } else {
      robot.setState(new LijnVolgenToestand(robot));
    }
    return;
  }

  // 3) Grijs onthouden/bevestigen (met lock + ontdubbeling) — voor het geval
  //    de richting nog niet bekend was, of er een tweede markering komt.
  behandelMarkeringen();

  // 4) Kruising OF splitsing met een onthouden richting? Afslaan na
  //    SPLITS_BEVESTIG metingen OP RIJ (debounce tegen een losse brede-zwart-
  //    meting van een hobbel/schuine lijnrand).
  //    - isKruising(): de loodrechte plus-vorm (4+ sensoren + midden zwart) —
  //      het echte kruispunt geeft dit signaal het duidelijkst.
  //    - isSplitsing(): de Y/T-vorm (brede zwartzone + een buitenrand zwart).
  //    We vertrouwen de grijs-richting (die wordt betrouwbaar gedetecteerd) en
  //    slaan ALTIJD die kant op. GEEN tak-verificatie meer: die overschreef
  //    soms het correcte grijs als op het meetmoment maar één tak in beeld was
  //    -> verkeerde afslag. Richting komt puur uit het grijs.
  if ((lijn.isKruising() || lijn.isSplitsing()) && (grijsLinks || grijsRechts)) {
    if (++splitsTeller >= RobotConfig::SPLITS_BEVESTIG) {
      bezigMetAfslaan = true;
      afslagRichting  = grijsLinks ? -1 : +1;   // -1 = links, +1 = rechts
      afslagTijd      = millis();
      afslagTicksL    = robot.getHardware().getTicksLinks();    // nulpunt draaihoek
      afslagTicksR    = robot.getHardware().getTicksRechts();
      DBG.print(F("[AFSLAG] start richting="));
      DBG.println(afslagRichting < 0 ? F("LINKS") : F("RECHTS"));
      return;
    }
  } else {
    splitsTeller = 0;   // reeks onderbroken -> opnieuw tellen
  }

  // 5) Geen splitsing binnen de geldige AFSTAND? Grijs was loos -> terug.
  //    (Afstand i.p.v. tijd: zo telt het venster in cm baan, los van snelheid.)
  if (s.distanceCm - grijsCm > RobotConfig::GRIJS_GELDIG_CM) {
    robot.setState(new LijnVolgenToestand(robot));
    return;
  }

  // 6) Anders: de lijn blijven volgen tot de splitsing (langzamer: na grijs
  //    is SNELHEID_GRIJS verlaagd zodat de afslag beheerst genomen wordt).
  volgLijn();
}

void KruispuntToestand::exit() {}

// Lijn volgen tijdens het naderen (langzaam): continue P-sturing, zelfde
// stijl als de hoofdlijnvolger. Bewust NIET meer via stuurPD — die heeft
// nog een dode zone + eigen D-geheugen (ander rijgevoel, bang-bang-risico).
void KruispuntToestand::volgLijn() {
  int fout      = robot.getSensorData().linePosition - 2000;
  int correctie = (int)(RobotConfig::KP * fout);
  correctie = constrain(correctie, -RobotConfig::SNELHEID_BOCHT,
                                    RobotConfig::SNELHEID_BOCHT);
  robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_GRIJS + correctie,
                                     RobotConfig::SNELHEID_GRIJS - correctie);
}

// Onthoudt een grijze markering aan de zijkant.
void KruispuntToestand::behandelMarkeringen() {
  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // LOCK: net grijs gelatcht? Dan GRIJS_LOCK_CM lang geen nieuwe richting
  // aannemen, zodat hij niet meteen de andere kant detecteert en flipt.
  if ((grijsLinks || grijsRechts) &&
      robot.getSensorData().distanceCm - grijsCm < RobotConfig::GRIJS_LOCK_CM) {
    return;
  }

  // Links: pas onthouden na GRIJS_BEVESTIG metingen achter elkaar grijs.
  if (lijn.grijsTapeLinks()) {
    if (++grijsLinksTeller >= RobotConfig::GRIJS_BEVESTIG && !grijsLinks) {
      grijsLinks = true;
      grijsCm    = robot.getSensorData().distanceCm;
      DBG.println(F("[GRIJS] LINKS onthouden"));
    }
  } else {
    grijsLinksTeller = 0;
  }

  // Rechts: idem.
  if (lijn.grijsTapeRechts()) {
    if (++grijsRechtsTeller >= RobotConfig::GRIJS_BEVESTIG && !grijsRechts) {
      grijsRechts = true;
      grijsCm     = robot.getSensorData().distanceCm;
      DBG.println(F("[GRIJS] RECHTS onthouden"));
    }
  } else {
    grijsRechtsTeller = 0;
  }
}
