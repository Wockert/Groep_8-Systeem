#pragma once

#include "../core/RobotToestand.h"

// Rijdt een vaste tijd rechtdoor en gaat dan terug naar de starttoestand.
class RechtdoorToestand : public RobotToestand {
private:
  unsigned long startTijd = 0;                       // begintijd (millis)
  static constexpr unsigned long DUUR_MS = 10000;    // hoe lang rechtdoor (ms)

public:
  RechtdoorToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
