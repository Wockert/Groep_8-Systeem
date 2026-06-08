#include "RijController.h"
#include "../hardware/ZumoHardware.h"
#include "../config/RobotConfig.h"

void RijController::koppel(ZumoHardware& hw) {
  hardware = &hw;
}

void RijController::rijdAfstand(int snelheid, float cm) {
  // (nog niet nodig voor lijnvolgen — wordt later gebruikt voor de 20 cm bij bruin)
}

void RijController::draaiGraden(int snelheid, int graden, bool links) {
  // (nog niet nodig — grijs-bochten doet LijnVolgenToestand zelf)
}

// PD-regeling: stuurt bij op basis van de fout (lijnpositie - midden).
// correctie = KP*fout + KD*(verandering van de fout). Links sneller -> bocht
// naar rechts en andersom.
void RijController::stuurPD(int basisSnelheid, int fout) {
  if (hardware == nullptr) return;

  int correctie = (int)(RobotConfig::KP * fout + RobotConfig::KD * (fout - vorigeFout));
  vorigeFout = fout;

  // Geen aparte clamp op de correctie (branch-logica): setMotorSpeeds() begrenst
  // de uiteindelijke snelheden al naar -400..400.
  hardware->setMotorSpeeds(basisSnelheid + correctie, basisSnelheid - correctie);
}

void RijController::stop() {
  if (hardware != nullptr) hardware->stopMotors();
}
