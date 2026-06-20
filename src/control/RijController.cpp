#include "RijController.h"
#include "../hardware/ZumoHardware.h"
#include "../config/RobotConfig.h"

void RijController::koppel(ZumoHardware& hw) {
  hardware = &hw;
}

void RijController::rijdAfstand(int snelheid, float cm) {
}

void RijController::draaiGraden(int snelheid, int graden, bool links) {
}

void RijController::stuurPD(int basisSnelheid, int fout) {
  if (hardware == nullptr) return;

  if (abs(fout) <= RobotConfig::LIJN_DODEZONE) {
    vorigeFout = fout;
    hardware->setMotorSpeeds(basisSnelheid, basisSnelheid);
    return;
  }

  int correctie = (int)(RobotConfig::KP * fout + RobotConfig::KD * (fout - vorigeFout));
  vorigeFout = fout;

  correctie = constrain(correctie, -basisSnelheid, basisSnelheid);

  hardware->setMotorSpeeds(basisSnelheid + correctie, basisSnelheid - correctie);
}

void RijController::stop() {
  if (hardware != nullptr) hardware->stopMotors();
}
