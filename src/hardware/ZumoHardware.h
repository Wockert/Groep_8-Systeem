#pragma once

#include <Arduino.h>
#include <Zumo32U4.h>
#include "../config/SensorData.h"

class ZumoHardware {
private:
  Zumo32U4LineSensors      sensors;
  Zumo32U4Motors           motors;
  Zumo32U4LCD              lcd;
  Zumo32U4IMU              imu;
  Zumo32U4ProximitySensors prox;
  Zumo32U4Encoders         encoders;
  Zumo32U4Buzzer           buzzer;
  Zumo32U4ButtonA          knopA;
  Zumo32U4ButtonB          knopB;
  Zumo32U4ButtonC          knopC;

public:
  void init();
  bool kalibreerLijn();
  int  kalibreerGroenVegend();
  int  readLine(unsigned int sensorWaarden[]);
  void readCalibrated(unsigned int sensorWaarden[]);
  void setMotorSpeeds(int left, int right);
  void stopMotors();
  void print(String text);
  void playDoneSound();
  void speelGroenGeluid();
  long getTicksLinks();
  long getTicksRechts();

  SensorData leesSnapshot();
};
