// =============================================================
//  BlokZoeker
//  Zoekt het eindblok en stuurt het duwen aan.
//  Deze klasse wordt pas gebruikt nadat de bruine lijn is gezien.
// =============================================================
#pragma once

#include <Arduino.h>

class ZumoHardware;

class BlokZoeker {
private:
  int besteRichting = 0;
  int besteWaarde   = 0;

  bool aanvalActief = false;
  bool klaar        = false;

  unsigned long aanvalStartTijd       = 0;
  unsigned long laatsteBlokGezienTijd = 0;
  unsigned long negeerZwarteRandTot   = 0;

  int laatsteVerschil = 0;

  bool zwarteRandGezien(ZumoHardware& hardware);
  void keerOmBijRand(ZumoHardware& hardware);
  void stopNaUitduwen(ZumoHardware& hardware);

  void rijMaxNaarBlok(ZumoHardware& hardware, uint8_t linksSensor, uint8_t rechtsSensor);
  void rijDoorMetLaatsteCorrectie(ZumoHardware& hardware);

public:
  void resetAanval();

  bool isKlaar() const;

  bool zoekEnDuwBlok(ZumoHardware& hardware);

  int scanRondom(ZumoHardware& hardware);
};
