#pragma once

#include "../core/RobotToestand.h"

class RechtdoorToestand : public RobotToestand {
private:
  unsigned long startTijd = 0;
  static constexpr unsigned long DUUR_MS = 10000;

public:
  RechtdoorToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
