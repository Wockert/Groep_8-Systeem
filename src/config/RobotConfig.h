// =============================================================
//  RobotConfig  <<utility>>
//  Alle instellingen op EEN plek: snelheden, kleurdrempels,
//  PID-waarden. Tijdens het testen pas je deze tientallen keren
//  aan; door ze hier te bundelen hoef je niet door de code te zoeken.
//  (Waarden zijn placeholders - afstellen tijdens het testen.)
// =============================================================
#pragma once

class RobotConfig {
public:
  // --- Snelheden ---
  static constexpr int SNELHEID_MAX      = 400;
  static constexpr int SNELHEID_NORMAAL  = 200;
  static constexpr int SNELHEID_GROEN    = 200;
  static constexpr int SNELHEID_WIP      = 350;
  static constexpr int SNELHEID_ZOEKEN   = 0;

  // --- Kleurdrempels ---
  static constexpr int DREMPEL_ZWART     = 0;
  static constexpr int DREMPEL_GRIJS_L   = 0;
  static constexpr int DREMPEL_GRIJS_H   = 0;
  static constexpr int DREMPEL_GROEN_L   = 0;
  static constexpr int DREMPEL_GROEN_H   = 0;
  static constexpr int DREMPEL_BRUIN_L   = 0;
  static constexpr int DREMPEL_BRUIN_H   = 0;

  // --- Timing & afstand ---
  static constexpr int   GEHEUGEN_MS     = 0;
  static constexpr int   DOORRIJDEN_CM   = 0;
  static constexpr float CM_PER_PULSE    = 0.0f;

  // --- PD-regeling ---
  static constexpr float KP = 0.0f;
  static constexpr float KD = 0.0f;
};
