// =============================================================
//  LijnVolgenToestand
//  Volgt de lijn met PD-sturing en behandelt kruisingen,
//  markeringen en lijn-verlies.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class LijnVolgenToestand : public RobotToestand {
private:
    unsigned int sensorWaarden[5];
    int vorigeFout    = 0;
    int groenTeller   = 0;   // hoeveel rondes achter elkaar groen gezien (debounce)
    int printTeller   = 0;   // voor de tijdelijke "max =" debugprint
    int laatsteLinks  = 0;   // laatst gestuurde motorsnelheden (voor lijn-kwijt-logica)
    int laatsteRechts = 0;

public:
    LijnVolgenToestand(ZumoRobot& robot);

    void enter() override;
    void update() override;
    void exit() override;
};

