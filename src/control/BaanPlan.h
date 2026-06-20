#pragma once

#include <Arduino.h>

// Soorten checkpoints die de robot onderweg tegenkomt.
enum Checkpoint : uint8_t {
  CP_STIPPELLIJN = 0,
  CP_GROEN,
  CP_KRUISPUNT,
  CP_OMHOOG,
  CP_WIP,
  CP_KLAAR
};

// Houdt bij welk checkpoint de robot verwacht en in welke volgorde (de baanvolgorde).
class BaanPlan {
private:
  uint8_t index   = 0;             // huidige stap in het plan
  float   startCm = 0.0f;          // afstand bij start van het plan
  float   laatsteAfrondCm = -1000.0f;  // waar het vorige checkpoint werd afgerond

public:
  void reset(float huidigeCm);                              // begin het plan opnieuw
  Checkpoint verwacht() const;                              // welk checkpoint is nu aan de beurt
  bool isVerwacht(Checkpoint type, float huidigeCm) const;  // klopt dit type met de verwachting
  void rondAf(float huidigeCm);                             // markeer huidig checkpoint als gehaald
  uint8_t getIndex() const { return index; }
};
