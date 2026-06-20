#pragma once

#include "../core/RobotToestand.h"

class StopToestand : public RobotToestand {
public:
  StopToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
