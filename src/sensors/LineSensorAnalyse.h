#pragma once

// Interpreteert de lijnsensorwaarden: zwart/grijs/groen, kruisingen, splitsingen.
class LineSensorAnalyse {
private:
  int  sensorWaarden[5] = {0};   // laatst ingelezen sensorwaarden
  int  grijsNiveau      = 0;     // geijkt grijsniveau (0 = niet geijkt)
  int  groenNiveau      = 0;     // geijkt groenniveau (0 = niet geijkt)
  int  witNiveau();              // laagste (lichtste) sensorwaarde van dit moment
  bool isGrijs(int waarde);      // valt deze waarde binnen de grijs-band

public:
  bool lijnZichtbaar();          // ziet minstens één sensor zwart
  int  aantalZwart();            // aantal sensoren boven de zwart-drempel
  bool isKruising();             // brede dwarsbalk met zwart in het midden
  bool isSplitsing();            // brede zwartzone met een buitenrand zwart
  bool isGroeneLijn();
  bool isBruineLijn();
  bool grijsTapeLinks();         // grijze markering aan de linkerrand
  bool grijsTapeRechts();        // grijze markering aan de rechterrand
  bool grijsTapeBeiden();
  bool startLijnGezien();
  void updateWaarden(const int waarden[]);   // nieuwe sensorwaarden inladen
  void ijkGrijs(int rauweWaarde);            // grijsniveau kalibreren
  int  getGrijsNiveau() { return grijsNiveau; }
  void ijkGroen(int gemetenWaarde);          // groenniveau kalibreren
  int  getGroenNiveau() { return groenNiveau; }
};
