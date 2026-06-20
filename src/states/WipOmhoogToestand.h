#pragma once

#include "../core/RobotToestand.h"

class WipOmhoogToestand : public RobotToestand {
private:
  float pitch = 0.0f;

public:
  WipOmhoogToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
