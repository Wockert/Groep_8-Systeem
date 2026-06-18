#include "BlokZoekToestand.h"
#include "../core/ZumoRobot.h"

BlokZoekToestand::BlokZoekToestand(ZumoRobot& robot)
: RobotToestand(robot) {}

void BlokZoekToestand::enter() {
  robot.getHardware().stopMotors();
  robot.getBlokZoeker().resetAanval();
}

void BlokZoekToestand::update() {
  bool klaar = robot.getBlokZoeker().zoekEnDuwBlok(robot.getHardware());

  if (klaar) {
    robot.getHardware().stopMotors();
  }
}

void BlokZoekToestand::exit() {
  robot.getHardware().stopMotors();
}
