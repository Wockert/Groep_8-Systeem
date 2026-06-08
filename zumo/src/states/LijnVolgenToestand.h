// =============================================================
//  LijnVolgenToestand
//  Volgt puur de zwarte lijn met PD-sturing. Zodra hij een grijze
//  markering ziet, draagt hij over aan KruispuntToestand, die de
//  richting onthoudt en de afslag afhandelt.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class LijnVolgenToestand : public RobotToestand {
private:
  int positie = 0;
  int fout    = 0;

  void volgLijn();

public:
  LijnVolgenToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
