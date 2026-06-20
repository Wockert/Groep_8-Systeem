#pragma once

// Centrale instellingen: alle afstelbare constanten voor de robot op één plek.
class RobotConfig {
public:
  static constexpr bool DEBUG_SENSOR_PRINT = false;

  // Snelheden (motorvermogen)
  static constexpr int SNELHEID_MAX      = 250;
  static constexpr int SNELHEID_NORMAAL  = 200;
  static constexpr int SNELHEID_GRIJS    = 120;
  static constexpr int SNELHEID_GROEN    = 200;
  static constexpr int SNELHEID_WIP      = 120;
  static constexpr int SNELHEID_ZOEKEN   = 150;
  static constexpr int SNELHEID_BOCHT    = 220;

  static constexpr int SNELHEID_RECHT    = 300;
  static constexpr int SNELHEID_LAAG     = 150;
  static constexpr int KRUIS_MS          = 350;

  static constexpr bool GRIJS_ACTIEF     = true;

  // Kleur- en lijndrempels (sensorwaarden)
  static constexpr int DREMPEL_ZWART     = 700;
  static constexpr int DREMPEL_GRIJS_L   = 200;
  static constexpr int DREMPEL_GRIJS_H   = 650;
  static constexpr int GRIJS_BEVESTIG    = 2;
  static constexpr int GRIJS_BEVESTIG_LIJN = 2;
  static constexpr int GRIJS_LIJN_MIDDEN   = 800;

  static constexpr int GRIJS_MARGE = 200;
  static constexpr int GRIJS_BOVEN_WIT = 150;

  static constexpr int DREMPEL_SPLITS_MIN = 3;
  static constexpr int SPLITS_BEVESTIG    = 2;
  static constexpr int SPLITS_BIAS        = 2000;
  static constexpr float GRIJS_GELDIG_CM = 40.0f;
  static constexpr float GRIJS_LOCK_CM   = 5.0f;
  static constexpr unsigned long AFSLAG_MS       = 350;
  static constexpr bool GROEN_ACTIEF     = true;
  static constexpr int DREMPEL_GROEN_L   = 250;
  static constexpr int DREMPEL_GROEN_H   = 350;
  static constexpr int GROEN_MARGE       = 90;
  static constexpr int GROEN_BEVESTIG    = 3;
  static constexpr int DREMPEL_BRUIN_L   = 0;
  static constexpr int DREMPEL_BRUIN_H   = 0;


  static constexpr float CM_PER_PULSE    = 0.019f;

  // PID-regeling voor het volgen van de lijn
  static constexpr float KP = 0.70f;
  static constexpr float KD = 2.0f;
  static constexpr float KI           = 0.012f;
  static constexpr long  FOUT_SOM_MAX = 15000;
  static constexpr int   LIJN_DODEZONE = 200;

  // Bochten en afstanden (cm / graden)
  static constexpr int BOCHT_RAND_ZWART = 500;

  static constexpr float BOCHT_AANKONDIG_CM = 10.0f;

  static constexpr float WIELBASIS_CM     = 8.5f;
  static constexpr int   BOCHT_MIN_GRADEN = 20;
  static constexpr int   BOCHT_MAX_GRADEN = 135;
  static constexpr int   BOCHT_START_FOUT = 1000;
  static constexpr int   BOCHT_RAND_DIRECT = 800;

  static constexpr float NABOCHT_CM            = 8.0f;
  static constexpr int   NABOCHT_CORRECTIE_MAX = 150;

  static constexpr float STIPPEL_EINDE_CM    = 8.0f;
  static constexpr float STIPPEL_ZONE_MAX_CM = 35.0f;

  static constexpr float LIJN_KWIJT_GEHEUGEN_CM = 4.0f;

  static constexpr float CP_MIN_AFSTAND_CM = 15.0f;

  // Wip-detectie via kanteling (pitch)
  static constexpr float PITCH_GRADEN   = 13.0f;
  static constexpr int   PITCH_BEVESTIG = 20;
};
