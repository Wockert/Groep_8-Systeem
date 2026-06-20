#include "StartToestand.h"
#include "../core/ZumoRobot.h"
#include "LijnVolgenToestand.h"

StartToestand::StartToestand(ZumoRobot& robot) : RobotToestand(robot) {}

static int kalStap = 0;

void StartToestand::enter() {
  robot.getHardware().stopMotors();
  robot.getHardware().print("A=ZWART, dan A=GROEN, C=start");
  aLosgelaten = false;
}

void StartToestand::update() {
  const SensorData& s = robot.getSensorData();

  if (!s.buttonB) {
    bLosgelaten = true;
  } else if (bLosgelaten) {
    if (kalStap < 1) {
      robot.getHardware().print("Eerst A (zwart kalib)");
      Serial.println(F("[GRIJS IJKEN] eerst zwart kalibreren (knop A) voordat je grijs ijkt"));
      bLosgelaten = false;
      return;
    }
    int hoog = s.lineValues[0], laag = s.lineValues[0];
    for (int i = 1; i < 5; i++) {
      if (s.lineValues[i] > hoog) hoog = s.lineValues[i];
      if (s.lineValues[i] < laag) laag = s.lineValues[i];
    }
    robot.getLijnAnalyse().ijkGrijs(hoog);

    Serial.print(F("[GRIJS IJKEN] grijs="));
    Serial.print(hoog);
    Serial.print(F("  (wit="));  Serial.print(laag);
    Serial.println(F(")"));

    String tekst = "Grijs=" + String(hoog);
    robot.getHardware().print(tekst);
    bLosgelaten = false;
    return;
  }

  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    aLosgelaten = false;
    if (kalStap == 0) {
      robot.getHardware().print("ZWART kalibreren (draait)...");
      if (robot.getHardware().kalibreerLijn()) {
        kalStap = 1;
        robot.getHardware().print("Zet op GROEN, druk A");
        Serial.println(F("[KALIBRATIE] zwart klaar -> zet op GROEN, druk A"));
      } else {
        robot.getHardware().print("Kalib FOUT! A=retry");
      }
    } else if (kalStap == 1) {
      robot.getHardware().print("GROEN ijken (draait)...");
      int groen = robot.getHardware().kalibreerGroenVegend();
      robot.getLijnAnalyse().ijkGroen(groen);
      kalStap = 2;
      robot.getHardware().print("Klaar! Druk C = start");
      Serial.print(F("[KALIBRATIE] groen op "));
      Serial.print(groen);
      Serial.println(F(" -> alles klaar, C = start"));
    }
    return;
  }

  if (!s.buttonC) {
    cLosgelaten = true;
  } else if (cLosgelaten) {
    cLosgelaten = false;
    if (kalStap < 2) {
      robot.getHardware().print("Eerst A: zwart + groen");
      Serial.print(F("[START] eerst zwart en groen kalibreren (knop A), nu bij stap "));
      Serial.println(kalStap);
      return;
    }
    robot.getBaanPlan().reset(robot.getSensorData().distanceCm);
    robot.setState(new LijnVolgenToestand(robot));
    return;
  }
}

void StartToestand::exit() {}
