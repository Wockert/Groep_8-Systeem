#pragma once

// Zoekt een blok in de omgeving en duwt het naar de rand (nog te implementeren).
class BlokZoeker {
private:
  int besteRichting = 0;   // richting met de sterkste meting
  int besteWaarde   = 0;   // beste gemeten waarde tijdens het scannen

public:
  void zoekEnDuwBlok();          // volledige routine: scannen, draaien en duwen
  int  scanRondom();             // draai rond en zoek de beste richting
  void draaiNaarBlok(int richting);
  void duwTotRand();             // duw het blok tot aan de rand
};
