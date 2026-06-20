#pragma once

struct SensorData {
  int           lineValues[5] = {0};
  int           linePosition  = 0;
  float         pitch         = 0.0f;
  float         roll          = 0.0f;
  int           proxLeft      = 0;
  int           proxRight     = 0;
  float         distanceCm    = 0.0f;
  bool          buttonA       = false;
  bool          buttonB       = false;
  bool          buttonC       = false;
  unsigned long timestamp     = 0;
};
