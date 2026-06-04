#include "ZumoHardware.h"
#include "../config/RobotConfig.h"
#include <Wire.h>

void ZumoHardware::init() {
  // 5 lijnsensoren (beste resolutie voor lijnvolgen) + de VOORSTE
  // proximitysensor (pin 22) erbij. Die deelt geen pin met de lijnsensoren,
  // dus dat kan samen. Echte linker/rechter prox blijft onmogelijk met 5
  // lijnsensoren. HARDWARE: jumpers op lijnsensor-stand (pin 20 -> DN2, pin 4 -> DN4).
  sensors.initFiveSensors();   // 5 lijnsensoren: DN1..DN5
  prox.initFrontSensor();      // alleen de voorste proximitysensor (pin 22)

  Wire.begin();                // I2C-bus starten (NODIG vóór imu.init(), anders leest de IMU 0)
  imu.init();                  // IMU opstarten
  imu.enableDefault();         // standaardinstellingen (versnellingsmeter + gyro)
}

int ZumoHardware::readLine(unsigned int sensorWaarden[]) {
  return 0;
}

void ZumoHardware::readCalibrated(unsigned int sensorWaarden[]) {
}

void ZumoHardware::setMotorSpeeds(int left, int right) {
}

void ZumoHardware::stopMotors() {
}

void ZumoHardware::print(String text) {
}

void ZumoHardware::playDoneSound() {
}

// -------------------------------------------------------------
//  leesSnapshot()  -  vult EEN SensorData met alle sensoren tegelijk.
//  Let op: init() moet de gebruikte sensoren eerst hebben opgestart
//  (sensors.initFiveSensors(), imu.init()/enableDefault(),
//  prox.initThreeSensors()) anders blijven de waarden 0.
// -------------------------------------------------------------
SensorData ZumoHardware::leesSnapshot() {
  SensorData data;

  // --- Lijnsensoren (5 RAUWE waarden, 0=wit .. ~2000=zwart) ---
  // We lezen RAUW (sensors.read) i.p.v. readCalibrated(): readCalibrated geeft
  // niks terug zolang er niet gekalibreerd is, dan blijven de waarden 0.
  // Alle 5 waarden lineValues[0..4] (= DN1..DN5) zijn nu geldig.
  unsigned int lijn[5] = {0};
  sensors.read(lijn);
  for (int i = 0; i < 5; i++) {
    data.lineValues[i] = (int)lijn[i];
  }

  // --- IMU: pitch en roll (graden) uit de versnellingsmeter ---
  // Standaard kantelformule. De toewijzing van x/y/z hangt af van hoe de
  // print is gemonteerd; controleer de tekens tijdens het testen (wip-test).
  imu.readAcc();
  float ax = (float)imu.a.x;
  float ay = (float)imu.a.y;
  float az = (float)imu.a.z;
  data.pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0f / PI;
  data.roll  = atan2(ay, az) * 180.0f / PI;

  // --- Proximity (VOORSTE sensor, pin 22) ---
  // Met 5 lijnsensoren is er geen aparte linker/rechter prox. We gebruiken
  // de ene voorste sensor en lezen die mét de linker- en rechter-LED's:
  //   proxLeft  = object meer links-vooraan, proxRight = meer rechts-vooraan.
  // 0 = niets in de buurt; de waarde stijgt als je een object VOOR de robot houdt.
  prox.read();
  data.proxLeft  = prox.countsFrontWithLeftLeds();
  data.proxRight = prox.countsFrontWithRightLeds();

  // --- Afgelegde afstand uit de encoders (cm) ---
  // Zelfde omrekening als jullie geteste encoderprogramma:
  // gemiddelde van links+rechts ticks * CM_PER_PULSE (0.019 cm/tick).
  // Let op: dit telt vanaf het opstarten (cumulatief), want we resetten
  // de encoders hier niet.
  long pulsen = ((long)encoders.getCountsLeft() + (long)encoders.getCountsRight()) / 2;
  data.distanceCm = pulsen * RobotConfig::CM_PER_PULSE;

  // --- Knop A en tijdstempel ---
  data.buttonA   = knopA.isPressed();
  data.timestamp = millis();

  return data;
}
