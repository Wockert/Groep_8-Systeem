// =============================================================
//  LineSensorAnalyse
//  Bepaalt wat de lijnsensor ziet: groene lijn, bruine lijn,
//  grijze tape of niets. Zo hoeft niet elke toestandsklasse zelf
//  drempelwaarden bij te houden.
// =============================================================
#pragma once

class LineSensorAnalyse {
private:
  unsigned int sensorWaarden[5] = {0};

public:
  bool lijnZichtbaar();
  bool isGroeneLijn();
  bool isBruineLijn();
  bool grijsTapeLinks();
  bool grijsTapeRechts();
  bool grijsTapeBeiden();
  bool startLijnGezien();
  void updateWaarden(unsigned int waarden[]);
};
