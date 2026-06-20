#pragma once

#include "../core/RobotToestand.h"

// Toestand: wachten op het balanspunt van de wip (nog te implementeren).
class WipWachtToestand : public RobotToestand {
private:
  float pitch = 0.0f;   // huidige kanteling

public:
  WipWachtToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
