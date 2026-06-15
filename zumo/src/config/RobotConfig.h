// =============================================================
//  RobotConfig  <<utility>>
//  Alle instellingen op EEN plek: snelheden, kleurdrempels,
//  PID-waarden. Tijdens het testen pas je deze tientallen keren
//  aan; door ze hier te bundelen hoef je niet door de code te zoeken.
//  (Waarden zijn placeholders - afstellen tijdens het testen.)
// =============================================================
#pragma once

class RobotConfig {
public:
  // Periodieke sensor-debugregel (t=.. L=.. pos=..) in de Serial Monitor.
  // Event-logs ([PLAN], [GRIJS], [STIPPELLIJN], ...) blijven altijd aan.
  static constexpr bool DEBUG_SENSOR_PRINT = false;

  // --- Snelheden (0-400) — afstellen tijdens testen ---
  static constexpr int SNELHEID_MAX      = 400;
  static constexpr int SNELHEID_NORMAAL  = 250;   // basissnelheid op zwart (zet hoger voor vol gas)
  static constexpr int SNELHEID_GRIJS    = 100;   // flink afremmen na grijs-detectie tot en met de afslag (slomer = beheerster afslaan)
  static constexpr int SNELHEID_NADER    = 180;   // alvast afremmen ZODRA een kruispunt verwacht is (meer meettijd voor grijs)
  static constexpr int SNELHEID_GROEN    = 200;   // halve snelheid op groen (waarde uit main)
  static constexpr int SNELHEID_WIP      = 120;
  static constexpr int SNELHEID_ZOEKEN   = 150;
  static constexpr int SNELHEID_BOCHT    = 220;   // pivotsnelheid (bochten/afslaan) — rustiger = minder doorschieten

  // --- Adaptieve snelheid bij het lijnvolgen ---
  // De basissnelheid schaalt mee met hoe ver de lijn uit het midden ligt:
  // recht stuk (fout ~0) = RECHT, vlak voor/in een scherpe bocht = LAAG.
  static constexpr int SNELHEID_RECHT    = 300;   // vol vooruit op rechte stukken (rijdt dit goed? probeer 350)
  static constexpr int SNELHEID_LAAG     = 150;   // afgeremd in scherpe bochten
  static constexpr int KRUIS_MS          = 350;   // hoe lang pivoten op een kruispunt (ms)

  // --- Kleurdrempels op de GEKALIBREERDE waarden (0=wit .. 1000=zwart) ---
  // ALLES (lijnvolgen, grijs, kruispunt) werkt op dezelfde gekalibreerde
  // schaal. Die is drift-vast: accu/licht verschuiven de rauwe waarden, maar
  // na kalibratie landen wit/grijs/zwart altijd op dezelfde plek.
  // !! Afstellen via 'L=' in de Serial Monitor (toont nu gekalibreerd).
  // Weer AAN: het baanplan (BaanPlan) laat grijs alleen nog tellen als een
  // kruispunt ook echt de eerstvolgende checkpoint is, dus de valse
  // detecties op bochtranden kunnen geen toestand meer starten.
  static constexpr bool GRIJS_ACTIEF     = true;

  static constexpr int DREMPEL_ZWART     = 700;   // GEKALIBREERD: > dit = zwart
  static constexpr int DREMPEL_GRIJS_L   = 200;   // grijs-band — !! meet grijs (knop B) en stel bij
  static constexpr int DREMPEL_GRIJS_H   = 650;   // (650..700 = dode overgangszone, telt niet als grijs)
  static constexpr int GRIJS_BEVESTIG    = 2;     // in KruispuntToestand: 2 metingen op rij (was 1: te vals-gevoelig)
  // Overdracht naar het kruispunt pas na zoveel metingen ACHTER ELKAAR grijs,
  // en alleen als de lijn binnen +-dit van het midden ligt. Tegen valse
  // detecties: hoger = strenger (minder vals, maar grijs moet duidelijker).
  // LET OP: BEVESTIG_LIJN moet <= 2 blijven, anders wint de scherpe-bocht-
  // detectie (2 metingen) de race en wordt grijs vlak voor een knik gemist.
  static constexpr int GRIJS_BEVESTIG_LIJN = 2;   // opeenvolgende grijs-metingen nodig (MAX 2 — zie waarschuwing hierboven: 3 laat de bocht-detectie de race winnen)
  static constexpr int GRIJS_LIJN_MIDDEN   = 800; // lijn moet zo dicht bij het midden liggen (0..2000)
                                                  // (niet te krap: een echte markering trekt positie zelf al opzij)
  static constexpr int DREMPEL_CONTRAST  = 200;   // (ongebruikt; lijnvolgen draait op readLine)

