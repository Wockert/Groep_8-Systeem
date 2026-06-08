#include "StartToestand.h"
#include "../core/ZumoRobot.h"
#include "LijnVolgenToestand.h"

StartToestand::StartToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void StartToestand::enter() {
  robot.getHardware().stopMotors();
  robot.getHardware().print("B grijs A go");   // B = grijs ijken, A = starten
}

void StartToestand::update() {
  const SensorData& s = robot.getSensorData();

  // Knop B (eenmalig per druk): GRIJS IJKEN. Houd één buitenste sensor op grijs
  // en de rest op wit; we meten hoeveel grijs boven wit ligt (max - min).
  if (s.buttonB && !vorigeB) {
    int minV = s.lineValues[0], maxV = s.lineValues[0];
    for (int i = 1; i < 5; i++) {
      if (s.lineValues[i] < minV) minV = s.lineValues[i];
      if (s.lineValues[i] > maxV) maxV = s.lineValues[i];
    }
    int delta = maxV - minV;
    robot.getLijnAnalyse().ijkGrijs(delta);
    robot.getHardware().print("Grijs=" + String(delta));
    Serial.print(F("[IJK] grijs-niveau = "));
    Serial.println(delta);
  }
  vorigeB = s.buttonB;

  // Knop A → de lijn gaan volgen.
  if (s.buttonA) {
    robot.setState(new LijnVolgenToestand(robot));
    return;   // belangrijk: oude toestand is hierna verwijderd
  }
}

void StartToestand::exit() {}
