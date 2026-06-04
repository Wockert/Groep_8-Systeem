// =============================================================
//  BlokZoekToestand
//  Actief wanneer de Zumo het eindblok moet zoeken: nadat de
//  bruine lijn is gedetecteerd en de robot 20 cm vooruit is gereden.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class BlokZoekToestand : public RobotToestand {
public:
  BlokZoekToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
