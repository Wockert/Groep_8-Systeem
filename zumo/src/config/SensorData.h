// =============================================================
//  SensorData  <<snapshot>>
//  Een momentopname van alle sensorwaarden tegelijk. Elke ronde
//  eenmaal gevuld, zodat elke klasse precies dezelfde meting ziet
//  en er geen tegenstrijdige beslissingen ontstaan.
// =============================================================
#pragma once

struct SensorData {
  int           lineValues[5] = {0};
  int           linePosition  = 0;     // 0..4000, 2000 = lijn in het midden
  float         pitch         = 0.0f;
  float         roll          = 0.0f;
  int           proxLeft      = 0;
  int           proxRight     = 0;
  float         distanceCm    = 0.0f;
  bool          buttonA       = false;
  bool          buttonB       = false;   // voor grijs ijken in StartToestand
  unsigned long timestamp     = 0;
};
