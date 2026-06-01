// =============================================================
//  PitchDetector
//  Leest de IMU uit en geeft simpele antwoorden terug via
//  isOmhoog(), isVlak() en isOmlaag(). Toestanden hoeven zo geen
//  ruwe pitch-waarden te interpreteren.
// =============================================================
#pragma once

class PitchDetector {
private:
  float laatstePitch = 0.0f;

public:
  float getPitch();
  bool  isOmhoog();
  bool  isVlak();
  bool  isOmlaag();
};
