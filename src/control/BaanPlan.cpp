#include "BaanPlan.h"
#include "../config/RobotConfig.h"

// --- HET PLAN: de vaste volgorde van de baan ---
// vanafCm/totCm = afstandsvenster vanaf de start (encoders). 0/0 = venster
// (nog) niet gebruiken — alleen op type gefilterd. Na een testrit: lees de
// "[PLAN] ... op XXX cm" regels af en vul de vensters in (ruim nemen, bv.
// gemeten cm -30 / +30), dan telt een detectie ook alleen op de juiste PLEK.
struct PlanStap {
  Checkpoint type;
  float vanafCm;
  float totCm;
};

static const PlanStap PLAN[] = {
  { CP_STIPPELLIJN, 0, 0 },   //  1: stippellijn
  { CP_STIPPELLIJN, 0, 0 },   //  2: stippellijn
  { CP_GROEN,       0, 0 },   //  3: groene lijn
  { CP_GROEN,       0, 0 },   //  4: groene lijn
  { CP_KRUISPUNT,   0, 0 },   //  5: kruispunt (grijs)
  { CP_KRUISPUNT,   0, 0 },   //  6: kruispunt (grijs)
  { CP_OMHOOG,      0, 0 },   //  7: helling/brug omhoog
  { CP_STIPPELLIJN, 0, 0 },   //  8: stippellijn
  { CP_STIPPELLIJN, 0, 0 },   //  9: stippellijn
  { CP_KRUISPUNT,   0, 0 },   // 10: kruispunt
  { CP_KRUISPUNT,   0, 0 },   // 11: kruispunt
  { CP_KRUISPUNT,   0, 0 },   // 12: kruispunt
  { CP_KRUISPUNT,   0, 0 },   // 13: kruispunt
  { CP_WIP,         0, 0 },   // 14: wip
  { CP_GROEN,       0, 0 },   // 15: groene lijn
  { CP_GROEN,       0, 0 },   // 16: groene lijn
};
static const uint8_t AANTAL = sizeof(PLAN) / sizeof(PLAN[0]);

// Naam voor de logs.
static const __FlashStringHelper* naamVan(Checkpoint type) {
  switch (type) {
    case CP_STIPPELLIJN: return F("STIPPELLIJN");
    case CP_GROEN:       return F("GROEN");
    case CP_KRUISPUNT:   return F("KRUISPUNT");
    case CP_OMHOOG:      return F("OMHOOG");
    case CP_WIP:         return F("WIP");
    default:             return F("KLAAR");
  }
}

void BaanPlan::reset(float huidigeCm) {
  index   = 0;
  startCm = huidigeCm;
  laatsteAfrondCm = -1000.0f;
  Serial.print(F("[PLAN] reset: "));
  Serial.print(AANTAL);
  Serial.print(F(" checkpoints, eerste = "));
  Serial.println(naamVan(PLAN[0].type));
}

Checkpoint BaanPlan::verwacht() const {
  return (index < AANTAL) ? PLAN[index].type : CP_KLAAR;
}

bool BaanPlan::isVerwacht(Checkpoint type, float huidigeCm) const {
  if (index >= AANTAL)          return false;
  if (PLAN[index].type != type) return false;
  // Minimale afstand sinds de vorige afronding: een groen-flapje of het
  // tweede gat van DEZELFDE stippellijn mag niet direct de volgende
  // checkpoint consumeren.
  if (laatsteAfrondCm > -999.0f &&
      huidigeCm - laatsteAfrondCm < RobotConfig::CP_MIN_AFSTAND_CM) return false;
  if (PLAN[index].totCm > 0.0f) {                 // venster ingevuld -> ook op plek filteren
    float rel = huidigeCm - startCm;
    if (rel < PLAN[index].vanafCm || rel > PLAN[index].totCm) return false;
  }
  return true;
}

void BaanPlan::rondAf(float huidigeCm) {
  if (index >= AANTAL) return;
  laatsteAfrondCm = huidigeCm;
  Serial.print(F("[PLAN] checkpoint "));
  Serial.print(index + 1);
  Serial.print(F("/"));
  Serial.print(AANTAL);
  Serial.print(F(" ("));
  Serial.print(naamVan(PLAN[index].type));
  Serial.print(F(") afgerond op "));
  Serial.print(huidigeCm - startCm);
  Serial.println(F(" cm"));
  index++;
  if (index < AANTAL) {
    Serial.print(F("[PLAN] volgende: "));
    Serial.println(naamVan(PLAN[index].type));
  } else {
    Serial.println(F("[PLAN] alle checkpoints afgerond!"));
  }
}
