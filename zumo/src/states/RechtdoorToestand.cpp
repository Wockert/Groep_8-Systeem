#include "RechtdoorToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "StartToestand.h"

RechtdoorToestand::RechtdoorToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void RechtdoorToestand::enter() {
  robot.getHardware().print("Rechtdoor 10s");
  startTijd = millis();
}

void RechtdoorToestand::update() {
  // Knop A = direct stoppen (test afbreken).
  if (robot.getSensorData().buttonA) {
    robot.setState(new StartToestand(robot));
    return;   // oude toestand is hierna verwijderd
  }

  // 10 seconden voorbij? Stoppen en terug naar het startscherm.
  if (millis() - startTijd >= DUUR_MS) {
    robot.setState(new StartToestand(robot));
    return;   // oude toestand is hierna verwijderd
  }

  int s = RobotConfig::SNELHEID_NORMAAL;
  robot.getHardware().setMotorSpeeds(s, s);
}

void RechtdoorToestand::exit() {
  robot.getHardware().stopMotors();
}
