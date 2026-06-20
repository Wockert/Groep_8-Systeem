#pragma once

#include "../core/RobotToestand.h"

// Begintoestand: kalibreren (knop A: zwart, dan groen; knop B: grijs) en starten met C.
class StartToestand : public RobotToestand {
private:
  // "Losgelaten"-vlaggen: zorgen dat één druk maar één keer telt (debounce).
  bool aLosgelaten = false;
  bool bLosgelaten = true;
  bool cLosgelaten = false;

public:
  StartToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
