#pragma once

// Bepaalt uit de pitch of de robot omhoog/vlak/omlaag staat (nog te implementeren).
class PitchDetector {
private:
  float laatstePitch = 0.0f;

public:
  float getPitch();   // huidige pitch (graden)
  bool  isOmhoog();    // helt naar boven
  bool  isVlak();      // ongeveer horizontaal
  bool  isOmlaag();    // helt naar beneden
};
