#include "LijnVolgenToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot)
: RobotToestand(robot) {}

void LijnVolgenToestand::enter() {
    Serial.println("ENTER: LijnVolgenToestand");
    vorigeFout = 0;
}

void LijnVolgenToestand::update() {
    // Lijn 1x uitlezen via de façade: 0..4000, 2000 = midden.
    int positie = robot.getHardware().readLine(sensorWaarden);

    int fout      = positie - 2000;
    int afgeleide = fout - vorigeFout;
    int correctie = (RobotConfig::KP * fout) + (RobotConfig::KD * afgeleide);

    int links  = constrain(RobotConfig::SNELHEID_NORMAAL + correctie, -400, 400);
    int rechts = constrain(RobotConfig::SNELHEID_NORMAAL - correctie, -400, 400);

    robot.getHardware().setMotorSpeeds(links, rechts);
    vorigeFout = fout;
}


void LijnVolgenToestand::exit() {
    robot.getHardware().stopMotors();
}
