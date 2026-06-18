// =============================================================
//  LineSensorAnalyse
//  Bepaalt wat de lijnsensor ziet: groene lijn, bruine lijn,
//  grijze tape of niets. Zo hoeft niet elke toestandsklasse zelf
//  drempelwaarden bij te houden.
// =============================================================
#pragma once

class LineSensorAnalyse {
private:
  int  sensorWaarden[5] = {0};       // GEKALIBREERD: 0=wit .. 1000=zwart (drift-vast)
  int  grijsNiveau      = 0;         // GEMETEN rauwe grijswaarde (knop B); 0 = niet geijkt
  int  groenNiveau      = 0;         // GEMETEN groenwaarde (2e A-stap); 0 = niet geijkt
  int  witNiveau();                  // laagste sensor = huidig wit-niveau (alleen nog voor debug)
  bool isGrijs(int waarde);          // ligt de RAUWE waarde in de grijs-band? (net als zwart)

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
  void ijkGrijs(int rauweWaarde);    // gemeten rauwe grijswaarde opslaan (knop B)
  int  getGrijsNiveau() { return grijsNiveau; }
  void ijkGroen(int gemetenWaarde);  // gemeten groenwaarde opslaan (2e A-stap)
  int  getGroenNiveau() { return groenNiveau; }
};
