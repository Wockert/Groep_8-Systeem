// =============================================================
//  RechtdoorToestand
//  Testtoestand: rijdt 10 seconden recht vooruit op vaste
//  snelheid en stopt daarna. Gestart met knop C vanuit
//  StartToestand; na afloop terug naar StartToestand.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class RechtdoorToestand : public RobotToestand {
private:
  unsigned long startTijd = 0;                 // millis() bij binnenkomst
  static constexpr unsigned long DUUR_MS = 10000;  // 10 seconden rechtdoor

public:
  RechtdoorToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
