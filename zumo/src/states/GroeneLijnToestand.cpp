#include "GroeneLijnToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "LijnVolgenToestand.h"
#include "StartToestand.h"

GroeneLijnToestand::GroeneLijnToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void GroeneLijnToestand::enter() {
  Serial.println(F("[GROEN] enter: groene lijn volgen"));
  robot.getHardware().print("Groene lijn");
  vorigeFout  = 0;
  dGefilterd  = 0;
  foutSom     = 0;
  basis       = RobotConfig::SNELHEID_LAAG;   // rustig instromen, ramp daarna op
  zwartTeller = 0;
  aLosgelaten = false;
}

void GroeneLijnToestand::update() {
  const SensorData& s = robot.getSensorData();

  // --- Nog een keer A = stoppen (zelfde vangnet als de lijnvolger) ---
  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    robot.setState(new StartToestand(robot));
    return;   // oude toestand is hierna verwijderd
  }

  // Niet meer groen? Pas terugschakelen na GROEN_BEVESTIG rondes op rij,
  // zodat een enkele afwijkende meting (naad, vlekje) hem niet laat flippen.
  if (!robot.getLijnAnalyse().isGroeneLijn()) {
    if (++zwartTeller >= RobotConfig::GROEN_BEVESTIG) {
      Serial.println(F("[GROEN] exit: geen groen meer -> zwarte lijnvolger"));
      robot.getBaanPlan().rondAf(s.distanceCm);   // groen-checkpoint afgerond
      robot.setState(new LijnVolgenToestand(robot));
      return;   // oude toestand is hierna verwijderd
    }
  } else {
    zwartTeller = 0;
  }

  // --- Zelfde PID-lijnvolgen als op zwart (zie LijnVolgenToestand) ---
  int fout      = s.linePosition - 2000;
  int afgeleide = fout - vorigeFout;
  dGefilterd    = (dGefilterd + afgeleide) / 2;   // ruisfilter op de D-term

  // Lekkende I-term: blijvende kleine afwijking wegsturen (midden houden),
  // zonder op te laden (lek 1/32 per ronde + begrenzing).
  foutSom += fout;
  foutSom -= foutSom / 32;
  foutSom = constrain(foutSom, -RobotConfig::FOUT_SOM_MAX, RobotConfig::FOUT_SOM_MAX);

  int correctie = (RobotConfig::KP * fout)
                + (RobotConfig::KI * foutSom)
                + (RobotConfig::KD * dGefilterd);

  // Progressief sturen: verder van het midden = onevenredig harder terug.
  correctie = (int)(correctie * (1.0f + abs(fout) / 1500.0f));

  // Adaptieve snelheid, geleidelijk: SNELHEID_GROEN is het maximum op groen.
  int doel = map(abs(fout), 0, 2000,
                 RobotConfig::SNELHEID_GROEN, RobotConfig::SNELHEID_LAAG);
  if (doel > basis) basis = min(doel, basis + 4);    // rustig optrekken
  else              basis = max(doel, basis - 12);   // vlot afremmen

  int links  = constrain(basis + correctie, -400, 400);
  int rechts = constrain(basis - correctie, -400, 400);

  robot.getHardware().setMotorSpeeds(links, rechts);
  vorigeFout = fout;
}

void GroeneLijnToestand::exit() {
  robot.getHardware().stopMotors();
}
