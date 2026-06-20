#pragma once

#include <Arduino.h>
#include <Zumo32U4.h>
#include "../config/SensorData.h"

// Hardware-laag: schermt de rest van de code af van de Zumo32U4-bibliotheek.
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
  void init();                                  // initialiseer sensoren en IMU
  bool kalibreerLijn();                          // kalibreer zwart/wit (draaiend), false bij te weinig contrast
  int  kalibreerGroenVegend();                   // meet de groen-piek tijdens het draaien
  int  readLine(unsigned int sensorWaarden[]);   // ruwe lijnmeting + positie
  void readCalibrated(unsigned int sensorWaarden[]);
  void setMotorSpeeds(int left, int right);      // motoren aansturen (begrensd op ±400)
  void stopMotors();
  void print(String text);                       // tekst op het LCD
  void playDoneSound();
  void speelGroenGeluid();
  long getTicksLinks();                          // encoder-tellingen links
  long getTicksRechts();                         // encoder-tellingen rechts

  SensorData leesSnapshot();                     // alle sensoren in één SensorData-momentopname
};
