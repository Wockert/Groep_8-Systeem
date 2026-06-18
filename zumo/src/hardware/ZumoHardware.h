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
  Zumo32U4Buzzer           buzzer;
  Zumo32U4ButtonA          knopA;
  Zumo32U4ButtonB          knopB;
  Zumo32U4ButtonC          knopC;

public:
  void init();
  bool kalibreerLijn();   // draait kort rond en leert wit/zwart; false = mislukt
                          // (een sensor zag te weinig contrast: robot stond niet op de lijn)
  int  kalibreerGroenVegend();  // zelfde heen-en-weer-veeg, maar MEET de groen-piek
                          // (sensor 1-3) en geeft die terug; vereist kalibreerLijn() vooraf
  int  readLine(unsigned int sensorWaarden[]);
  void readCalibrated(unsigned int sensorWaarden[]);
  void setMotorSpeeds(int left, int right);
  void stopMotors();
  void print(String text);
  void playDoneSound();
  void speelGroenGeluid();   // kort deuntje bij het oppakken van de groene lijn
  long getTicksLinks();    // ruwe encoderstanden (cumulatief sinds opstarten),
  long getTicksRechts();   // voor draaihoek-odometrie tijdens scherpe bochten

  void    readRawLine(unsigned int sensorWaarden[]);
  void    readProximity();
  uint8_t getProxLeft();
  uint8_t getProxRight();

  // Leest alle sensoren in EEN keer uit en geeft een SensorData-momentopname
  // terug. ZumoRobot roept dit aan bovenaan elke ronde, zodat elke klasse in
  // die ronde exact dezelfde meting ziet (zie SensorData <<snapshot>>).
  SensorData leesSnapshot();
};
