#pragma once

#include "../core/RobotToestand.h"

class StartToestand : public RobotToestand {
private:
  bool aLosgelaten = false;
  bool bLosgelaten = true;
  bool cLosgelaten = false;

public:
  StartToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
