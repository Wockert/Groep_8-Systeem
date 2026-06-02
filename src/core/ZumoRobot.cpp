#include "ZumoRobot.h"
#include "RobotToestand.h"
#include "../states/LijnVolgenToestand.h"

void ZumoRobot::setup() {
    hardware.init();
    setState(new LijnVolgenToestand(*this));
    Serial.println("ZumoRobot::setup()");

}

void ZumoRobot::loop() {
    if (huidigeStaat != nullptr)
        huidigeStaat->update();
}



void ZumoRobot::setState(RobotToestand* staat) {
    Serial.println("STATE CHANGE");

}

void ZumoRobot::update() {
}
