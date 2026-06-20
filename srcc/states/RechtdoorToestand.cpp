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
  if (robot.getSensorData().buttonA) {
    robot.setState(new StartToestand(robot));
    return;
  }

  if (millis() - startTijd >= DUUR_MS) {
    robot.setState(new StartToestand(robot));
    return;
  }

  int s = RobotConfig::SNELHEID_NORMAAL;
  robot.getHardware().setMotorSpeeds(s, s);
}

void RechtdoorToestand::exit() {
  robot.getHardware().stopMotors();
}
