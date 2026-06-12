#include "ZumoHardware.h"
#include "../config/RobotConfig.h"
#include <Wire.h>

void ZumoHardware::init() {
  // 5 lijnsensoren (beste resolutie voor lijnvolgen) + de VOORSTE
  // proximitysensor (pin 22) erbij. Die deelt geen pin met de lijnsensoren,
  // dus dat kan samen. Echte linker/rechter prox blijft onmogelijk met 5
  // lijnsensoren. HARDWARE: jumpers op lijnsensor-stand (pin 20 -> DN2, pin 4 -> DN4).
  sensors.initFiveSensors();   // 5 lijnsensoren: DN1..DN5
  // Kalibreren gebeurt NIET meer hier maar bij de eerste druk op knop A
  // (StartToestand): dan staat de robot al netjes op de lijn.

  Wire.begin();                // I2C-bus starten (NODIG vóór imu.init(), anders leest de IMU 0)
  imu.init();                  // IMU opstarten
  imu.enableDefault();         // standaardinstellingen (versnellingsmeter + gyro)
}

// Kalibratie: draai ~1 rondje terwijl de sensoren wit en zwart leren kennen.
// Plaats de robot op/over de lijn voordat dit draait. Pas hierna geeft
// readLine()/readCalibrated() bruikbare waarden (0=wit .. 1000=zwart).
bool ZumoHardware::kalibreerLijn() {
  // Exacte werkende kalibratie uit branch Lijnsensorvolgen-Verbeteren: draai
  // ter plekke heen en weer zodat elke sensor over zwart EN wit veegt.
  for (uint16_t i = 0; i < 120; i++) {
    if (i > 30 && i <= 90) motors.setSpeeds(-200, 200);   // naar de ene kant
    else                   motors.setSpeeds(200, -200);   // en weer terug
    sensors.calibrate();
  }
  motors.setSpeeds(0, 0);

  // Validatie: heeft ELKE sensor echt wit en zwart gezien? Zo niet (robot
  // stond naast de lijn), dan zijn alle gekalibreerde waarden onzin en
  // gedraagt de hele robot zich "raar" zonder aanwijsbare reden. Dan liever
  // hier hard weigeren, zodat je het meteen ziet en opnieuw kunt kalibreren.
  for (uint8_t i = 0; i < 5; i++) {
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

int ZumoHardware::readLine(unsigned int sensorWaarden[]) {
  // Geeft de lijnpositie 0..4000 (2000 = midden) en vult sensorWaarden[]
  // met de gekalibreerde waarden. Vereist kalibratie vooraf.
  return sensors.readLine(sensorWaarden);
}

void ZumoHardware::readCalibrated(unsigned int sensorWaarden[]) {
  sensors.readCalibrated(sensorWaarden);
}

void ZumoHardware::setMotorSpeeds(int left, int right) {
  // Beveilig tegen waarden buiten het toegestane bereik (-400..400).
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

void ZumoHardware::playDoneSound() {
}

long ZumoHardware::getTicksLinks()  { return encoders.getCountsLeft();  }
long ZumoHardware::getTicksRechts() { return encoders.getCountsRight(); }

// -------------------------------------------------------------
//  leesSnapshot()  -  vult EEN SensorData met alle sensoren tegelijk.
//  Let op: init() moet de gebruikte sensoren eerst hebben opgestart
//  (sensors.initFiveSensors(), imu.init()/enableDefault(),
//  prox.initThreeSensors()) anders blijven de waarden 0.
// -------------------------------------------------------------
SensorData ZumoHardware::leesSnapshot() {
  SensorData data;

  // --- Lijn: EEN gekalibreerde meting voor ALLES (positie + per-sensor) ---
  // Gekalibreerd (0=wit .. 1000=zwart per sensor) is drift-vast: accu en
  // omgevingslicht schuiven de rauwe waarden, maar na kalibratie landen
  // wit/grijs/zwart altijd op dezelfde plek in de schaal. Grijs ligt dan
  // stabiel in het middengebied. Ook: een read per ronde i.p.v. drie, dus
  // elke klasse ziet exact dezelfde meting en de lus blijft snel.
  unsigned int cal[5] = {0};
  data.linePosition = (int)sensors.readLine(cal);
  for (int i = 0; i < 5; i++) {
    data.lineValues[i] = (int)cal[i];
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

  // --- Proximity (TIJDELIJK UIT tijdens lijnvolgen) ---
  // Niet uitgelezen om IR-interferentie met de lijnsensoren te voorkomen.
  // Zet prox.initFrontSensor() in init() weer aan + lees hier opnieuw uit
  // als je vooruit-detectie nodig hebt.
  data.proxLeft  = 0;
  data.proxRight = 0;

  // --- Afgelegde afstand uit de encoders (cm) ---
  // Zelfde omrekening als jullie geteste encoderprogramma:
  // gemiddelde van links+rechts ticks * CM_PER_PULSE (0.019 cm/tick).
  // Let op: dit telt vanaf het opstarten (cumulatief), want we resetten
  // de encoders hier niet.
  long pulsen = ((long)encoders.getCountsLeft() + (long)encoders.getCountsRight()) / 2;
  data.distanceCm = pulsen * RobotConfig::CM_PER_PULSE;

  // --- Knoppen en tijdstempel ---
  data.buttonA   = knopA.isPressed();
  data.buttonB   = knopB.isPressed();
  data.buttonC   = knopC.isPressed();
  data.timestamp = millis();

  return data;
}
