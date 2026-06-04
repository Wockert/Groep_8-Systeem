// =============================================================
//  RijController
//  Berekent de motorsnelheden en past PD-regeling toe (stuurPD).
//  Meerdere toestanden hebben PD-sturing nodig; door dit apart te
//  zetten staat dezelfde berekening niet op meerdere plekken.
// =============================================================
#pragma once

class RijController {
private:
  int vorigeFout = 0;

public:
  void rijdAfstand(int snelheid, float cm);
  void draaiGraden(int snelheid, int graden, bool links);
  void stuurPD(int basisSnelheid, int fout);
  void stop();
};
