#pragma once

// =============================================================
//  BlokZoekConfig
//  Aparte instellingen voor het blok zoeken/duwen.
//  Hierdoor blijven de oude parkour- en lijnvolgwaardes in
//  RobotConfig.h onaangeraakt.
// =============================================================
class BlokZoekConfig {
public:

  static constexpr int SNELHEID_MAX = 400;


  static constexpr int SNELHEID_ZOEKEN = 300;
  static constexpr int SNELHEID_DUWEN = 280;
  static constexpr int SNELHEID_DRAAIEN = 260;


  static constexpr int SNELHEID_NAAR_BLOK_LAAG = 260;
  static constexpr int SNELHEID_NAAR_BLOK_MID  = 310;
  static constexpr int SNELHEID_NAAR_BLOK_HOOG = 340;


  static constexpr int SNELHEID_AANVAL_VERLOREN = 270;


  static constexpr int BLOK_STUUR_CORRECTIE = 95;
  static constexpr int BLOK_MAX_CORRECTIE = 190;
  static constexpr int BLOK_MIN_MOTORSNELHEID = 80;


  static constexpr int LAATSTE_CORRECTIE_MAX = 120;


  static constexpr int DREMPEL_BRUIN_L = 450;
  static constexpr int DREMPEL_BRUIN_H = 950;

  static constexpr int MIN_BRUIN_SENSOREN = 2;
  static constexpr int BRUIN_CONFIRMATIES = 2;

  static constexpr int DREMPEL_ZWART_L = 1000;


  static constexpr int DREMPEL_BLOK = 3;


  static constexpr unsigned long NEGEER_ZWART_NA_BRUIN_MS = 900;
  static constexpr unsigned long DOORRIJDEN_NA_BRUIN_MS = 1500;
  static constexpr unsigned long DOORDUWEN_NA_RAND_MS = 500;

  static constexpr unsigned long SCAN_ROND_MS = 1000;
  static constexpr unsigned long VERPLAATS_ZOEK_MS = 300;

  static constexpr unsigned long MIN_AANVAL_TIJD_VOOR_RAND_MS = 500;
  static constexpr unsigned long BLOK_VERLOREN_GRACE_MS = 900;
};
