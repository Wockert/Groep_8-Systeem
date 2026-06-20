#include "ZumoHardware.h"
#include "../config/RobotConfig.h"
#include <Wire.h>

void ZumoHardware::init() {
  sensors.initFiveSensors();

  Wire.begin();
  imu.init();
  imu.enableDefault();
}

// Draai heen en weer over de lijn en kalibreer; faalt als een sensor te weinig contrast ziet.
bool ZumoHardware::kalibreerLijn() {
  for (uint16_t i = 0; i < 120; i++) {
    if (i > 30 && i <= 90) motors.setSpeeds(-200, 200);
    else                   motors.setSpeeds(200, -200);
    sensors.calibrate();
  }
  motors.setSpeeds(0, 0);

  for (uint8_t i = 1; i <= 3; i++) {
    int bereik = (int)sensors.calibratedMaximumOn[i]
               - (int)sensors.calibratedMinimumOn[i];
    if (bereik < 300) {
      Serial.print(F("[KALIBRATIE] sensor "));
      Serial.print(i);
      Serial.print(F(" zag te weinig contrast (bereik="));
      Serial.print(bereik);
      Serial.println(F(") -> opnieuw!"));
      return false;
    }
  }
  return true;
}

// Draai over de groene lijn en geef de hoogste gemeten middensensorwaarde (de groen-piek) terug.
int ZumoHardware::kalibreerGroenVegend() {
  int piek = 0;
  for (uint16_t i = 0; i < 120; i++) {
    if (i > 30 && i <= 90) motors.setSpeeds(-200, 200);
    else                   motors.setSpeeds(200, -200);
    sensors.calibrate();
    unsigned int cal[5] = {0};
    sensors.readCalibrated(cal);
    for (uint8_t k = 1; k <= 3; k++)
      if ((int)cal[k] > piek) piek = (int)cal[k];
  }
  motors.setSpeeds(0, 0);
  return piek;
}

int ZumoHardware::readLine(unsigned int sensorWaarden[]) {
  return sensors.readLine(sensorWaarden);
}

void ZumoHardware::readCalibrated(unsigned int sensorWaarden[]) {
  sensors.readCalibrated(sensorWaarden);
}

void ZumoHardware::setMotorSpeeds(int left, int right) {
  left  = constrain(left,  -400, 400);
  right = constrain(right, -400, 400);
  motors.setSpeeds(left, right);
}

void ZumoHardware::stopMotors() {
  motors.setSpeeds(0, 0);
}

void ZumoHardware::print(String text) {
  lcd.clear();
  lcd.print(text);
}

// Nog niet geïmplementeerd.
void ZumoHardware::playDoneSound() {
}

void ZumoHardware::speelGroenGeluid() {
  buzzer.play("L16 cegc");
}

long ZumoHardware::getTicksLinks()  { return encoders.getCountsLeft();  }
long ZumoHardware::getTicksRechts() { return encoders.getCountsRight(); }

// Lees alle sensoren uit en bundel ze in één SensorData-momentopname.
SensorData ZumoHardware::leesSnapshot() {
  SensorData data;

  unsigned int cal[5] = {0};
  data.linePosition = (int)sensors.readLine(cal);
  for (int i = 0; i < 5; i++) {
    data.lineValues[i] = (int)cal[i];
  }

  // Pitch/roll uit de versnellingsmeter berekenen.
  imu.readAcc();
  float ax = (float)imu.a.x;
  float ay = (float)imu.a.y;
  float az = (float)imu.a.z;
  data.pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0f / PI;
  data.roll  = atan2(ay, az) * 180.0f / PI;

  data.proxLeft  = 0;
  data.proxRight = 0;

  // Gemiddelde encoderstand omrekenen naar afgelegde afstand in cm.
  long pulsen = ((long)encoders.getCountsLeft() + (long)encoders.getCountsRight()) / 2;
  data.distanceCm = pulsen * RobotConfig::CM_PER_PULSE;

  data.buttonA   = knopA.isPressed();
  data.buttonB   = knopB.isPressed();
  data.buttonC   = knopC.isPressed();
  data.timestamp = millis();

  return data;
}
