#include "ZumoHardware.h"
#include <Wire.h>

void ZumoHardware::init() {
  Wire.begin();

  sensors.initFiveSensors();
  prox.initFrontSensor();

  for (uint16_t i = 0; i < 120; i++) {
    if (i > 30 && i <= 90) {
      motors.setSpeeds(-200, 200);
    } else {
      motors.setSpeeds(200, -200);
    }

    sensors.calibrate();
  }

  motors.setSpeeds(0, 0);
}

int ZumoHardware::readLine(unsigned int sensorWaarden[]) {
  return sensors.readLine(sensorWaarden);
}

void ZumoHardware::readCalibrated(unsigned int sensorWaarden[]) {
  sensors.readCalibrated(sensorWaarden);
}

void ZumoHardware::readRawLine(unsigned int sensorWaarden[]) {
  sensors.read(sensorWaarden);
}

void ZumoHardware::readProximity() {
  prox.read();
}

uint8_t ZumoHardware::getProxLeft() {
  return prox.countsFrontWithLeftLeds();
}

uint8_t ZumoHardware::getProxRight() {
  return prox.countsFrontWithRightLeds();
}

void ZumoHardware::setMotorSpeeds(int left, int right) {
  motors.setSpeeds(left, right);
}

void ZumoHardware::stopMotors() {
  motors.setSpeeds(0, 0);
}

void ZumoHardware::printRawLineSensors() {
}

void ZumoHardware::print(String text) {
}
