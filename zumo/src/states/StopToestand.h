// =============================================================
//  StopToestand
//  Actief wanneer de Zumo moet stoppen. Geen eigen attributen:
//  enter() stopt de motoren, update() houdt de robot stil.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class StopToestand : public RobotToestand {
public:
  StopToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
