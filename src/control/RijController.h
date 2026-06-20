#pragma once

class ZumoHardware;

// Vertaalt rij-opdrachten (rechtdoor, draaien, lijn corrigeren) naar motoraansturing.
class RijController {
private:
  int vorigeFout = 0;              // fout van de vorige stap (voor de D-term)
  ZumoHardware* hardware = nullptr;

public:
  void koppel(ZumoHardware& hw);                          // verbind met de hardware-laag
  void rijdAfstand(int snelheid, float cm);               // rijd een vaste afstand rechtdoor
  void draaiGraden(int snelheid, int graden, bool links); // draai een aantal graden ter plekke
  void stuurPD(int basisSnelheid, int fout);              // corrigeer de koers met PD-regeling
  void stop();                                            // stop beide motoren
};
