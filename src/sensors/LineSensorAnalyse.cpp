#include "LineSensorAnalyse.h"
#include "../config/RobotConfig.h"

void LineSensorAnalyse::updateWaarden(const int waarden[]) {
  for (int i = 0; i < 5; i++) sensorWaarden[i] = waarden[i];
}

int LineSensorAnalyse::witNiveau() {
  int m = sensorWaarden[0];
  for (int i = 1; i < 5; i++) if (sensorWaarden[i] < m) m = sensorWaarden[i];
  return m;
}

void LineSensorAnalyse::ijkGrijs(int waarde) {
  grijsNiveau = waarde;
}

bool LineSensorAnalyse::isGrijs(int waarde) {
  int laag = (grijsNiveau > 0) ? grijsNiveau - RobotConfig::GRIJS_MARGE : RobotConfig::DREMPEL_GRIJS_L;
  int hoog = (grijsNiveau > 0) ? grijsNiveau + RobotConfig::GRIJS_MARGE : RobotConfig::DREMPEL_GRIJS_H;
  return waarde > laag && waarde < hoog && waarde < RobotConfig::DREMPEL_ZWART;
}

int LineSensorAnalyse::aantalZwart() {
  int n = 0;
  for (int i = 0; i < 5; i++) {
    if (sensorWaarden[i] > RobotConfig::DREMPEL_ZWART) n++;
  }
  return n;
}

bool LineSensorAnalyse::lijnZichtbaar() {
  return aantalZwart() >= 1;
}

bool LineSensorAnalyse::isKruising() {
  return aantalZwart() >= 4 && sensorWaarden[2] > RobotConfig::DREMPEL_ZWART;
}

bool LineSensorAnalyse::isSplitsing() {
  return aantalZwart() >= RobotConfig::DREMPEL_SPLITS_MIN
      && (sensorWaarden[0] > RobotConfig::DREMPEL_ZWART
          || sensorWaarden[4] > RobotConfig::DREMPEL_ZWART);
}

void LineSensorAnalyse::ijkGroen(int waarde) {
  groenNiveau = waarde;
}

bool LineSensorAnalyse::isGroeneLijn() {
  int maxWaarde = 0;
  for (int i = 1; i <= 3; i++)
    if (sensorWaarden[i] > maxWaarde) maxWaarde = sensorWaarden[i];

  int laag = (groenNiveau > 0) ? groenNiveau - RobotConfig::GROEN_MARGE : RobotConfig::DREMPEL_GROEN_L;
  int hoog = (groenNiveau > 0) ? groenNiveau + RobotConfig::GROEN_MARGE : RobotConfig::DREMPEL_GROEN_H;
  return maxWaarde >= laag && maxWaarde <= hoog;
}

bool LineSensorAnalyse::isBruineLijn() {
  return false;
}

bool LineSensorAnalyse::grijsTapeLinks() {
  return isGrijs(sensorWaarden[0])
      && sensorWaarden[0] - witNiveau() > RobotConfig::GRIJS_BOVEN_WIT
      && sensorWaarden[1] < RobotConfig::DREMPEL_GRIJS_L
      && sensorWaarden[0] >= sensorWaarden[4];
}

bool LineSensorAnalyse::grijsTapeRechts() {
  return isGrijs(sensorWaarden[4])
      && sensorWaarden[4] - witNiveau() > RobotConfig::GRIJS_BOVEN_WIT
      && sensorWaarden[3] < RobotConfig::DREMPEL_GRIJS_L
      && sensorWaarden[4] >  sensorWaarden[0];
}

bool LineSensorAnalyse::grijsTapeBeiden() {
  return grijsTapeLinks() && grijsTapeRechts();
}

bool LineSensorAnalyse::startLijnGezien() {
  return false;
}
