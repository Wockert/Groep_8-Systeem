#include "BaanPlan.h"
#include "../config/RobotConfig.h"

struct PlanStap {
  Checkpoint type;
  float vanafCm;
  float totCm;
};

// Vaste volgorde van checkpoints op de baan (vanafCm/totCm = 0 betekent: geen afstandsgrens).
static const PlanStap PLAN[] = {
  { CP_STIPPELLIJN, 0, 0 },
  { CP_STIPPELLIJN, 0, 0 },
  { CP_GROEN,       0, 0 },
  { CP_GROEN,       0, 0 },
  { CP_KRUISPUNT,   0, 0 },
  { CP_KRUISPUNT,   0, 0 },
  { CP_OMHOOG,      0, 0 },
  { CP_STIPPELLIJN, 0, 0 },
  { CP_STIPPELLIJN, 0, 0 },
  { CP_KRUISPUNT,   0, 0 },
  { CP_KRUISPUNT,   0, 0 },
  { CP_KRUISPUNT,   0, 0 },
  { CP_KRUISPUNT,   0, 0 },
  { CP_WIP,         0, 0 },
  { CP_GROEN,       0, 0 },
  { CP_GROEN,       0, 0 },
};
static const uint8_t AANTAL = sizeof(PLAN) / sizeof(PLAN[0]);

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

// Klopt het aangeboden type met de verwachting, en zijn we ver genoeg van het vorige checkpoint?
bool BaanPlan::isVerwacht(Checkpoint type, float huidigeCm) const {
  if (index >= AANTAL)          return false;
  if (PLAN[index].type != type) return false;
  // Minimale afstand sinds vorig checkpoint, om dubbel-detecteren te voorkomen.
  if (laatsteAfrondCm > -999.0f &&
      huidigeCm - laatsteAfrondCm < RobotConfig::CP_MIN_AFSTAND_CM) return false;
  if (PLAN[index].totCm > 0.0f) {
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
