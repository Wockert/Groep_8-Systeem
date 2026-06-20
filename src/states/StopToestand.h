#pragma once

#include "../core/RobotToestand.h"

// Eindtoestand: robot staat stil (nog te implementeren).
class StopToestand : public RobotToestand {
public:
  StopToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
