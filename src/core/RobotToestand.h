#pragma once

class ZumoRobot;

class RobotToestand {
protected:
  ZumoRobot& robot;

private:
  int snelheid = 0;

public:
  RobotToestand(ZumoRobot& robot) : robot(robot) {}
  virtual ~RobotToestand() {}

  virtual void enter()  = 0;
  virtual void update() = 0;
  virtual void exit()   = 0;
};
