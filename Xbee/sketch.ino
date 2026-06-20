#include "rijden.h"
#include "xbee.h"
#include "Wire.h"

Zumo32U4LineSensors lineSensor;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

Rijden Motors;
Xbee xbee;

unsigned long laatsteCommandoTijd = 0;
const unsigned long STOP_TIMEOUT = 500;  // ms zonder invoer voordat de robot stopt

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.print(Motors.getSnelheid());
  lineSensor.initFiveSensors();
  for(int i = 0; i<120; i++) {
    lineSensor.calibrate();
  }
  Motors.initialiseer();
  
}

void loop() {
  int16_t position = lineSensor.readLine(lineSensorValues);
  //Serial.println(Motors.getSnelheid());
  //Serial.println(lineSensorValues);
  int arrayLength = sizeof(lineSensorValues) / sizeof(lineSensorValues[0]);
  for(int i = 0; i<arrayLength; i++) {
    if (lineSensorValues[i] != 0) {
      Serial.println(lineSensorValues[i]);
    }
  }
  delay(100);

  // lees alle binnengekomen tekens en onthoud het laatste
  char commando = 0;
  while (xbee.beschikbaar() > 0) {
    commando = xbee.leesXbee();
  }

  if (commando != 0) {
    switch (commando) {
      case 'w':                  // vooruit
        Motors.setSnelheid(300);
        break;
      case 's':                  // achteruit
        Motors.setSnelheid(300);
        Motors.Achteruit();
        break;
      case 'a':                  // naar links draaien
        Motors.setSnelheid(200);
        Motors.naarLinks();
        break;
      case 'd':                  // naar rechts draaien
        Motors.setSnelheid(200);
        Motors.naarRechts();
        break;
      case 'e':                  // vooruit op volle kracht (helling)
        Motors.setSnelheid(400);
        break;
    }
    laatsteCommandoTijd = millis();
  }

  // geen invoer meer binnen STOP_TIMEOUT -> automatisch stoppen
  if (millis() - laatsteCommandoTijd > STOP_TIMEOUT) {
    Motors.Stop();
  }
}