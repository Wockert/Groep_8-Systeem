#include "rijden.h"
#include "xbee.h"
#include "Wire.h"

Zumo32U4LineSensors lineSensor;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

Rijden Motors;
Xbee xbee;

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
  switch (xbee.leesXbee()) {
    case 'w':
      Motors.setSnelheid(300);
      break;
    case 's':
      Motors.setSnelheid(300);
      Motors.Achteruit();
      break;
    case ' ':
      Motors.Stop();
      break;
    case 'a':
      Motors.setSnelheid(200);
      Motors.naarLinks();
      break;
    case 'd':
      Motors.setSnelheid(200);
      Motors.naarRechts();
      break;
    case 'e':
      Motors.setSnelheid(400);
      break;
  }
}