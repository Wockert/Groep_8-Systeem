#include "LijnVolgenToestand.h"
#include "../core/ZumoRobot.h"

const float KP = 0.4;
const float KD = 2.0;
const int BASIS_SNELHEID = 300;

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot)
: RobotToestand(robot) {}

void LijnVolgenToestand::enter() {
    Serial.println("ENTER: LijnVolgenToestand");
}

void LijnVolgenToestand::update() {

    Serial.println("UPDATE: LijnVolgenToestand");
    Serial.print("Positie = ");
    Serial.println(robot.getHardware().readLine(sensorWaarden));

    // Lijn uitlezen via façade
    int positie = robot.getHardware().readLine(sensorWaarden);
    Serial.println(positie);


    int fout = positie - 2000;
    int afgeleide = fout - vorigeFout;

    int correctie = (KP * fout) + (KD * afgeleide);

    int links  = constrain(BASIS_SNELHEID + correctie, -400, 400);
    int rechts = constrain(BASIS_SNELHEID - correctie, -400, 400);

    robot.getHardware().setMotorSpeeds(links, rechts);

    vorigeFout = fout;
}

void LijnVolgenToestand::exit() {
    robot.getHardware().stopMotors();
}
