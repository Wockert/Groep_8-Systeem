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
#include "../control/BaanPlan.h"

class RobotToestand;   // forward declaration (alleen pointer nodig)

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

  // --- 1:1 associaties uit het UML (door ZumoRobot gecoordineerd) ---
  ZumoHardware     hardware;
  LineSensorAnalyse lijnAnalyse;
  PitchDetector    pitchDetector;
  RijController    rijController;
  BlokZoeker       blokZoeker;
  BaanPlan         baanPlan;
  SensorData       sensorData;

  // Print de huidige snapshot naar de seriële monitor (debug/controle).
  void printSensorData();

public:
  void setup();
  void loop();
  void setState(RobotToestand* staat);
  void update();

  // Geeft de snapshot van deze ronde terug. Toestanden lezen hieruit
  // (via hun robot-referentie) zodat iedereen dezelfde meting gebruikt.
  const SensorData& getSensorData() const;

  // Toegang voor de toestanden tot de serviceklassen.
  ZumoHardware&      getHardware()     { return hardware; }
  RijController&     getRijController() { return rijController; }
  LineSensorAnalyse& getLijnAnalyse()  { return lijnAnalyse; }
  BaanPlan&          getBaanPlan()     { return baanPlan; }
};
