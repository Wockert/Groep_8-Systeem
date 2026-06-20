#include "GroeneLijnToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "LijnVolgenToestand.h"
#include "StartToestand.h"

GroeneLijnToestand::GroeneLijnToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void GroeneLijnToestand::enter() {
  Serial.println(F("[GROEN] enter: groene lijn volgen"));
  robot.getHardware().print("Groene lijn");
  robot.getHardware().speelGroenGeluid();
  vorigeFout  = 0;
  dGefilterd  = 0;
  foutSom     = 0;
  basis       = RobotConfig::SNELHEID_LAAG;
  zwartTeller = 0;
  aLosgelaten = false;
}

void GroeneLijnToestand::update() {
  const SensorData& s = robot.getSensorData();

  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    robot.setState(new StartToestand(robot));
    return;
  }

  // Geen groen meer (na een paar bevestigingen): checkpoint afronden en terug naar de zwarte lijnvolger.
  if (!robot.getLijnAnalyse().isGroeneLijn()) {
    if (++zwartTeller >= RobotConfig::GROEN_BEVESTIG) {
      Serial.println(F("[GROEN] exit: geen groen meer -> zwarte lijnvolger"));
      robot.getBaanPlan().rondAf(s.distanceCm);
      robot.setState(new LijnVolgenToestand(robot));
      return;
    }
  } else {
    zwartTeller = 0;
  }

  // PID-regeling, identiek aan de zwarte lijnvolger maar met groen-snelheden.
  int fout      = s.linePosition - 2000;
  int afgeleide = fout - vorigeFout;
  dGefilterd    = (dGefilterd + afgeleide) / 2;

  foutSom += fout;
  foutSom -= foutSom / 32;
  foutSom = constrain(foutSom, -RobotConfig::FOUT_SOM_MAX, RobotConfig::FOUT_SOM_MAX);

  int correctie = (RobotConfig::KP * fout)
                + (RobotConfig::KI * foutSom)
                + (RobotConfig::KD * dGefilterd);

  correctie = (int)(correctie * (1.0f + abs(fout) / 1500.0f));

  int doel = map(abs(fout), 0, 2000,
                 RobotConfig::SNELHEID_GROEN, RobotConfig::SNELHEID_LAAG);
  if (doel > basis) basis = min(doel, basis + 4);
  else              basis = max(doel, basis - 12);

  int links  = constrain(basis + correctie, -400, 400);
  int rechts = constrain(basis - correctie, -400, 400);

  robot.getHardware().setMotorSpeeds(links, rechts);
  vorigeFout = fout;
}

void GroeneLijnToestand::exit() {
  robot.getHardware().stopMotors();
}
