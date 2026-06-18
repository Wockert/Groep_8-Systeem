// =============================================================
//  WipWachtToestand
//  Actief wanneer de Zumo tijdelijk moet wachten op de wip/helling.
//  pitch bewaart de hellingshoek; als de pitch onder 0 graden komt
//  mag de robot verder rijden.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class WipWachtToestand : public RobotToestand {
private:
  float pitch = 0.0f;

public:
  WipWachtToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
