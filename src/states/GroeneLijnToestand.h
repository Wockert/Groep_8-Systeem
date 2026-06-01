// =============================================================
//  GroeneLijnToestand
//  Actief wanneer de Zumo een groene lijn detecteert. Op groen
//  mag de robot maximaal halve snelheid rijden (attribuut snelheid).
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class GroeneLijnToestand : public RobotToestand {
private:
  int snelheid = 0;

public:
  GroeneLijnToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
