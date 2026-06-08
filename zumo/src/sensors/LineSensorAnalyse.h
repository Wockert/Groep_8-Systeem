// =============================================================
//  LineSensorAnalyse
//  Bepaalt wat de lijnsensor ziet: groene lijn, bruine lijn,
//  grijze tape of niets. Zo hoeft niet elke toestandsklasse zelf
//  drempelwaarden bij te houden.
// =============================================================
#pragma once

class LineSensorAnalyse {
private:
  int  sensorWaarden[5] = {0};       // RAUW: ~0=wit .. ~2000=zwart
  int  grijsNiveau      = 0;         // GEMETEN grijs-delta (boven wit); 0 = niet geijkt
  int  witNiveau();                  // laagste sensor = huidig wit-niveau (drift-referentie)
  bool isGrijs(int waarde);          // ligt de waarde grijs-ver boven het witniveau?

public:
  bool lijnZichtbaar();
  int  aantalZwart();       // aantal sensoren dat zwart ziet
  bool isKruising();        // brede zwarte balk = kruising/t-splitsing
  bool isSplitsing();       // Y-fork: brede zwartzone met een buitenrand zwart
  bool isGroeneLijn();
  bool isBruineLijn();
  bool grijsTapeLinks();
  bool grijsTapeRechts();
  bool grijsTapeBeiden();
  bool startLijnGezien();
  void updateWaarden(const int waarden[]);
  void ijkGrijs(int delta);          // gemeten grijs-delta opslaan (grijs-kalibratie)
  int  getGrijsNiveau() { return grijsNiveau; }
};
