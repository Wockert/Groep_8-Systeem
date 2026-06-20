#pragma once

#include "../config/SensorData.h"
#include "../hardware/ZumoHardware.h"
#include "../sensors/LineSensorAnalyse.h"
#include "../sensors/PitchDetector.h"
#include "../control/RijController.h"
#include "../control/BlokZoeker.h"
#include "../control/BaanPlan.h"

class RobotToestand;

// Hoofdklasse: bundelt alle hardware/sensoren en stuurt de state-machine aan.
class ZumoRobot {
private:
  RobotToestand* huidigeStaat     = nullptr;   // actieve toestand
  unsigned long  laatsteLijnGezien = 0;
  int            laatsteFout       = 0;
  int            vorigeFout        = 0;
  bool           grijsLinksGezien  = false;
  bool           grijsRechtsGezien = false;
  int            huidigeSnelheid   = 0;

  ZumoHardware     hardware;
  LineSensorAnalyse lijnAnalyse;
  PitchDetector    pitchDetector;
  RijController    rijController;
  BlokZoeker       blokZoeker;
  BaanPlan         baanPlan;
  SensorData       sensorData;

  void printSensorData();

public:
  void setup();                        // eenmalige initialisatie (Arduino setup)
  void loop();                         // herhaalde lus (Arduino loop)
  void setState(RobotToestand* staat); // wissel naar een nieuwe toestand
  void update();                       // lees sensoren en draai de huidige toestand

  const SensorData& getSensorData() const;

  ZumoHardware&      getHardware()     { return hardware; }
  RijController&     getRijController() { return rijController; }
  LineSensorAnalyse& getLijnAnalyse()  { return lijnAnalyse; }
  BaanPlan&          getBaanPlan()     { return baanPlan; }
};
