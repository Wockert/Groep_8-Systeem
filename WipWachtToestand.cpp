#include "WipWachtToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "LijnVolgenToestand.h"

WipWachtToestand::WipWachtToestand(ZumoRobot& robot) : RobotToestand(robot) {}

static unsigned long wipStartTijd = 0;
static float hoogstePitch = 0.0f;

void WipWachtToestand::enter() {
  robot.getHardware().print("Wip wacht");

  wipStartTijd = robot.getSensorData().timestamp;
  hoogstePitch = robot.getSensorData().pitch;
}

void WipWachtToestand::update() {
  const SensorData& s = robot.getSensorData();

  pitch = s.pitch;

  if (pitch > hoogstePitch) {
    hoogstePitch = pitch;
  }

  // Eerst nog kort doorrijden, zodat de wip echt naar beneden kan kantelen.
  const unsigned long WIP_DOORRIJD_MS = 500;

  if (s.timestamp - wipStartTijd < WIP_DOORRIJD_MS) {
    robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_WIP,
                                       RobotConfig::SNELHEID_WIP);
    return;
  }

  // Daarna stoppen en wachten.
  robot.getHardware().stopMotors();

  // Verder rijden als de pitch duidelijk lager is dan de hoogste pitch.
  // Dus: eerst /, daarna richting \.
  bool pitchLager = pitch < (hoogstePitch - 2.0f);

  if (pitchLager) {
    if (robot.getBaanPlan().verwacht() == CP_WIP) {
      robot.getBaanPlan().rondAf(s.distanceCm);
    }

    robot.setState(new LijnVolgenToestand(robot));
    return;
  }
}

void WipWachtToestand::exit() {
  robot.getHardware().stopMotors();
}
