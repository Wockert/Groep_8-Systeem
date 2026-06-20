#pragma once

#include "../core/RobotToestand.h"

class GroeneLijnToestand : public RobotToestand {
private:
  int  vorigeFout  = 0;
  int  dGefilterd  = 0;
  long foutSom     = 0;
  int  basis       = 0;
  int  zwartTeller = 0;
  bool aLosgelaten = false;

public:
  GroeneLijnToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
