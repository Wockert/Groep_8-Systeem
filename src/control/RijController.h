#pragma once

class ZumoHardware;

class RijController {
private:
  int vorigeFout = 0;
  ZumoHardware* hardware = nullptr;

public:
  void koppel(ZumoHardware& hw);
  void rijdAfstand(int snelheid, float cm);
  void draaiGraden(int snelheid, int graden, bool links);
  void stuurPD(int basisSnelheid, int fout);
  void stop();
};
