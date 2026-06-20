#pragma once

#include "../core/RobotToestand.h"

class WipWachtToestand : public RobotToestand {
private:
  float pitch = 0.0f;

public:
  WipWachtToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
