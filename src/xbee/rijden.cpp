#include "rijden.h"

Rijden::Rijden():snelheid(0) {}

int Rijden::getSnelheid() const {
  return snelheid;
}

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

void Rijden::setSnelheid(int nieuweSnelheid) {
  snelheid = nieuweSnelheid;
  motorenVanZumo.setSpeeds(nieuweSnelheid, nieuweSnelheid);
}

void Rijden::naarLinks() {
  motorenVanZumo.setSpeeds(snelheid * -1, snelheid);
}

void Rijden::naarRechts() {
  motorenVanZumo.setSpeeds(snelheid, snelheid * -1);
}

void Rijden::Stop() {
  snelheid = 0;
  motorenVanZumo.setSpeeds(0, 0);
}

void Rijden::Achteruit() {
  snelheid *= -1;
  motorenVanZumo.setSpeeds(snelheid, snelheid);
}