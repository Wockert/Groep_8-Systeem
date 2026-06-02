#include <Zumo32U4.h>

class Rijden {
  private:
    int snelheid;
    int richting;
    Zumo32U4Motors motorenVanZumo;
  public:
    Rijden();
    int getSnelheid() const;
    void setSnelheid(int snelheid);
    void initialiseer() const;
    void naarLinks();
    void naarRechts();
    void Achteruit();
    void Stop();
};