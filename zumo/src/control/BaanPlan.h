// =============================================================
//  BaanPlan  <<checkpoint-systeem>>
//  De baan heeft een VASTE volgorde van kenmerken (zwart lijnvolgen
//  loopt er tussendoor en telt niet als checkpoint). Een detectie
//  (grijs/groen/stippellijn/helling) mag alleen een toestand starten
//  als dat type ook de EERSTVOLGENDE verwachte checkpoint is — dat
//  haalt vrijwel alle valse detecties weg. Optioneel kan per stap
//  een afstandsvenster (cm vanaf de start, via de encoders) worden
//  ingevuld om het nog preciezer te maken: elke afronding print de
//  gemeten afstand, dus na een testrit kun je de venster-kolommen
//  invullen.
// =============================================================
#pragma once

#include <Arduino.h>

// Typen checkpoints op de baan.
enum Checkpoint : uint8_t {
  CP_STIPPELLIJN = 0,
  CP_GROEN,
  CP_KRUISPUNT,
  CP_OMHOOG,        // helling/brug op (neus omhoog)
  CP_WIP,           // wip
  CP_KLAAR          // einde van het plan
};

class BaanPlan {
private:
  uint8_t index   = 0;          // eerstvolgende checkpoint in het plan
  float   startCm = 0.0f;       // encoderstand bij de start; afstanden tellen vanaf hier
  float   laatsteAfrondCm = -1000.0f;   // waar de vorige checkpoint afgerond is
                                        // (CP_MIN_AFSTAND_CM ertussen = geen dubbel tellen)

public:
  void reset(float huidigeCm);              // bij de start (knop A) aanroepen
  Checkpoint verwacht() const;              // welk type is nu aan de beurt?
  // Mag een detectie van dit type NU tellen? Type moet aan de beurt zijn,
  // en als de planstap een afstandsvenster heeft moet de afgelegde weg
  // (vanaf start) erin vallen.
  bool isVerwacht(Checkpoint type, float huidigeCm) const;
  void rondAf(float huidigeCm);             // checkpoint afgerond -> volgende + log
  uint8_t getIndex() const { return index; }
};
