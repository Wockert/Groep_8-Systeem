#include "KruispuntToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "LijnVolgenToestand.h"
#include "StartToestand.h"

KruispuntToestand::KruispuntToestand(ZumoRobot& robot, int richting)
: RobotToestand(robot) {
  grijsLinks  = (richting < 0);
  grijsRechts = (richting > 0);
}

void KruispuntToestand::enter() {
  grijsCm = robot.getSensorData().distanceCm;
  robot.getHardware().print("Kruispunt");
  if (grijsLinks)  Serial.println(F("[GRIJS] LINKS (van lijnvolger)"));
  if (grijsRechts) Serial.println(F("[GRIJS] RECHTS (van lijnvolger)"));
}

void KruispuntToestand::update() {
  const SensorData& s = robot.getSensorData();
  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    robot.setState(new StartToestand(robot));
    return;
  }

  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // Afslaan: stuur met een vaste bias naar de gekozen kant tot de afslagtijd om is.
  if (bezigMetAfslaan) {
    int positie   = robot.getSensorData().linePosition;
    int fout      = (positie - 2000) + afslagRichting * RobotConfig::SPLITS_BIAS;
    int correctie = (int)(RobotConfig::KP * fout);
    correctie = constrain(correctie, -RobotConfig::SNELHEID_BOCHT,
                                      RobotConfig::SNELHEID_BOCHT);
    robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_GRIJS + correctie,
                                       RobotConfig::SNELHEID_GRIJS - correctie);
    if (millis() - afslagTijd > RobotConfig::AFSLAG_MS) {
      Serial.println(F("[AFSLAG] klaar"));
      robot.getBaanPlan().rondAf(robot.getSensorData().distanceCm);
      robot.setState(new LijnVolgenToestand(robot));
    }
    return;
  }

  // Lijn weg op de splitsing: afslaan als een kant onthouden is, anders gewoon verder lijnvolgen.
  unsigned int maxW = 0;
  for (int i = 0; i < 5; i++)
    if ((unsigned int)s.lineValues[i] > maxW) maxW = (unsigned int)s.lineValues[i];
  if (maxW < 200) {
    if (grijsLinks || grijsRechts) {
      bezigMetAfslaan = true;
      afslagRichting  = grijsLinks ? -1 : +1;
      afslagTijd      = millis();
      Serial.println(F("[AFSLAG] start (lijn weg op de splitsing)"));
    } else {
      robot.setState(new LijnVolgenToestand(robot));
    }
    return;
  }

  behandelMarkeringen();

  if (lijn.isSplitsing() && (grijsLinks || grijsRechts)) {
    if (++splitsTeller >= RobotConfig::SPLITS_BEVESTIG) {
      bezigMetAfslaan = true;
      afslagRichting  = grijsLinks ? -1 : +1;
      afslagTijd      = millis();
      Serial.println(F("[AFSLAG] start"));
      return;
    }
  } else {
    splitsTeller = 0;
  }

  // Geen afslag binnen de geldige afstand: terug naar de normale lijnvolger.
  if (s.distanceCm - grijsCm > RobotConfig::GRIJS_GELDIG_CM) {
    robot.setState(new LijnVolgenToestand(robot));
    return;
  }

  volgLijn();
}

void KruispuntToestand::exit() {}

void KruispuntToestand::volgLijn() {
  int fout      = robot.getSensorData().linePosition - 2000;
  int correctie = (int)(RobotConfig::KP * fout);
  correctie = constrain(correctie, -RobotConfig::SNELHEID_BOCHT,
                                    RobotConfig::SNELHEID_BOCHT);
  robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_GRIJS + correctie,
                                     RobotConfig::SNELHEID_GRIJS - correctie);
}

// Detecteer grijze markeringen links/rechts en onthoud de eerste die bevestigd wordt.
void KruispuntToestand::behandelMarkeringen() {
  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // Kort na een eerdere markering niets nieuws onthouden (lock-afstand).
  if ((grijsLinks || grijsRechts) &&
      robot.getSensorData().distanceCm - grijsCm < RobotConfig::GRIJS_LOCK_CM) {
    return;
  }

  if (lijn.grijsTapeLinks()) {
    if (++grijsLinksTeller >= RobotConfig::GRIJS_BEVESTIG && !grijsLinks) {
      grijsLinks = true;
      grijsCm    = robot.getSensorData().distanceCm;
      Serial.println(F("[GRIJS] LINKS onthouden"));
    }
  } else {
    grijsLinksTeller = 0;
  }

  if (lijn.grijsTapeRechts()) {
    if (++grijsRechtsTeller >= RobotConfig::GRIJS_BEVESTIG && !grijsRechts) {
      grijsRechts = true;
      grijsCm     = robot.getSensorData().distanceCm;
      Serial.println(F("[GRIJS] RECHTS onthouden"));
    }
  } else {
    grijsRechtsTeller = 0;
  }
}
