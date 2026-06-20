#pragma once

#include "../core/RobotToestand.h"

// Toestand voor het zoeken en duwen van een blok (nog te implementeren).
class BlokZoekToestand : public RobotToestand {
public:
  BlokZoekToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
