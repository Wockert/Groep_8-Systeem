// =============================================================
//  ZumoHardware  <<facade>>
//  Facade voor alle hardware: motoren, sensoren, LCD en IMU.
//  De rest van de code praat alleen met deze klasse, niet direct
//  met de Zumo32U4-bibliotheek.
// =============================================================
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
  Zumo32U4ButtonA          knopA;
  Zumo32U4ButtonB          knopB;

public:
  void init();
  void kalibreerLijn();   // draait kort rond en leert wit/zwart (nodig voor readLine)
  int  readLine(unsigned int sensorWaarden[]);
  void readCalibrated(unsigned int sensorWaarden[]);
  void setMotorSpeeds(int left, int right);
  void stopMotors();
  void print(String text);
  void playDoneSound();

  // Leest alle sensoren in EEN keer uit en geeft een SensorData-momentopname
  // terug. ZumoRobot roept dit aan bovenaan elke ronde, zodat elke klasse in
  // die ronde exact dezelfde meting ziet (zie SensorData <<snapshot>>).
  SensorData leesSnapshot();
};
