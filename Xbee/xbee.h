#include <Zumo32U4.h>

class Xbee {
    public:
        Xbee();              // opent de verbinding met de XBee (9600 baud)
        char leesXbee();     // leest één binnengekomen teken
        int beschikbaar();   // aantal tekens dat klaarstaat om te lezen
};