// =============================================================
//  BlokZoeker
//  Zoekt het eindblok en stuurt het duwen aan. Slaat de beste
//  richting en sensorwaarde op tijdens het scannen en rijdt
//  daarna het blok uit de cirkel.
// =============================================================
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
