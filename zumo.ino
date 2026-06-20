// Arduino entry point: enkel wiring, alle logica zit in ZumoRobot.
#include "src/core/ZumoRobot.h"

ZumoRobot robot;

void setup() {
  robot.setup();   // eenmalige initialisatie
}

void loop() {
  robot.loop();    // herhaalde hoofdlus
}
