// =============================================================
//  RijController
//  Berekent de motorsnelheden en past PD-regeling toe (stuurPD).
//  Meerdere toestanden hebben PD-sturing nodig; door dit apart te
//  zetten staat dezelfde berekening niet op meerdere plekken.
// =============================================================
#pragma once

class ZumoHardware;   // forward declaration (alleen pointer nodig)

class RijController {
private:
  int vorigeFout = 0;
  ZumoHardware* hardware = nullptr;   // gekoppeld via koppel() in ZumoRobot::setup()

public:
  void koppel(ZumoHardware& hw);      // geef de controller toegang tot de motoren
  void rijdAfstand(int snelheid, float cm);
  void draaiGraden(int snelheid, int graden, bool links);
  void stuurPD(int basisSnelheid, int fout);
  void stop();
};
