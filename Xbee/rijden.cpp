#include "rijden.h"

// Start met snelheid 0 (stilstand).
Rijden::Rijden():snelheid(0) {}

// Geeft de huidige snelheid terug.
int Rijden::getSnelheid() const {
  return snelheid;
}

// Korte testbeweging bij het opstarten: links, rechts, links, stop.
void Rijden::initialiseer() const {
  setSnelheid(200);
  naarLinks();
  delay(1000);
  naarRechts();
  delay(2000);
  naarLinks();
  delay(1000);
  Stop();
}

// Zet de snelheid en rijdt recht vooruit (of achteruit bij een negatieve waarde).
void Rijden::setSnelheid(int nieuweSnelheid) {
  snelheid = nieuweSnelheid;
  motorenVanZumo.setSpeeds(nieuweSnelheid, nieuweSnelheid);
}

// Draait op de plek naar links: linkerwiel achteruit, rechterwiel vooruit.
void Rijden::naarLinks() {
  motorenVanZumo.setSpeeds(snelheid * -1, snelheid);
}

// Draait op de plek naar rechts: linkerwiel vooruit, rechterwiel achteruit.
void Rijden::naarRechts() {
  motorenVanZumo.setSpeeds(snelheid, snelheid * -1);
}

// Zet beide motoren stil.
void Rijden::Stop() {
  snelheid = 0;
  motorenVanZumo.setSpeeds(0, 0);
}

// Keert de rijrichting om (vooruit <-> achteruit) met dezelfde snelheid.
void Rijden::Achteruit() {
  snelheid *= -1;
  motorenVanZumo.setSpeeds(snelheid, snelheid);
}