  // Grijs werkt NET ALS ZWART: vaste band op de GEKALIBREERDE waarde (zie
  // DREMPEL_GRIJS_L/H hierboven). Na ijken met knop B geldt in plaats
  // daarvan: gemeten grijswaarde +- deze marge.
  static constexpr int GRIJS_MARGE = 200;   // band rond de geijkte grijswaarde
  // Vangnet: de markering-sensor moet ook zoveel BOVEN de laagste sensor
  // (het huidige wit) liggen — gekalibreerd zit wit bij ~0, dus dit vangt
  // vooral metingen af waarbij alles ongeveer even hoog is (geen markering).
  static constexpr int GRIJS_BOVEN_WIT = 200;   // 150->200: strenger zonder de bocht-race te raken (echte markering steekt ruim boven wit uit)
  // "Sterkste kant wint" was asymmetrisch (s0>=s4 vs s4>s0): leest de ene
  // buitensensor structureel iets hoger (sensorverschil/lichtval), dan vuurde
  // die kant nooit. Nu blokkeert een kant alleen als de ANDERE kant ECHT
  // sterker is (meer dan deze marge) — klein ruisverschil telt niet meer mee.
  static constexpr int GRIJS_KANT_MARGE = 150;

  // --- Afslaan op een splitsing (grijs = richtingshint) ---
  static constexpr int DREMPEL_SPLITS_MIN = 3;    // >= zoveel sensoren zwart + buitenrand zwart = splitsing
                                                  // (was 2: dan telt elke scherpe bocht al als splitsing)
  static constexpr int SPLITS_BEVESTIG    = 1;    // zoveel metingen OP RIJ kruising/splitsing zien voor de afslag start
                                                  // (1: een smalle dwarslijn op snelheid raakt de sensoren maar kort;
                                                  //  2 miste dat soms -> hij reed rechtdoor. Richting komt uit het grijs,
                                                  //  dus 1 trigger is genoeg om die kant op te draaien.)
  static constexpr int SPLITS_BIAS        = 2000; // (ONGEBRUIKT sinds de afslag een draai-op-de-plek werd i.p.v. een gestuurde boog)
  // Vensters in CENTIMETERS (encoders), niet in tijd: dan gedraagt de robot
  // zich hetzelfde ongeacht de snelheid van dat moment.
  static constexpr float GRIJS_GELDIG_CM = 40.0f; // onthouden richting vervalt na deze afstand
  static constexpr float GRIJS_LOCK_CM   = 5.0f;  // na een grijs-detectie zo lang GEEN nieuwe richting aannemen
  // AFSLAG blijft in tijd: tijdens de afslag draaien de motoren tegengesteld
  // (gemiddelde afstand ~0), dus daar zegt de encoder-afstand niets.
  static constexpr unsigned long AFSLAG_MS       = 1200; // MAX duur van de afslag-bias (puur vangnet als de lijn niet teruggevonden wordt).
                                                         // Ruim: de afslag eindigt normaal op de HOEK (AFSLAG_MIN_GRADEN + lijn
                                                         // in het midden). Te krap en het vangnet kapt de 90-graden-draai af
                                                         // VOOR de hoek bereikt is -> hij stopt alsnog op de rechtdoor-lijn.
  // Afslag in twee fasen, gestuurd op de gedraaide hoek (encoder-odometrie):
  //   FASE A: tot AFSLAG_MIN_GRADEN draait hij BLIND door — sensoren genegeerd.
  //   FASE B: daarboven pakt hij de eerste lijn-in-het-midden (= de dwarstak).
  // Op een PLUS-kruispunt loopt de lijn ook RECHTDOOR door. Die rechtdoor-lijn
  // ligt vlak na de start van de draai nog in/bij het midden; zou hij daar al
  // een lijn accepteren, dan "keert hij terug" naar rechtdoor. Door tot
  // AFSLAG_MIN_GRADEN blind door te draaien staat hij ECHT voorbij rechtdoor
  // voor hij iets accepteert. Voor een haakse (~90 graden) afslag moet deze
  // dus ruim voorbij het punt liggen waar rechtdoor uit beeld is: 75 graden.
  // Te laag = pakt rechtdoor (terugkeren); te hoog = draait voorbij de dwarstak.
  static constexpr int AFSLAG_MIN_GRADEN         = 75;
  // Bovengrens-vangnet: voorbij deze hoek is de dwarstak gemist -> toch
  // overgeven aan de lijnvolger (die herstelt). Ruim boven 90 graden zodat een
  // nette haakse afslag normaal op de LIJN eindigt, niet op dit vangnet.
  static constexpr int AFSLAG_MAX_GRADEN         = 120;
  static constexpr int AFSLAG_MIDDEN             = 400;  // lijn zo dicht bij het midden (0..2000) = afslag klaar
  // --- Groene lijn (module uit main) ---
  // Groen = donkerste sensor in deze GEKALIBREERDE band (tussen wit en zwart).
  // Zet GROEN_ACTIEF op false om de module snel uit te schakelen bij testen.
  static constexpr bool GROEN_ACTIEF     = true;
  static constexpr int DREMPEL_GROEN_L   = 250;   // 250 i.p.v. 200: marge boven de lijn-kwijt-drempel,
                                                  // anders flippert vaal groen tussen "groen" en "lijn kwijt"
  static constexpr int DREMPEL_GROEN_H   = 350;
  static constexpr int GROEN_BEVESTIG    = 3;    // x rondes groen zien voor de overgang
  static constexpr int DREMPEL_BRUIN_L   = 0;
  static constexpr int DREMPEL_BRUIN_H   = 0;

