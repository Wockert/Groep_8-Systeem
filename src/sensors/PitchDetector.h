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
