#include "LijnVolgenToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "KruispuntToestand.h"

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void LijnVolgenToestand::enter() {
  robot.getHardware().print("Lijn volgen");
}

void LijnVolgenToestand::update() {
  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // Grijze markering gezien? Dan neemt KruispuntToestand het over: die onthoudt
  // de richting, volgt door tot de splitsing en handelt de afslag af.
  if (lijn.grijsTapeLinks() || lijn.grijsTapeRechts()) {
    robot.setState(new KruispuntToestand(robot));
    return;   // oude toestand is hierna verwijderd
  }

  // Anders gewoon de lijn volgen.
  volgLijn();
}

void LijnVolgenToestand::exit() {
  robot.getRijController().stop();
}

// Volgt de zwarte lijn met PD-sturing: fout = positie - 2000.
void LijnVolgenToestand::volgLijn() {
  positie = robot.getSensorData().linePosition;   // gekalibreerd 0..4000
  fout    = positie - 2000;                        // 0 = lijn in het midden
  robot.getRijController().stuurPD(RobotConfig::SNELHEID_NORMAAL, fout);
}
