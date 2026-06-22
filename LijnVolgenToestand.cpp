#include "LijnVolgenToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "StartToestand.h"
#include "KruispuntToestand.h"
#include "GroeneLijnToestand.h"
#include "WipWachtToestand.h"

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void LijnVolgenToestand::enter() {
  robot.getHardware().print("Lijn volgen");
  vorigeFout  = 0;
  dGefilterd  = 0;
  foutSom     = 0;
  basis       = RobotConfig::SNELHEID_LAAG;
  aLosgelaten = false;
  inBocht     = false;
  bochtKant   = 0;
  randTeller  = 0;
  grijsTeller = 0;
  groenTeller = 0;
}

void LijnVolgenToestand::update() {
  const SensorData& s = robot.getSensorData();

  // --- Nog een keer A = stoppen ---
  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    robot.setState(new StartToestand(robot));
    return;
  }

  // --- Sensorwaarden uit snapshot halen ---
  int positie = s.linePosition;
  for (int i = 0; i < 5; i++) {
    sensorWaarden[i] = (unsigned int)s.lineValues[i];
  }

  // Sterkste sensor zoeken.
  unsigned int maxWaarde = 0;
  int maxIndex = 0;

  for (int i = 0; i < 5; i++) {
    if (sensorWaarden[i] > maxWaarde) {
      maxWaarde = sensorWaarden[i];
      maxIndex = i;
    }
  }

  const unsigned int DREMPEL_LIJN = 150;

  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // =============================================================
  //  WIP: STOPPEN BIJ LINKS EN RECHTS GRIJS
  // =============================================================
  // BELANGRIJK:
  // Hier gebruiken we GEEN pitch.
  // De pitch komt pas in WipWachtToestand, nadat hij kort is doorgereden.
  //
  // Links grijs = sensor 0 OF 1.
  // Rechts grijs = sensor 3 OF 4.
  // Alleen als links EN rechts grijs gezien zijn, gaat hij naar WipWacht.
  // =============================================================

  bool linksGrijs =
    (
      sensorWaarden[0] >= RobotConfig::DREMPEL_GRIJS_L &&
      sensorWaarden[0] <= RobotConfig::DREMPEL_GRIJS_H &&
      sensorWaarden[0] <  RobotConfig::DREMPEL_ZWART
    ) ||
    (
      sensorWaarden[1] >= RobotConfig::DREMPEL_GRIJS_L &&
      sensorWaarden[1] <= RobotConfig::DREMPEL_GRIJS_H &&
      sensorWaarden[1] <  RobotConfig::DREMPEL_ZWART
    );

  bool rechtsGrijs =
    (
      sensorWaarden[3] >= RobotConfig::DREMPEL_GRIJS_L &&
      sensorWaarden[3] <= RobotConfig::DREMPEL_GRIJS_H &&
      sensorWaarden[3] <  RobotConfig::DREMPEL_ZWART
    ) ||
    (
      sensorWaarden[4] >= RobotConfig::DREMPEL_GRIJS_L &&
      sensorWaarden[4] <= RobotConfig::DREMPEL_GRIJS_H &&
      sensorWaarden[4] <  RobotConfig::DREMPEL_ZWART
    );

  // Kort geheugen: soms ziet hij eerst links en meteen daarna rechts.
  // Dan telt het nog steeds als "beide kanten".
  static float wipLinksGezienCm  = -1000.0f;
  static float wipRechtsGezienCm = -1000.0f;
  static float laatsteWipStartCm = -1000.0f;

  const float WIP_ZIJDEN_VENSTER_CM = 6.0f;

  if (linksGrijs) {
    wipLinksGezienCm = s.distanceCm;
  }

  if (rechtsGrijs) {
    wipRechtsGezienCm = s.distanceCm;
  }

  bool linksRecent = (wipLinksGezienCm >= 0.0f) &&
                     (s.distanceCm - wipLinksGezienCm <= WIP_ZIJDEN_VENSTER_CM);

  bool rechtsRecent = (wipRechtsGezienCm >= 0.0f) &&
                      (s.distanceCm - wipRechtsGezienCm <= WIP_ZIJDEN_VENSTER_CM);

  bool grijsBeideKanten = linksRecent && rechtsRecent;

  bool wipCooldownVoorbij = (s.distanceCm - laatsteWipStartCm > 20.0f);

  if (!inBocht && wipCooldownVoorbij && grijsBeideKanten) {
    laatsteWipStartCm = s.distanceCm;
    wipLinksGezienCm  = -1000.0f;
    wipRechtsGezienCm = -1000.0f;

    robot.getHardware().print("Wip wacht");
    robot.setState(new WipWachtToestand(robot));
    return;
  }

  // --- Zijgeheugen bijhouden ---
  if (maxWaarde >= DREMPEL_LIJN && (maxIndex == 0 || maxIndex == 4)) {
    zwartZijKant = (maxIndex == 0) ? -1 : +1;
    zwartZijCm   = s.distanceCm;
  }

  // =============================================================
  //  NORMALE GRIJS-DETECTIE VOOR KRUISPUNT
  // =============================================================

  bool kruispuntVerwacht = robot.getBaanPlan().isVerwacht(CP_KRUISPUNT, s.distanceCm);

  bool grijsL = RobotConfig::GRIJS_ACTIEF && kruispuntVerwacht && lijn.grijsTapeLinks();
  bool grijsR = RobotConfig::GRIJS_ACTIEF && kruispuntVerwacht && lijn.grijsTapeRechts();

  bool lijnRecht = (maxWaarde >= DREMPEL_LIJN)
                && (abs(positie - 2000) < RobotConfig::GRIJS_LIJN_MIDDEN);

  if (!inBocht && lijnRecht && (grijsL || grijsR)) {
    if (++grijsTeller >= RobotConfig::GRIJS_BEVESTIG_LIJN) {
      robot.setState(new KruispuntToestand(robot, grijsL ? -1 : +1));
      return;
    }
  } else {
    grijsTeller = 0;
  }

  // --- Bezig een scherpe bocht uit te draaien? ---
  if (inBocht) {
    long dL = robot.getHardware().getTicksLinks()  - bochtTicksL;
    long dR = robot.getHardware().getTicksRechts() - bochtTicksR;

    float boogCm = ((abs(dL) + abs(dR)) / 2.0f) * RobotConfig::CM_PER_PULSE;
    int graden = (int)(boogCm / (RobotConfig::WIELBASIS_CM / 2.0f) * 57.296f);

    if (graden > RobotConfig::BOCHT_MAX_GRADEN) {
      inBocht     = false;
      vorigeFout  = 0;
      dGefilterd  = 0;
      foutSom     = 0;
      basis       = RobotConfig::SNELHEID_LAAG;
      randZwartCm = -1000.0f;
      naBochtCm   = s.distanceCm;

      robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_LAAG,
                                         RobotConfig::SNELHEID_LAAG);
      return;
    }

    int p = RobotConfig::SNELHEID_BOCHT;
    bool klaar = false;

    if (bochtKant < 0) {
      robot.getHardware().setMotorSpeeds(-p, p);
      klaar = (maxWaarde >= DREMPEL_LIJN && maxIndex >= 2);
    } else {
      robot.getHardware().setMotorSpeeds(p, -p);
      klaar = (maxWaarde >= DREMPEL_LIJN && maxIndex <= 2);
    }

    klaar = klaar && (graden >= RobotConfig::BOCHT_MIN_GRADEN);

    if (!klaar) {
      return;
    }

    inBocht     = false;
    vorigeFout  = positie - 2000;
    dGefilterd  = 0;
    foutSom     = 0;
    basis       = RobotConfig::SNELHEID_LAAG;
    naBochtCm   = s.distanceCm;
    randZwartCm = -1000.0f;
  }

  // --- Na-bocht-cooldown ---
  bool naBocht = (naBochtCm >= 0.0f) &&
                 (s.distanceCm - naBochtCm < RobotConfig::NABOCHT_CM);

  // --- Scherpe bocht aankondigen ---
  bool randLinksZwart  = sensorWaarden[0] > (unsigned int)RobotConfig::BOCHT_RAND_ZWART;
  bool randRechtsZwart = sensorWaarden[4] > (unsigned int)RobotConfig::BOCHT_RAND_ZWART;

  if (!naBocht && (randLinksZwart != randRechtsZwart)) {
    int kant = randLinksZwart ? -1 : +1;

    randTeller = (kant == randZwartKant) ? randTeller + 1 : 1;
    randZwartKant = kant;

    unsigned int randWaarde = randLinksZwart ? sensorWaarden[0] : sensorWaarden[4];

    if (randTeller >= 2 || randWaarde > (unsigned int)RobotConfig::BOCHT_RAND_DIRECT) {
      randZwartCm = s.distanceCm;
    }
  } else {
    randTeller = 0;
  }

  bool bochtAanstaande = (randZwartCm >= 0.0f) &&
                         (s.distanceCm - randZwartCm < RobotConfig::BOCHT_AANKONDIG_CM);

  // --- Groene lijn ---
  if (RobotConfig::GROEN_ACTIEF &&
      !bochtAanstaande &&
      !naBocht &&
      robot.getBaanPlan().isVerwacht(CP_GROEN, s.distanceCm) &&
      maxWaarde >= DREMPEL_LIJN &&
      robot.getLijnAnalyse().isGroeneLijn()) {

    if (++groenTeller >= RobotConfig::GROEN_BEVESTIG) {
      robot.setState(new GroeneLijnToestand(robot));
      return;
    }
  } else {
    groenTeller = 0;
  }

  // --- Lijn helemaal kwijt? ---
  if (maxWaarde < DREMPEL_LIJN) {
    bool stippellijnVerwacht = robot.getBaanPlan().isVerwacht(CP_STIPPELLIJN, s.distanceCm);

    if (stippellijnVerwacht || inStippelZone) {
      if (!inStippelZone) {
        inStippelZone = true;
        stippelZoneStartCm = s.distanceCm;
      }

      stippelGatCm = s.distanceCm;

      if (s.distanceCm - stippelZoneStartCm > RobotConfig::STIPPEL_ZONE_MAX_CM) {
        inStippelZone = false;
      } else {
        robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_LAAG,
                                           RobotConfig::SNELHEID_LAAG);
        return;
      }
    }

    bool zijNetGezien =
      (zwartZijKant != 0) &&
      (zwartZijCm >= 0.0f) &&
      (s.distanceCm - zwartZijCm < RobotConfig::LIJN_KWIJT_GEHEUGEN_CM);

    inBocht = true;

    if (bochtAanstaande) {
      bochtKant = randZwartKant;
    } else if (zijNetGezien) {
      bochtKant = zwartZijKant;
    } else if (abs(vorigeFout) > 1000) {
      bochtKant = (vorigeFout < 0) ? -1 : +1;
    } else {
      bochtKant = (foutSom < 0) ? -1 : +1;
    }

    bochtTicksL = robot.getHardware().getTicksLinks();
    bochtTicksR = robot.getHardware().getTicksRechts();

    int p = RobotConfig::SNELHEID_BOCHT;

    if (bochtKant < 0) {
      robot.getHardware().setMotorSpeeds(-p, p);
    } else {
      robot.getHardware().setMotorSpeeds(p, -p);
    }

    return;
  }

  // --- Lijn in beeld ---
  if (inStippelZone) {
    if (s.distanceCm - stippelGatCm > RobotConfig::STIPPEL_EINDE_CM) {
      inStippelZone = false;

      if (robot.getBaanPlan().isVerwacht(CP_STIPPELLIJN, s.distanceCm)) {
        robot.getBaanPlan().rondAf(s.distanceCm);
      }
    }
  }

  // --- Alleen OMHOOG via pitch afvinken ---
  Checkpoint vw = robot.getBaanPlan().verwacht();

  if (vw == CP_OMHOOG && fabs(s.pitch) > RobotConfig::PITCH_GRADEN) {
    if (++pitchTeller >= RobotConfig::PITCH_BEVESTIG) {
      robot.getBaanPlan().rondAf(s.distanceCm);
      pitchTeller = 0;
    }
  } else {
    pitchTeller = 0;
  }

  // --- PD-lijnvolgen ---
  int fout = positie - 2000;

  if (!naBocht && bochtAanstaande && abs(fout) >= RobotConfig::BOCHT_START_FOUT) {
    inBocht = true;
    bochtKant = randZwartKant;

    bochtTicksL = robot.getHardware().getTicksLinks();
    bochtTicksR = robot.getHardware().getTicksRechts();

    int p = RobotConfig::SNELHEID_BOCHT;

    if (bochtKant < 0) {
      robot.getHardware().setMotorSpeeds(-p, p);
    } else {
      robot.getHardware().setMotorSpeeds(p, -p);
    }

    return;
  }

  int afgeleide = fout - vorigeFout;
  dGefilterd = (dGefilterd + afgeleide) / 2;

  foutSom += fout;
  foutSom -= foutSom / 32;
  foutSom = constrain(foutSom, -RobotConfig::FOUT_SOM_MAX, RobotConfig::FOUT_SOM_MAX);

  int correctie =
    (RobotConfig::KP * fout) +
    (RobotConfig::KI * foutSom) +
    (RobotConfig::KD * dGefilterd);

  correctie = (int)(correctie * (1.0f + abs(fout) / 1500.0f));

  if (naBocht) {
    correctie = constrain(correctie,
                          -RobotConfig::NABOCHT_CORRECTIE_MAX,
                           RobotConfig::NABOCHT_CORRECTIE_MAX);
  }

  int doel = map(abs(fout),
                 0,
                 2000,
                 RobotConfig::SNELHEID_RECHT,
                 RobotConfig::SNELHEID_LAAG);

  if (bochtAanstaande || naBocht) {
    doel = RobotConfig::SNELHEID_LAAG;
  }

  if (doel > basis) {
    basis = min(doel, basis + 4);
  } else {
    basis = max(doel, basis - 12);
  }

  int links  = constrain(basis + correctie, -400, 400);
  int rechts = constrain(basis - correctie, -400, 400);

  robot.getHardware().setMotorSpeeds(links, rechts);
  vorigeFout = fout;
}

void LijnVolgenToestand::exit() {
  robot.getHardware().stopMotors();
}
