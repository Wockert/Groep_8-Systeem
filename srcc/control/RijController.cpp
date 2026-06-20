#include "RijController.h"
#include "../hardware/ZumoHardware.h"
#include "../config/RobotConfig.h"

void RijController::koppel(ZumoHardware& hw) {
  hardware = &hw;
}

// Nog niet geïmplementeerd.
void RijController::rijdAfstand(int snelheid, float cm) {
}

// Nog niet geïmplementeerd.
void RijController::draaiGraden(int snelheid, int graden, bool links) {
}

// PD-regeling: stuurt bij op basis van de lijnfout en de verandering daarvan.
void RijController::stuurPD(int basisSnelheid, int fout) {
  if (hardware == nullptr) return;

  // Kleine fout binnen de dodezone: gewoon rechtdoor, niet bijsturen.
  if (abs(fout) <= RobotConfig::LIJN_DODEZONE) {
    vorigeFout = fout;
    hardware->setMotorSpeeds(basisSnelheid, basisSnelheid);
    return;
  }

  // P-term op de fout + D-term op de verandering t.o.v. de vorige stap.
  int correctie = (int)(RobotConfig::KP * fout + RobotConfig::KD * (fout - vorigeFout));
  vorigeFout = fout;

  correctie = constrain(correctie, -basisSnelheid, basisSnelheid);

  // Correctie tegengesteld over de twee wielen verdelen om te draaien.
  hardware->setMotorSpeeds(basisSnelheid + correctie, basisSnelheid - correctie);
}

void RijController::stop() {
  if (hardware != nullptr) hardware->stopMotors();
}
