// =============================================================
//  ZumoHardware  <<facade>>
//  Facade voor alle hardware: motoren, sensoren, LCD en IMU.
//  De rest van de code praat alleen met deze klasse, niet direct
//  met de Zumo32U4-bibliotheek.
// =============================================================
#pragma once

#include <Arduino.h>
#include <Zumo32U4.h>

class ZumoHardware {
private:
  Zumo32U4LineSensors      sensors;
  Zumo32U4Motors           motors;
  Zumo32U4LCD              lcd;
  Zumo32U4IMU              imu;
  Zumo32U4ProximitySensors prox;
  Zumo32U4Encoders         encoders;
  Zumo32U4Buzzer           buzzer;

public:
  void init();
  int  readLine(unsigned int sensorWaarden[]);
  void readCalibrated(unsigned int sensorWaarden[]);
  void readRawLine(unsigned int sensorWaarden[]);
  void printRawLineSensors();
  void readProximity();
  uint8_t getProxLeft();
  uint8_t getProxRight();
  void setMotorSpeeds(int left, int right);
  void stopMotors();
  void print(String text);
  void playDoneSound();
};
