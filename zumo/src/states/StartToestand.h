// =============================================================
//  StartToestand
//  Begin van de rit: de Zumo staat klaar en wacht tot hij mag
//  beginnen. Geen eigen attributen; via RobotToestand wel toegang
//  tot de robot.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class StartToestand : public RobotToestand {
public:
  StartToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
