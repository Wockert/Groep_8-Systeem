#pragma once

#include <Arduino.h>

enum Checkpoint : uint8_t {
  CP_STIPPELLIJN = 0,
  CP_GROEN,
  CP_KRUISPUNT,
  CP_OMHOOG,
  CP_WIP,
  CP_KLAAR
};

class BaanPlan {
private:
  uint8_t index   = 0;
  float   startCm = 0.0f;
  float   laatsteAfrondCm = -1000.0f;

public:
  void reset(float huidigeCm);
  Checkpoint verwacht() const;
  bool isVerwacht(Checkpoint type, float huidigeCm) const;
  void rondAf(float huidigeCm);
  uint8_t getIndex() const { return index; }
};
