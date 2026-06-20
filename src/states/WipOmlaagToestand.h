#pragma once

#include "../core/RobotToestand.h"

class WipOmlaagToestand : public RobotToestand {
private:
  float pitch = 0.0f;

public:
  WipOmlaagToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
