#include "LijnVolgenToestand.h"
#include "../core/ZumoRobot.h"

extern Zumo32U4LineSensors lineSensors;
extern Zumo32U4Motors motors;

unsigned int sensorValues[5];

const int BASIS_SNELHEID = 200;
const float KP = 0.2;
const float KD = 1.5;

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void LijnVolgenToestand::enter()  {

}

void LijnVolgenToestand::update() {
     int positie = lineSensors.readLine(sensorValues);

    int fout = positie - 2000;

    int afgeleide = fout - vorigeFout;

    int correctie =
        (KP * fout) +
        (KD * afgeleide);

    int links =
        BASIS_SNELHEID + correctie;

    int rechts =
        BASIS_SNELHEID - correctie;

    links = constrain(links, -400, 400);
    rechts = constrain(rechts, -400, 400);

    motors.setSpeeds(links, rechts);

    vorigeFout = fout;

}

void LijnVolgenToestand::exit()   {

}


void LijnVolgenToestand::volgLijn()            {

}

void LijnVolgenToestand::behandelKruising()    {

}

void LijnVolgenToestand::behandelMarkeringen() {
    
}

void LijnVolgenToestand::LijnKwijt()           {
    
}
