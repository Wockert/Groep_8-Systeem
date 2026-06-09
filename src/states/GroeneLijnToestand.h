// =============================================================
//  GroeneLijnToestand
//  Actief wanneer de Zumo een groene lijn detecteert. Op groen
//  mag de robot maximaal halve snelheid rijden (attribuut snelheid).
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class GroeneLijnToestand : public RobotToestand {
private:
  unsigned int sensorWaarden[5] = {0};
  int          vorigeFout       = 0;
  int          zwartTeller      = 0;   

public:
  GroeneLijnToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
