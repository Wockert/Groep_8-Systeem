#pragma once

class BlokZoeker {
private:
  int besteRichting = 0;
  int besteWaarde   = 0;

public:
  void zoekEnDuwBlok();
  int  scanRondom();
  void draaiNaarBlok(int richting);
  void duwTotRand();
};
