// =============================================================
//  WipOmlaagToestand
//  Actief wanneer de Zumo van de wip of helling naar beneden rijdt.
//  pitch bewaart de hellingshoek om te bepalen of hij nog omlaag
//  rijdt of alweer vlak staat.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class WipOmlaagToestand : public RobotToestand {
private:
  float pitch = 0.0f;

public:
  WipOmlaagToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