  // --- Timing & afstand ---
  static constexpr int   GEHEUGEN_MS     = 0;
  static constexpr int   DOORRIJDEN_CM   = 0;
  // Door het team gemeten en getest: 0.019 cm per encodertick/pulse.
  // (afstand = gemiddelde van links+rechts ticks * deze waarde)
  static constexpr float CM_PER_PULSE    = 0.019f;

  // --- PD-regeling (fout = lijnpositie - 2000, bereik +-2000) — branch-waarden ---
  // KP lager = rustiger sturen; KD hoger = meer demping (minder slingeren).
  static constexpr float KP = 0.70f;  // hoger = sneller/strakker corrigeren (te hoog = slingeren)
  static constexpr float KD = 2.0f;   // demping schaalt mee met KP (~3x KP houden)
  // I-term: regelt de kleine rest-afwijking weg zodat hij echt op het MIDDEN
  // van de lijn rijdt. De som is begrensd (anti-windup) zodat hij in een
  // lange bocht niet "oplaadt" en daarna doorschiet.
  static constexpr float KI           = 0.012f;  // was 0.008: trekt sneller naar het exacte midden
  static constexpr long  FOUT_SOM_MAX = 15000;   // max bijdrage = KI * dit = 120
  static constexpr int   LIJN_DODEZONE = 200;  // binnen +-deze fout: rechtdoor (niet bijsturen)

  // Scherpe punt-bocht herkennen: bij een scherpe hoek (zoals de zigzag) ziet
  // readLine de inkomende EN uitgaande lijn en middelt dat tot het midden, dus
  // de PD stuurt te weinig en hij rijdt rechtdoor. Ziet precies ÉÉN buitenrand-
  // sensor (0 of 4) sterker zwart dan dit (gekalibreerd, ~0=wit..1000=zwart)
  // terwijl de andere kant wit is, dan buigt de lijn die kant op -> hard draaien.
  static constexpr int BOCHT_RAND_ZWART = 500;

  // Zag een buitensensor (0 of 4) korter dan dit geleden zwart (in cm
  // afgelegde weg), dan is een bocht "aanstaande": afremmen, stippellijn-
  // stand geblokkeerd, en pivoteren zodra de lijn echt wegdraait/verdwijnt.
  static constexpr float BOCHT_AANKONDIG_CM = 10.0f;

