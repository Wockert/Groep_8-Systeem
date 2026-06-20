#pragma once

#include "../core/RobotToestand.h"

// Toestand tijdens het afrijden van de wip (nog te implementeren).
class WipOmlaagToestand : public RobotToestand {
private:
  float pitch = 0.0f;   // huidige kanteling

public:
  WipOmlaagToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
