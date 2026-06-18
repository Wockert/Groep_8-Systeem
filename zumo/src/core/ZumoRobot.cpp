#include "ZumoRobot.h"
#include "RobotToestand.h"
#include "../config/RobotConfig.h"
#include "../states/StartToestand.h"

void ZumoRobot::setup() {
  Serial.begin(9600);                    // seriële monitor voor printSensorData()
  hardware.init();                       // sensoren, IMU en encoders opstarten
  rijController.koppel(hardware);        // controller mag nu de motoren aansturen

  sensorData = hardware.leesSnapshot();  // eerste momentopname
  printSensorData();                     // toon de beginmeting

  // Kalibreren gebeurt NA knop A (in StartToestand), zodat de robot dan al
  // op de lijn staat en de sensoren echt wit én zwart zien.
  setState(new StartToestand(*this));
}

void ZumoRobot::loop() {
  update();   // elke ronde: verse snapshot + de actieve toestand laten beslissen
}

void ZumoRobot::setState(RobotToestand* staat) {
  // LET OP: een toestand die zelf setState() aanroept, moet daarna meteen
  // 'return;' doen — de oude toestand wordt hieronder namelijk verwijderd.
  if (huidigeStaat != nullptr) {
    huidigeStaat->exit();   // oude toestand netjes afsluiten
    delete huidigeStaat;    // en opruimen (was met new aangemaakt)
  }
  huidigeStaat = staat;
  if (huidigeStaat != nullptr) {
    huidigeStaat->enter();  // nieuwe toestand activeren
  }
  printSensorData();        // toon de sensorwaarden bij elke toestandswissel
}

void ZumoRobot::update() {
  // 1) EEN momentopname van alle sensoren bovenaan de ronde, zodat elke
  //    klasse hieronder exact dezelfde meting ziet (zie SensorData).
  sensorData = hardware.leesSnapshot();
  lijnAnalyse.updateWaarden(sensorData.lineValues);   // lijnanalyse bijwerken voor deze ronde

  // Debug: elke 1000 ms een compacte regel printen (anders loopt de monitor vol
  // en kost het printen zelf merkbaar lustijd).
  static unsigned long laatstePrint = 0;
  if (sensorData.timestamp - laatstePrint >= 1000) {
    laatstePrint = sensorData.timestamp;
    printSensorData();
  }

  // 2) De actieve toestand beslist op basis van diezelfde snapshot.
  if (huidigeStaat != nullptr) {
    huidigeStaat->update();
  }
}

const SensorData& ZumoRobot::getSensorData() const {
  return sensorData;
}

// Compacte debugregel: alleen wat nu telt (lijn, positie, grijs-delta,
// grijs/kruispunt-detectie). dL/dR = hoeveel de buitenste sensoren BOVEN het
// witniveau liggen — rijd over grijs en kijk hoe hoog ze pieken.
void ZumoRobot::printSensorData() {
  if (!RobotConfig::DEBUG_SENSOR_PRINT) return;   // uitgezet in RobotConfig

  int minV = sensorData.lineValues[0];
  for (int i = 1; i < 5; i++) if (sensorData.lineValues[i] < minV) minV = sensorData.lineValues[i];

  Serial.print(F("t="));
  Serial.print(sensorData.timestamp);
  Serial.print(F(" L="));
  for (int i = 0; i < 5; i++) {
    Serial.print(sensorData.lineValues[i]);
    Serial.print(i < 4 ? ',' : ' ');
  }
  Serial.print(F("pos="));  Serial.print(sensorData.linePosition);
  Serial.print(F(" dL="));  Serial.print(sensorData.lineValues[0] - minV);
  Serial.print(F(" dR="));  Serial.print(sensorData.lineValues[4] - minV);
  Serial.print(F(" gL="));  Serial.print(lijnAnalyse.grijsTapeLinks());
  Serial.print(F(" gR="));  Serial.print(lijnAnalyse.grijsTapeRechts());
  Serial.print(F(" kr="));  Serial.print(lijnAnalyse.isKruising());
  Serial.print(F(" gN="));  Serial.print(lijnAnalyse.getGrijsNiveau());   // geijkt grijs-niveau (0=niet)
  Serial.println();
}
