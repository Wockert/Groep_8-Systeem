#include <Zumo32U4.h>

class Rijden {
  private:
    int snelheid;
    int richting;
    Zumo32U4Motors motorenVanZumo;
  public:
    Rijden();
    int getSnelheid() const;         // huidige snelheid opvragen
    void setSnelheid(int snelheid);  // snelheid zetten en recht vooruit/achteruit rijden
    void initialiseer() const;       // korte testbeweging bij het opstarten
    void naarLinks();                // op de plek naar links draaien
    void naarRechts();               // op de plek naar rechts draaien
    void Achteruit();                // rijrichting omkeren
    void Stop();                     // motoren stilzetten
};