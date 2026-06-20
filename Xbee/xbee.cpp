#include "xbee.h"

// Opent de seriële verbinding met de XBee (9600 baud, gelijk aan de BD-instelling).
Xbee::Xbee() {
    Serial1.begin(9600);
}

// Leest één binnengekomen teken (-1 als er niets klaarstaat).
char Xbee::leesXbee() {
    return Serial1.read();
}

// Geeft terug hoeveel tekens er klaarstaan om gelezen te worden.
int Xbee::beschikbaar() {
    return Serial1.available();
}