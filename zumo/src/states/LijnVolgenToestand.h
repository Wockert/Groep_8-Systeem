// =============================================================
//  LijnVolgenToestand
//  De hoofdtoestand: volgt de lijn met PD-sturing en behandelt
//  kruisingen en markeringen onderweg.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class LijnVolgenToestand : public RobotToestand {
private:
  int positie = 0;
  int fout    = 0;

  void volgLijn();
  void behandelKruising();
  void behandelMarkeringen();
  void LijnKwijt();

public:
  LijnVolgenToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
