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

// Grijs-kalibratie: sla de gemeten (gekalibreerde) grijswaarde op (knop B).
void LineSensorAnalyse::ijkGrijs(int waarde) {
  grijsNiveau = waarde;
}

// Grijs werkt NET ALS ZWART: een vaste band op de GEKALIBREERDE waarde
// (zwart = waarde > DREMPEL_ZWART; grijs = waarde tussen L en H).
// Is grijs geijkt met knop B (grijsNiveau > 0)? Dan een band van
// +-GRIJS_MARGE rond de gemeten waarde. Anders de vaste drempels.
bool LineSensorAnalyse::isGrijs(int waarde) {
  int laag = (grijsNiveau > 0) ? grijsNiveau - RobotConfig::GRIJS_MARGE : RobotConfig::DREMPEL_GRIJS_L;
  int hoog = (grijsNiveau > 0) ? grijsNiveau + RobotConfig::GRIJS_MARGE : RobotConfig::DREMPEL_GRIJS_H;
  return waarde > laag && waarde < hoog && waarde < RobotConfig::DREMPEL_ZWART;
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

// Groen-kalibratie: sla de gemeten (gekalibreerde) groenwaarde op (2e A-stap).
void LineSensorAnalyse::ijkGroen(int waarde) {
  groenNiveau = waarde;
}

// Groene lijn: de donkerste sensor geeft de kleur recht onder de robot. Groen
// kaatst meer IR terug dan zwart -> een middenwaarde tussen wit en zwart.
// LET OP: alleen de MIDDELSTE drie sensoren (1,2,3). De buitenste (0 en 4)
// blijven onbetrouwbaar op ~1000 hangen; nemen we die mee, dan is de max altijd
// ~1000 en wordt groen NOOIT herkend.
// Is groen geijkt (groenNiveau > 0)? Dan een band van +-GROEN_MARGE rond de
// gemeten waarde. Anders de vaste drempels DREMPEL_GROEN_L..H.
bool LineSensorAnalyse::isGroeneLijn() {
  int maxWaarde = 0;
  for (int i = 1; i <= 3; i++)
    if (sensorWaarden[i] > maxWaarde) maxWaarde = sensorWaarden[i];

  int laag = (groenNiveau > 0) ? groenNiveau - RobotConfig::GROEN_MARGE : RobotConfig::DREMPEL_GROEN_L;
  int hoog = (groenNiveau > 0) ? groenNiveau + RobotConfig::GROEN_MARGE : RobotConfig::DREMPEL_GROEN_H;
  return maxWaarde >= laag && maxWaarde <= hoog;
}

bool LineSensorAnalyse::isBruineLijn() {
  return false;   // (nog niet verwerkt )
}

// Grijze markering links = linker buitenste sensor in de grijs-band, ÉN de
// binnenbuur (sensor 1) WIT (onder de grijs-band). Dat laatste is de sterkste
// eis: een echte losse markering ligt met een witte strook naast de lijn,
// terwijl een valse detectie (bocht-gradient, schaduw, brede lijnrand) juist
// doorloopt in de buursensor. ÉN sterker dan de rechterkant (sterkste wint).
bool LineSensorAnalyse::grijsTapeLinks() {
  return isGrijs(sensorWaarden[0])
      && sensorWaarden[0] - witNiveau() > RobotConfig::GRIJS_BOVEN_WIT  // steekt echt boven wit uit
      && sensorWaarden[1] < RobotConfig::DREMPEL_GRIJS_L  // binnenbuur wit: scheiding lijn<->markering
      && sensorWaarden[0] >= sensorWaarden[4];            // sterkste kant wint
}

// Grijze markering rechts = idem, gespiegeld.
bool LineSensorAnalyse::grijsTapeRechts() {
  return isGrijs(sensorWaarden[4])
      && sensorWaarden[4] - witNiveau() > RobotConfig::GRIJS_BOVEN_WIT
      && sensorWaarden[3] < RobotConfig::DREMPEL_GRIJS_L
      && sensorWaarden[4] >  sensorWaarden[0];
}

// Grijs aan beide kanten tegelijk.
bool LineSensorAnalyse::grijsTapeBeiden() {
  return grijsTapeLinks() && grijsTapeRechts();
}

bool LineSensorAnalyse::startLijnGezien() {
  return false;   // (nog niet verwerkt )
}
