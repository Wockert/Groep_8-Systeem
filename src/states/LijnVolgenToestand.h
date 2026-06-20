#pragma once

#include "../core/RobotToestand.h"

class LijnVolgenToestand : public RobotToestand {
private:
  unsigned int sensorWaarden[5] = {0};
  int  vorigeFout  = 0;
  int  dGefilterd  = 0;
  long foutSom     = 0;
  int  basis       = 0;
  bool aLosgelaten = false;
  bool inBocht     = false;
  int  bochtKant   = 0;
  int  randTeller  = 0;
  int  grijsTeller = 0;
  int  groenTeller = 0;
  int  pitchTeller = 0;
  float randZwartCm = -1000.0f;
  int  randZwartKant = 0;
  long bochtTicksL = 0;
  long bochtTicksR = 0;
  float naBochtCm     = -1000.0f;
  float zwartZijCm  = -1000.0f;
  int  zwartZijKant = 0;
  bool  inStippelZone     = false;
  float stippelZoneStartCm = 0.0f;
  float stippelGatCm       = 0.0f;

public:
  LijnVolgenToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
