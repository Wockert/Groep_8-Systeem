#pragma once

#include "../config/SensorData.h"
#include "../hardware/ZumoHardware.h"
#include "../sensors/LineSensorAnalyse.h"
#include "../sensors/PitchDetector.h"
#include "../control/RijController.h"
#include "../control/BlokZoeker.h"
#include "../control/BaanPlan.h"

class RobotToestand;

class ZumoRobot {
private:
  RobotToestand* huidigeStaat     = nullptr;
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
  void setup();
  void loop();
  void setState(RobotToestand* staat);
  void update();

  const SensorData& getSensorData() const;

  ZumoHardware&      getHardware()     { return hardware; }
  RijController&     getRijController() { return rijController; }
  LineSensorAnalyse& getLijnAnalyse()  { return lijnAnalyse; }
  BaanPlan&          getBaanPlan()     { return baanPlan; }
};
