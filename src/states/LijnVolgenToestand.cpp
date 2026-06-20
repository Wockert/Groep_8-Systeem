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
  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    robot.setState(new StartToestand(robot));
    return;
  }

  int positie = s.linePosition;
  for (int i = 0; i < 5; i++) sensorWaarden[i] = (unsigned int)s.lineValues[i];

  // Donkerste sensor opzoeken (max waarde + index).
  unsigned int maxWaarde = 0;
  int          maxIndex  = 0;
  for (int i = 0; i < 5; i++)
    if (sensorWaarden[i] > maxWaarde) { maxWaarde = sensorWaarden[i]; maxIndex = i; }

  const unsigned int DREMPEL_LIJN = 150;

  // Onthoud zwart aan een buitensensor: hint voor een aankomende scherpe bocht.
  if (maxWaarde >= DREMPEL_LIJN && (maxIndex == 0 || maxIndex == 4)) {
    zwartZijKant = (maxIndex == 0) ? -1 : +1;
    zwartZijCm   = s.distanceCm;
  }

  // Grijze markering bij een verwacht kruispunt -> overgaan naar KruispuntToestand.
  bool kruispuntVerwacht = robot.getBaanPlan().isVerwacht(CP_KRUISPUNT, s.distanceCm);
  LineSensorAnalyse& lijn = robot.getLijnAnalyse();
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

  // Bezig met een pivot-bocht: draai door tot de lijn weer in het midden zit (of de max-hoek is bereikt).
  if (inBocht) {
    // Gedraaide hoek schatten uit het verschil in encoderstanden sinds de bocht begon.
    long dL = robot.getHardware().getTicksLinks()  - bochtTicksL;
    long dR = robot.getHardware().getTicksRechts() - bochtTicksR;
    float boogCm = ((abs(dL) + abs(dR)) / 2.0f) * RobotConfig::CM_PER_PULSE;
    int   graden = (int)(boogCm / (RobotConfig::WIELBASIS_CM / 2.0f) * 57.296f);

    if (graden > RobotConfig::BOCHT_MAX_GRADEN) {
      inBocht       = false;
      vorigeFout    = 0;
      dGefilterd    = 0;
      foutSom       = 0;
      basis         = RobotConfig::SNELHEID_LAAG;
      randZwartCm   = -1000.0f;
      naBochtCm     = s.distanceCm;
      robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_LAAG,
                                         RobotConfig::SNELHEID_LAAG);
      return;
    }

    int p = RobotConfig::SNELHEID_BOCHT;
    bool klaar;
    if (bochtKant < 0) {
      robot.getHardware().setMotorSpeeds(-p, p);
      klaar = (maxWaarde >= DREMPEL_LIJN && maxIndex >= 2);
    } else {
      robot.getHardware().setMotorSpeeds(p, -p);
      klaar = (maxWaarde >= DREMPEL_LIJN && maxIndex <= 2);
    }
    klaar = klaar && (graden >= RobotConfig::BOCHT_MIN_GRADEN);
    if (!klaar) return;
    inBocht    = false;
    vorigeFout = positie - 2000;
    dGefilterd = 0;
    foutSom    = 0;
    basis      = RobotConfig::SNELHEID_LAAG;
    naBochtCm  = s.distanceCm;
    randZwartCm = -1000.0f;
  }

  bool naBocht = (naBochtCm >= 0.0f) &&
                 (s.distanceCm - naBochtCm < RobotConfig::NABOCHT_CM);

  // Bocht aankondigen: één buitenrand wordt zwart (na een paar bevestigingen of bij heel sterk zwart).
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

  // Groene lijn bij een verwacht groen-checkpoint -> overgaan naar GroeneLijnToestand.
  if (RobotConfig::GROEN_ACTIEF && !bochtAanstaande && !naBocht
      && robot.getBaanPlan().isVerwacht(CP_GROEN, s.distanceCm)
      && maxWaarde >= DREMPEL_LIJN && robot.getLijnAnalyse().isGroeneLijn()) {
    if (++groenTeller >= RobotConfig::GROEN_BEVESTIG) {
      Serial.print(F("[GROEN] gedetecteerd (max="));
      Serial.print(maxWaarde);
      Serial.println(F(") -> GroeneLijnToestand"));
      robot.setState(new GroeneLijnToestand(robot));
      return;
    }
  } else {
    groenTeller = 0;
  }

  // Lijn kwijt: of een bocht inzetten (recent zwart aan de zijkant / aangekondigd), of het gat overbruggen.
  if (maxWaarde < DREMPEL_LIJN) {
    bool zijNetGezien = (zwartZijKant != 0) && (zwartZijCm >= 0.0f) &&
                        (s.distanceCm - zwartZijCm < RobotConfig::LIJN_KWIJT_GEHEUGEN_CM);

    if (bochtAanstaande || zijNetGezien) {
      inBocht   = true;
      bochtKant = bochtAanstaande ? randZwartKant : zwartZijKant;
      bochtTicksL = robot.getHardware().getTicksLinks();
      bochtTicksR = robot.getHardware().getTicksRechts();
      int p = RobotConfig::SNELHEID_BOCHT;
      if (bochtKant < 0) robot.getHardware().setMotorSpeeds(-p, p);
      else               robot.getHardware().setMotorSpeeds(p, -p);
      return;
    }

    if (!inStippelZone) {
      inStippelZone      = true;
      stippelZoneStartCm = s.distanceCm;
      Serial.println(F("[GAT] lijn kwijt zonder bocht -> rechtdoor overbruggen"));
    }
    stippelGatCm = s.distanceCm;

    // Te lang geen lijn gezien: alsnog gaan zoeken met een pivot, richting op basis van de laatste fout.
    if (s.distanceCm - stippelZoneStartCm > RobotConfig::STIPPEL_ZONE_MAX_CM) {
      inStippelZone = false;
      inBocht       = true;
      bochtKant     = (foutSom < 0) ? -1 : +1;
      bochtTicksL   = robot.getHardware().getTicksLinks();
      bochtTicksR   = robot.getHardware().getTicksRechts();
      int p = RobotConfig::SNELHEID_BOCHT;
      if (bochtKant < 0) robot.getHardware().setMotorSpeeds(-p, p);
      else               robot.getHardware().setMotorSpeeds(p, -p);
      Serial.println(F("[GAT] te lang geen lijn -> alsnog zoeken (pivot)"));
      return;
    }

    robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_LAAG,
                                       RobotConfig::SNELHEID_LAAG);
    return;
  }

  if (inStippelZone) {
    if (s.distanceCm - stippelGatCm > RobotConfig::STIPPEL_EINDE_CM) {
      inStippelZone = false;
      Serial.println(F("[GAT] lijn weer doorlopend -> gat-zone uit"));
      if (robot.getBaanPlan().isVerwacht(CP_STIPPELLIJN, s.distanceCm)) {
        robot.getBaanPlan().rondAf(s.distanceCm);
      }
    }
  }

  // Helling-checkpoint (omhoog/wip): bevestig via de pitch en rond het checkpoint af.
  Checkpoint vw = robot.getBaanPlan().verwacht();
  if ((vw == CP_OMHOOG || vw == CP_WIP) && fabs(s.pitch) > RobotConfig::PITCH_GRADEN) {
    if (++pitchTeller >= RobotConfig::PITCH_BEVESTIG) {
      robot.getBaanPlan().rondAf(s.distanceCm);
      pitchTeller = 0;
    }
  } else {
    pitchTeller = 0;
  }

  int fout = positie - 2000;

  if (!naBocht && bochtAanstaande && abs(fout) >= RobotConfig::BOCHT_START_FOUT) {
    inBocht   = true;
    bochtKant = randZwartKant;
    bochtTicksL = robot.getHardware().getTicksLinks();
    bochtTicksR = robot.getHardware().getTicksRechts();
    int p = RobotConfig::SNELHEID_BOCHT;
    if (bochtKant < 0) robot.getHardware().setMotorSpeeds(-p, p);
    else               robot.getHardware().setMotorSpeeds(p, -p);
    return;
  }

  // PID-regeling: P op de fout, I op de (afnemende) foutsom, D op de gefilterde afgeleide.
  int afgeleide = fout - vorigeFout;
  dGefilterd    = (dGefilterd + afgeleide) / 2;

  foutSom += fout;
  foutSom -= foutSom / 32;
  foutSom = constrain(foutSom, -RobotConfig::FOUT_SOM_MAX, RobotConfig::FOUT_SOM_MAX);

  int correctie = (RobotConfig::KP * fout)
                + (RobotConfig::KI * foutSom)
                + (RobotConfig::KD * dGefilterd);

  // Bij grotere fout sterker bijsturen.
  correctie = (int)(correctie * (1.0f + abs(fout) / 1500.0f));

  if (naBocht) {
    correctie = constrain(correctie, -RobotConfig::NABOCHT_CORRECTIE_MAX,
                                      RobotConfig::NABOCHT_CORRECTIE_MAX);
  }

  // Doelsnelheid: hard op recht stuk, langzamer bij grote fout / rond bochten; geleidelijk laten meebewegen.
  int doel = map(abs(fout), 0, 2000,
                 RobotConfig::SNELHEID_RECHT, RobotConfig::SNELHEID_LAAG);
  if (bochtAanstaande || naBocht) doel = RobotConfig::SNELHEID_LAAG;
  if (doel > basis) basis = min(doel, basis + 4);
  else              basis = max(doel, basis - 12);

  int links  = constrain(basis + correctie, -400, 400);
  int rechts = constrain(basis - correctie, -400, 400);

  robot.getHardware().setMotorSpeeds(links, rechts);
  vorigeFout = fout;
}

void LijnVolgenToestand::exit() {
  robot.getHardware().stopMotors();
}
