#include "LijnVolgenToestand.h"
#include "../core/ZumoRobot.h"

extern Zumo32U4LineSensors lineSensors;
extern Zumo32U4Motors motors;

unsigned int sensorValues[5];

// PD-waarden
const int BASIS_SNELHEID = 200;
const float KP = 0.20;
const float KD = 1.50;

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot)
: RobotToestand(robot) {}


void LijnVolgenToestand::enter() {
    vorigeFout = 0;
}


void LijnVolgenToestand::update() {

    // Lees lijnpositie (0–4000)
    positie = lineSensors.readLine(sensorValues);

    // Bereken fout t.o.v. midden (2000)
    fout = positie - 2000;
    int afgeleide = fout - vorigeFout;

    // --- Detecties ---
    bool kruising =
        sensorValues[0] > 800 &&
        sensorValues[1] > 800 &&
        sensorValues[2] > 800 &&
        sensorValues[3] > 800 &&
        sensorValues[4] > 800;

    bool lijnKwijt =
        sensorValues[0] < 200 &&
        sensorValues[1] < 200 &&
        sensorValues[2] < 200 &&
        sensorValues[3] < 200 &&
        sensorValues[4] < 200;

    if (kruising) {
        behandelKruising();
        return;
    }

    if (lijnKwijt) {
        LijnKwijt();
        return;
    }

    // --- PD-regeling ---
    int correctie = (KP * fout) + (KD * afgeleide);

    int links  = constrain(BASIS_SNELHEID + correctie, -400, 400);
    int rechts = constrain(BASIS_SNELHEID - correctie, -400, 400);

    motors.setSpeeds(links, rechts);

    vorigeFout = fout;
}


void LijnVolgenToestand::exit() {
    motors.setSpeeds(0, 0);
}


// =============================================================
//  Subfuncties
// =============================================================

void LijnVolgenToestand::volgLijn() {
    // Wordt nu direct in update() gedaan
}

void LijnVolgenToestand::behandelKruising() {
    // Voor nu: stop even zodat je ziet dat hij het detecteert
    motors.setSpeeds(0, 0);
    delay(300);
}

void LijnVolgenToestand::behandelMarkeringen() {
    // Nog niet gebruikt
}

void LijnVolgenToestand::LijnKwijt() {
    // Simpele herstelstrategie: draai langzaam naar links
    motors.setSpeeds(-150, 150);
}
