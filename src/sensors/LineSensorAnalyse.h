#pragma once

class LineSensorAnalyse {
private:
  int  sensorWaarden[5] = {0};
  int  grijsNiveau      = 0;
  int  groenNiveau      = 0;
  int  witNiveau();
  bool isGrijs(int waarde);

public:
  bool lijnZichtbaar();
  int  aantalZwart();
  bool isKruising();
  bool isSplitsing();
  bool isGroeneLijn();
  bool isBruineLijn();
  bool grijsTapeLinks();
  bool grijsTapeRechts();
  bool grijsTapeBeiden();
  bool startLijnGezien();
  void updateWaarden(const int waarden[]);
  void ijkGrijs(int rauweWaarde);
  int  getGrijsNiveau() { return grijsNiveau; }
  void ijkGroen(int gemetenWaarde);
  int  getGroenNiveau() { return groenNiveau; }
};
