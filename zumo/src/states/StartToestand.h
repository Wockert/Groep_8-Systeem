// =============================================================
//  StartToestand
//  Begin van de rit: de Zumo staat klaar en wacht tot hij mag
//  beginnen. Geen eigen attributen; via RobotToestand wel toegang
//  tot de robot.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class StartToestand : public RobotToestand {
private:
  bool aLosgelaten = false;   // A moet eerst los (na een stop met A) voor hij opnieuw start
  bool bLosgelaten = true;    // B (grijs ijken) moet tussen metingen worden losgelaten
  bool cLosgelaten = false;   // C moet eerst los (na de rechtdoor-test) voor een nieuwe start

public:
  StartToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
