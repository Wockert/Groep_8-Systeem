#pragma once

class ZumoRobot;

// Basisklasse voor alle toestanden van de robot (state-machine).
class RobotToestand {
protected:
  ZumoRobot& robot;   // verwijzing naar de robot voor toegang tot hardware/sensoren

private:
  int snelheid = 0;

public:
  RobotToestand(ZumoRobot& robot) : robot(robot) {}
  virtual ~RobotToestand() {}

  virtual void enter()  = 0;   // eenmalig bij binnenkomst in deze toestand
  virtual void update() = 0;   // elke lus-iteratie zolang deze toestand actief is
  virtual void exit()   = 0;   // eenmalig bij verlaten van deze toestand
};
