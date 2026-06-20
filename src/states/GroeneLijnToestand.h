#pragma once

#include "../core/RobotToestand.h"

// Volgt de groene lijn met PID; stapt terug naar de zwarte lijnvolger zodra het groen op is.
class GroeneLijnToestand : public RobotToestand {
private:
  int  vorigeFout  = 0;     // PID-toestand (zie LijnVolgenToestand)
  int  dGefilterd  = 0;
  long foutSom     = 0;
  int  basis       = 0;
  int  zwartTeller = 0;     // bevestigingsteller: aantal keren géén groen meer
  bool aLosgelaten = false;

public:
  GroeneLijnToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