  // --- Bocht-odometrie (encoders) ---
  // Tijdens een scherpe-bocht-pivot meten de encoders hoeveel graden er al
  // gedraaid is. Pas na MIN_GRADEN mag de bocht "klaar" zijn (anders accepteert
  // hij direct de oude lijn die nog onder de sensoren ligt), en boven
  // MAX_GRADEN wordt de pivot afgebroken — doordraaien betekent de INKOMENDE
  // lijn pakken en via dezelfde kant terugrijden.
  static constexpr float WIELBASIS_CM     = 8.5f;  // afstand tussen de rupsbanden
  static constexpr int   BOCHT_MIN_GRADEN = 20;    // minimaal zoveel draaien voor "klaar"
  static constexpr int   BOCHT_MAX_GRADEN = 135;   // hierboven = doorgedraaid -> afbreken
  // Een aangekondigde bocht (buitenrand zag zwart) start de pivot pas als de
  // lijnpositie zo ver uit het midden slaat (0..2000). Te vroeg draaien =
  // naast de knik draaien en de bocht missen; te laat = eroverheen schieten.
  // (1000 i.p.v. 1200: op de knik ligt de uitgaande lijn over sensor 0..2 en
  // komt de gewogen positie vaak niet verder dan ~1000 uit het midden.)
  static constexpr int   BOCHT_START_FOUT = 1000;
  // Ziet een buitensensor in EEN meting al bijna vol zwart, dan is de bocht
  // direct aangekondigd (geen 2e meting nodig — anders mist hij op hoge
  // snelheid de korte aanraking van de dwarslijn).
  static constexpr int   BOCHT_RAND_DIRECT = 800;

  // --- Na-bocht-cooldown ---
  // Direct na een pivot zwiept de lijn door de restdraai nog over de
  // sensoren. Zonder cooldown ziet de bocht-detectie dat aan voor een NIEUWE
  // bocht en start hij een tegenpivot: pingpong = "trillen na de bocht".
  // Zo lang (cm) na een pivot: geen nieuwe aankondiging/pivot-start, sturen
  // begrensd en snelheid laag, zodat hij zich eerst recht trekt.
  static constexpr float NABOCHT_CM            = 8.0f;
  static constexpr int   NABOCHT_CORRECTIE_MAX = 150;

  // --- Stippellijn-zone (meerdere streepjes per checkpoint) ---
  // Een stippellijn = onderbroken lijn met meerdere witte gaten. De zone
  // start bij het eerste gat (als stippellijn verwacht is) en overbrugt ALLE
  // gaten rechtdoor. De zone (en het checkpoint) is pas klaar als de lijn
  // weer STIPPEL_EINDE_CM aaneengesloten zwart is — dan zijn de streepjes
  // voorbij. Vangnet: duurt de zone langer dan STIPPEL_ZONE_MAX_CM zonder dat
  // einde, dan was het geen stippellijn maar een gemiste bocht -> pivoteren.
  static constexpr float STIPPEL_EINDE_CM    = 8.0f;   // zoveel cm doorlopend zwart = streepjes voorbij
  static constexpr float STIPPEL_ZONE_MAX_CM = 60.0f;  // zone langer dan dit = geen stippellijn (abort)

  // Zijgeheugen: lag de lijn korter dan dit aantal cm geleden DUIDELIJK aan
  // een kant (sterkste sensor = een buitensensor), en is de lijn nu weg?
  // Dan die kant op draaien — daar knikte hij heen. In cm (encoders), zodat
  // het venster hetzelfde stuk baan beslaat ongeacht de snelheid. Alleen de
  // BUITENSTE sensoren tellen: gewoon licht slingeren (sensor 1/3) mag de
  // stippellijn-overbrugging niet verdringen.
  static constexpr float LIJN_KWIJT_GEHEUGEN_CM = 4.0f;

  // --- Baanplan ---
  // Minimale afstand tussen twee checkpoint-afrondingen: voorkomt dubbel
  // tellen (groen-flapje, twee gaten van dezelfde stippellijn vlak na elkaar).
  static constexpr float CP_MIN_AFSTAND_CM = 15.0f;

  // TIJDELIJK: bij welk checkpoint begint het plan? 0 = normaal vanaf het
  // begin (stippellijn 1). Zet op 4 om bij het eerste KRUISPUNT (grijze lijn,
  // checkpoint 5 in het plan) te starten — handig om daar los te testen.
  // (Index, dus 0-gebaseerd: checkpoint N in het plan = index N-1.)
  static constexpr int START_CHECKPOINT = 0;

  // Helling/wip-checkpoint via de versnellingsmeter. LET OP: hard remmen
  // leest ook als ~10 graden "kantelen" (een accelerometer kan dat niet
  // onderscheiden), dus de drempel moet daar ruim boven en de bevestiging
  // lang genoeg om een remstoot te overleven.
  static constexpr float PITCH_GRADEN   = 13.0f;
  static constexpr int   PITCH_BEVESTIG = 20;     // rondes op rij gekanteld
};
