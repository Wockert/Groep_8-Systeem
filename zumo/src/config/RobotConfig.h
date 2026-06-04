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
  static constexpr int SNELHEID_MAX      = 0;
  static constexpr int SNELHEID_NORMAAL  = 0;
  static constexpr int SNELHEID_GROEN    = 0;
  static constexpr int SNELHEID_WIP      = 0;
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
  // Door het team gemeten en getest: 0.019 cm per encodertick/pulse.
  // (afstand = gemiddelde van links+rechts ticks * deze waarde)
  static constexpr float CM_PER_PULSE    = 0.019f;

  // --- PD-regeling ---
  static constexpr float KP = 0.0f;
  static constexpr float KD = 0.0f;
};
