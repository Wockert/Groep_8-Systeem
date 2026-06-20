#pragma once

#include "../core/RobotToestand.h"

class KruispuntToestand : public RobotToestand {
private:
  bool grijsLinks  = false;
  bool grijsRechts = false;
  int  grijsLinksTeller  = 0;
  int  grijsRechtsTeller = 0;
  float grijsCm = 0.0f;
  int  splitsTeller = 0;
  bool bezigMetAfslaan = false;
  int  afslagRichting  = 0;
  unsigned long afslagTijd = 0;
  bool aLosgelaten = false;

  void volgLijn();
  void behandelMarkeringen();

public:
  KruispuntToestand(ZumoRobot& robot, int richting = 0);
  void enter()  override;
  void update() override;
  void exit()   override;
};
