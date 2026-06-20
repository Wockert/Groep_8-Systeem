#pragma once

#include "../core/RobotToestand.h"

// Toestand tijdens het oprijden van de wip (nog te implementeren).
class WipOmhoogToestand : public RobotToestand {
private:
  float pitch = 0.0f;   // huidige kanteling

public:
  WipOmhoogToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
