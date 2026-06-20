#pragma once

#include "../core/RobotToestand.h"

class BlokZoekToestand : public RobotToestand {
public:
  BlokZoekToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
