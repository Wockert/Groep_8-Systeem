#include "xbee.h"

Xbee::Xbee() {
    Serial1.begin(9600);
}

char Xbee::leesXbee() {
    return Serial1.read();
}