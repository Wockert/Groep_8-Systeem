// =============================================================
//  zumo.ino  -  Arduino entry point (alleen wiring, geen logica)
//  Maakt de hoofdklasse ZumoRobot aan en delegeert setup/loop.
// =============================================================
#include "src/core/ZumoRobot.h"

ZumoRobot robot;

void setup() {
  robot.setup();   // draait 1x bij opstarten
}

void loop() {
  robot.loop();    // draait continu
}
