#include "LineSensorAnalyse.h"
#include "../config/RobotConfig.h"

bool LineSensorAnalyse::lijnZichtbaar() {
  return false;
}

bool LineSensorAnalyse::isGroeneLijn() {
  // De donkerste sensor geeft de kleur van de lijn recht onder de robot.
  unsigned int maxWaarde = 0;
  for (int i = 0; i < 5; i++)
    if (sensorWaarden[i] > maxWaarde) maxWaarde = sensorWaarden[i];

  // Groen kaatst meer IR terug dan zwart -> een middenwaarde tussen wit en zwart.
  return maxWaarde >= RobotConfig::DREMPEL_GROEN_L
      && maxWaarde <= RobotConfig::DREMPEL_GROEN_H;
}

bool LineSensorAnalyse::isBruineLijn() {
  return false;
}

bool LineSensorAnalyse::grijsTapeLinks() {
  return false;
}

bool LineSensorAnalyse::grijsTapeRechts() {
  return false;
}

bool LineSensorAnalyse::grijsTapeBeiden() {
  return false;
}

bool LineSensorAnalyse::startLijnGezien() {
  return false;
}

void LineSensorAnalyse::updateWaarden(unsigned int waarden[]) {
  for (int i = 0; i < 5; i++) sensorWaarden[i] = waarden[i];
}
