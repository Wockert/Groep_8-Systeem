// =============================================================
//  RobotToestand  <<abstract>>
//  Basisklasse voor alle toestanden. Elke toestand heeft dezelfde
//  drie methodes: enter() bij activeren, update() elke ronde,
//  exit() bij verlaten. Zo hoeft ZumoRobot niet per toestand bij
//  te houden welke methodes hij moet aanroepen.
// =============================================================
#pragma once

class ZumoRobot;   // forward declaration (vermijdt circulaire include)

class RobotToestand {
protected:
  ZumoRobot& robot;          // # robot: ZumoRobot

private:
  int snelheid = 0;

public:
  RobotToestand(ZumoRobot& robot) : robot(robot) {}
  virtual ~RobotToestand() {}

  virtual void enter()  = 0;
  virtual void update() = 0;
  virtual void exit()   = 0;
};
