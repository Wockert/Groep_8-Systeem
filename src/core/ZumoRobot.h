// =============================================================
//  ZumoRobot
//  De hoofdklasse die alles aanstuurt. Doet zelf het werk niet,
//  maar delegeert naar de juiste klassen. setup() draait 1x bij
//  het opstarten, loop() continu. Houdt de actieve toestand bij
//  en schakelt ertussen via setState().
// =============================================================
#pragma once

#include "../config/SensorData.h"
#include "../hardware/ZumoHardware.h"
#include "../sensors/LineSensorAnalyse.h"
#include "../sensors/PitchDetector.h"
#include "../control/RijController.h"
#include "../control/BlokZoeker.h"

class RobotToestand;

class ZumoRobot {
private:
  // --- UML attributen ---
  RobotToestand* huidigeStaat     = nullptr;
  unsigned long  laatsteLijnGezien = 0;
  int            laatsteFout       = 0;
  int            vorigeFout        = 0;
  bool           grijsLinksGezien  = false;
  bool           grijsRechtsGezien = false;
  int            huidigeSnelheid   = 0;

  // --- 1:1 associaties uit het UML ---
  ZumoHardware      hardware;
  LineSensorAnalyse lijnAnalyse;
  PitchDetector     pitchDetector;
  RijController     rijController;
  BlokZoeker        blokZoeker;
  SensorData        sensorData;

public:
  void setup();
  void loop();
  void setState(RobotToestand* staat);
  void update();

  ZumoHardware& getHardware() {
    return hardware;
  }

  LineSensorAnalyse& getLijnAnalyse() {
    return lijnAnalyse;
  }

  RijController& getRijController() {
    return rijController;
  }

  BlokZoeker& getBlokZoeker() {
    return blokZoeker;
  }

  SensorData& getSensorData() {
    return sensorData;
  }
};
