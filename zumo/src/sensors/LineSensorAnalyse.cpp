#include "LineSensorAnalyse.h"
#include "../config/RobotConfig.h"

// Sla de gekalibreerde waarden van deze ronde op (komen uit de snapshot).
void LineSensorAnalyse::updateWaarden(const int waarden[]) {
  for (int i = 0; i < 5; i++) sensorWaarden[i] = waarden[i];
}

// Het huidige wit-niveau = de laagste sensorwaarde van deze meting. Dit schuift
// mee als de rauwe waarden driften, dus grijs/wit blijven gescheiden.
int LineSensorAnalyse::witNiveau() {
  int m = sensorWaarden[0];
  for (int i = 1; i < 5; i++) if (sensorWaarden[i] < m) m = sensorWaarden[i];
  return m;
}

// Grijs-kalibratie: sla het gemeten grijs-delta op (grijs boven wit).
void LineSensorAnalyse::ijkGrijs(int delta) {
  grijsNiveau = delta;
}

// Grijs = de sensor ligt grijs-ver BOVEN het witniveau (relatief, dus drift-vast)
// ÉN is zelf niet al zwart. Is grijs geijkt (grijsNiveau>0)? Dan een band ROND
// het gemeten niveau (half .. anderhalf keer). Anders de vaste drempels.
bool LineSensorAnalyse::isGrijs(int waarde) {
  int delta = waarde - witNiveau();
  int laag  = (grijsNiveau > 0) ? grijsNiveau / 2              : RobotConfig::GRIJS_DELTA_L;
  int hoog  = (grijsNiveau > 0) ? grijsNiveau + grijsNiveau / 2 : RobotConfig::GRIJS_DELTA_H;
  return delta > laag && delta < hoog && waarde < RobotConfig::DREMPEL_ZWART;
}

// Aantal sensoren dat zwart ziet.
int LineSensorAnalyse::aantalZwart() {
  int n = 0;
  for (int i = 0; i < 5; i++) {
    if (sensorWaarden[i] > RobotConfig::DREMPEL_ZWART) n++;
  }
  return n;
}

// Ziet minstens één sensor zwart? Dan is de lijn in beeld.
bool LineSensorAnalyse::lijnZichtbaar() {
  return aantalZwart() >= 1;
}

// Kruising (loodrechte dwarsbalk): breed ÉN het midden zwart. De middeneis
// houdt scherpe bochten / de zwarte cirkel (tangentieel) eruit.
bool LineSensorAnalyse::isKruising() {
  return aantalZwart() >= 4 && sensorWaarden[2] > RobotConfig::DREMPEL_ZWART;
}

// Y-splitsing: brede zwartzone (>= DREMPEL_SPLITS_MIN) met een BUITENrand zwart
// (de lijn splitst / wordt breder naar een kant). Grijs wordt alleen op de
// rechte aanloop gelatcht, dus al vuurt dit op de cirkel: zonder onthouden
// richting gebeurt er niets.
bool LineSensorAnalyse::isSplitsing() {
  return aantalZwart() >= RobotConfig::DREMPEL_SPLITS_MIN
      && (sensorWaarden[0] > RobotConfig::DREMPEL_ZWART
          || sensorWaarden[4] > RobotConfig::DREMPEL_ZWART);
}

bool LineSensorAnalyse::isGroeneLijn() {
  return false;   // (nog niet nodig voor deze opdracht)
}

bool LineSensorAnalyse::isBruineLijn() {
  return false;   // (nog niet nodig voor deze opdracht)
}

// Grijze markering links = linker buitenste sensor grijs, ÉN sterker dan z'n
// binnenbuur (een LOSSE markering; bij een bocht is de binnenbuur juist hoger
// omdat die naar de zwarte lijn loopt), ÉN sterker dan de rechterkant.
bool LineSensorAnalyse::grijsTapeLinks() {
  return isGrijs(sensorWaarden[0])
      && sensorWaarden[0] > sensorWaarden[1]     // losse markering, geen lijnrand-gradient
      && sensorWaarden[0] >= sensorWaarden[4];   // sterkste kant wint
}

// Grijze markering rechts = idem, gespiegeld.
bool LineSensorAnalyse::grijsTapeRechts() {
  return isGrijs(sensorWaarden[4])
      && sensorWaarden[4] > sensorWaarden[3]
      && sensorWaarden[4] >  sensorWaarden[0];
}

// Grijs aan beide kanten tegelijk.
bool LineSensorAnalyse::grijsTapeBeiden() {
  return grijsTapeLinks() && grijsTapeRechts();
}

bool LineSensorAnalyse::startLijnGezien() {
  return false;   // (nog niet nodig voor deze opdracht)
}
