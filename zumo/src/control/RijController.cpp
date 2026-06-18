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

  // Dode zone: ligt de lijn (bijna) in het midden, dan gewoon rechtdoor.
  // Zonder dit wordt elke sensorruis een stuurcorrectie en staat de robot
  // de hele tijd te trillen terwijl hij al goed op de lijn zit.
  if (abs(fout) <= RobotConfig::LIJN_DODEZONE) {
    vorigeFout = fout;   // bijhouden zodat de D-term geen sprong maakt
    hardware->setMotorSpeeds(basisSnelheid, basisSnelheid);
    return;
  }

  int correctie = (int)(RobotConfig::KP * fout + RobotConfig::KD * (fout - vorigeFout));
  vorigeFout = fout;

  // Correctie begrenzen tot de basissnelheid: het binnenste wiel kan dan
  // hooguit stilstaan, niet vol achteruit slaan (dat schokt/trilt hard).
  correctie = constrain(correctie, -basisSnelheid, basisSnelheid);

  hardware->setMotorSpeeds(basisSnelheid + correctie, basisSnelheid - correctie);
}

void RijController::stop() {
  if (hardware != nullptr) hardware->stopMotors();
}
