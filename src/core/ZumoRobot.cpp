#include "ZumoRobot.h"
#include "RobotToestand.h"
#include "../config/RobotConfig.h"
#include "../states/StartToestand.h"

void ZumoRobot::setup() {
  Serial.begin(9600);
  hardware.init();
  rijController.koppel(hardware);

  sensorData = hardware.leesSnapshot();
  printSensorData();

  setState(new StartToestand(*this));
}

void ZumoRobot::loop() {
  update();
}

void ZumoRobot::setState(RobotToestand* staat) {
  if (huidigeStaat != nullptr) {
    huidigeStaat->exit();
    delete huidigeStaat;
  }
  huidigeStaat = staat;
  if (huidigeStaat != nullptr) {
    huidigeStaat->enter();
  }
  printSensorData();
}

void ZumoRobot::update() {
  sensorData = hardware.leesSnapshot();
  lijnAnalyse.updateWaarden(sensorData.lineValues);

  static unsigned long laatstePrint = 0;
  if (sensorData.timestamp - laatstePrint >= 1000) {
    laatstePrint = sensorData.timestamp;
    printSensorData();
  }

  if (huidigeStaat != nullptr) {
    huidigeStaat->update();
  }
}

const SensorData& ZumoRobot::getSensorData() const {
  return sensorData;
}

void ZumoRobot::printSensorData() {
  if (!RobotConfig::DEBUG_SENSOR_PRINT) return;

  int minV = sensorData.lineValues[0];
  for (int i = 1; i < 5; i++) if (sensorData.lineValues[i] < minV) minV = sensorData.lineValues[i];

  Serial.print(F("t="));
  Serial.print(sensorData.timestamp);
  Serial.print(F(" L="));
  for (int i = 0; i < 5; i++) {
    Serial.print(sensorData.lineValues[i]);
    Serial.print(i < 4 ? ',' : ' ');
  }
  Serial.print(F("pos="));  Serial.print(sensorData.linePosition);
  Serial.print(F(" dL="));  Serial.print(sensorData.lineValues[0] - minV);
  Serial.print(F(" dR="));  Serial.print(sensorData.lineValues[4] - minV);
  Serial.print(F(" gL="));  Serial.print(lijnAnalyse.grijsTapeLinks());
  Serial.print(F(" gR="));  Serial.print(lijnAnalyse.grijsTapeRechts());
  Serial.print(F(" kr="));  Serial.print(lijnAnalyse.isKruising());
  Serial.print(F(" gN="));  Serial.print(lijnAnalyse.getGrijsNiveau());
  Serial.print(F(" cp="));  Serial.print(baanPlan.getIndex());
  Serial.println();
}
