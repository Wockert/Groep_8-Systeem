#include "BaanPlan.h"
#include "../config/RobotConfig.h"
#include "../config/Debug.h"
#include "../hardware/ZumoHardware.h"   // voor de checkpoint-piep in rondAf()

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
  // TIJDELIJK startpunt: begin niet altijd bij checkpoint 1, maar bij de in
  // RobotConfig ingestelde index (bv. 4 = eerste kruispunt/grijs). Begrensd,
  // zodat een te hoge waarde het plan niet meteen op KLAAR zet.
  index   = (RobotConfig::START_CHECKPOINT >= 0 &&
             RobotConfig::START_CHECKPOINT < AANTAL)
              ? (uint8_t)RobotConfig::START_CHECKPOINT : 0;
  startCm = huidigeCm;
  laatsteAfrondCm = -1000.0f;
  DBG.print(F("[PLAN] reset: "));
  DBG.print(AANTAL);
  DBG.print(F(" checkpoints, start bij "));
  DBG.print(index + 1);
  DBG.print(F(" = "));
  DBG.println(naamVan(PLAN[index].type));
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
  // Hoorbaar: elke checkpoint-afronding (+1) geeft een korte piep. Centraal
  // hier, zodat het GEGARANDEERD klinkt vanuit elke toestand die rondAf()
  // aanroept (kruispunt, groen, stippellijn, helling) — niet per plek apart.
  if (hardware != nullptr) hardware->speelCheckpointGeluid();
  DBG.print(F("[PLAN] checkpoint "));
  DBG.print(index + 1);
  DBG.print(F("/"));
  DBG.print(AANTAL);
  DBG.print(F(" ("));
  DBG.print(naamVan(PLAN[index].type));
  DBG.print(F(") afgerond op "));
  DBG.print(huidigeCm - startCm);
  DBG.println(F(" cm"));
  index++;
  if (index < AANTAL) {
    DBG.print(F("[PLAN] volgende: "));
    DBG.println(naamVan(PLAN[index].type));
  } else {
    DBG.println(F("[PLAN] alle checkpoints afgerond!"));
  }
}
