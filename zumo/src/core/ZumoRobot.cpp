#include "ZumoRobot.h"
#include "RobotToestand.h"
#include "../states/StartToestand.h"

void ZumoRobot::setup() {
  Serial.begin(9600);                    // seriële monitor voor printSensorData()
  hardware.init();                       // sensoren, IMU en encoders opstarten

  sensorData = hardware.leesSnapshot();  // eerste momentopname
  printSensorData();                     // toon de beginmeting

  setState(new StartToestand(*this));    // start de state machine in de starttoestand
}

void ZumoRobot::loop() {
  printSensorData();
  update();   // elke ronde: verse snapshot + de actieve toestand laten beslissen
}

void ZumoRobot::setState(RobotToestand* staat) {
  // LET OP: een toestand die zelf setState() aanroept, moet daarna meteen
  // 'return;' doen — de oude toestand wordt hieronder namelijk verwijderd.
  if (huidigeStaat != nullptr) {
    huidigeStaat->exit();   // oude toestand netjes afsluiten
    delete huidigeStaat;    // en opruimen (was met new aangemaakt)
  }
  huidigeStaat = staat;
  if (huidigeStaat != nullptr) {
    huidigeStaat->enter();  // nieuwe toestand activeren
  }
  printSensorData();        // toon de sensorwaarden bij elke toestandswissel
}

void ZumoRobot::update() {
  // 1) EEN momentopname van alle sensoren bovenaan de ronde, zodat elke
  //    klasse hieronder exact dezelfde meting ziet (zie SensorData).
  sensorData = hardware.leesSnapshot();

  // Debug: elke 200 ms de snapshot printen. Niet elke ronde, anders loopt
  // de Serial Monitor vol. (Verwijder dit blok als de robot echt rijdt.)
  static unsigned long laatstePrint = 0;
  if (sensorData.timestamp - laatstePrint >= 200) {
    laatstePrint = sensorData.timestamp;
    printSensorData();
  }

  // 2) De actieve toestand beslist op basis van diezelfde snapshot.
  if (huidigeStaat != nullptr) {
    huidigeStaat->update();
  }
}

const SensorData& ZumoRobot::getSensorData() const {
  return sensorData;
}

void ZumoRobot::printSensorData() {
  Serial.print(F("t="));
  Serial.print(sensorData.timestamp);

  Serial.print(F("  lijn="));
  for (int i = 0; i < 5; i++) {
    Serial.print(sensorData.lineValues[i]);
    Serial.print(i < 4 ? ',' : ' ');
  }

  Serial.print(F(" pitch="));
  Serial.print(sensorData.pitch);
  Serial.print(F(" roll="));
  Serial.print(sensorData.roll);
  Serial.print(F(" proxL="));
  Serial.print(sensorData.proxLeft);
  Serial.print(F(" proxR="));
  Serial.print(sensorData.proxRight);
  Serial.print(F(" cm="));
  Serial.print(sensorData.distanceCm);
  Serial.print(F(" knopA="));
  Serial.print(sensorData.buttonA);
  Serial.println();
}
