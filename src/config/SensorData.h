#pragma once

// Momentopname van alle sensoren op één tijdstip; doorgegeven aan de toestanden.
struct SensorData {
  int           lineValues[5] = {0};   // 5 gekalibreerde lijnsensorwaarden
  int           linePosition  = 0;     // lijnpositie 0..4000 (2000 = midden)
  float         pitch         = 0.0f;  // kanteling voor/achter (graden)
  float         roll          = 0.0f;  // kanteling links/rechts (graden)
  int           proxLeft      = 0;
  int           proxRight     = 0;
  float         distanceCm    = 0.0f;  // afgelegde afstand sinds start (cm)
  bool          buttonA       = false;
  bool          buttonB       = false;
  bool          buttonC       = false;
  unsigned long timestamp     = 0;     // tijdstip van de meting (millis)
};
