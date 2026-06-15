// =============================================================
//  Debug.h  <<utility>>
//  Centrale debug-uitgang voor de RIJ-event-logs ([PLAN], [GRIJS],
//  [AFSLAG], [STIPPELLIJN], ...). Achter een compile-time schakelaar:
//
//    DEBUG 1 -> DBG = USB Serial: je ziet de logs in de Serial Monitor.
//    DEBUG 0 -> DBG = NullSerial: elke DBG.print/println compileert weg,
//               inclusief de F("...")-strings. Bespaart flash (wedstrijd).
//
//  LET OP: de grijs-ijk + kalibratie-feedback (knop B) gebruikt bewust
//  GEWOON Serial (niet DBG), zodat die ALTIJD zichtbaar blijft.
//
//  Gebruik in de code: DBG.print(...) / DBG.println(...).
// =============================================================
#pragma once

#include <Arduino.h>

// Zet op 0 voor de wedstrijd (kleinste binary), op 1 om mee te lezen.
#define DEBUG 0

#if DEBUG
  #define DBG Serial
#else
  // Lege "uitgang": alle print/println-aanroepen worden inline niets en de
  // meegegeven argumenten (ook F-strings) worden door -Os geheel verwijderd.
  struct NullSerial {
    void begin(unsigned long) {}
    template <typename... T> void print(T...)   {}
    template <typename... T> void println(T...) {}
    void println() {}
  };
  extern NullSerial DBG;   // exact één definitie (in ZumoRobot.cpp)
#endif
