// =============================================================
//  WipOmhoogToestand
//  Actief wanneer de Zumo een wip of helling omhoog rijdt.
//  pitch bewaart de huidige hellingshoek (van de IMU via PitchDetector).
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class WipOmhoogToestand : public RobotToestand {
private:
  float pitch = 0.0f;

public:
  WipOmhoogToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
