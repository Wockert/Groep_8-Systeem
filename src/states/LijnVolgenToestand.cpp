#include "LijnVolgenToestand.h"
#include "../core/ZumoRobot.h"

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void LijnVolgenToestand::enter()  {}
void LijnVolgenToestand::update() {}
void LijnVolgenToestand::exit()   {}

void LijnVolgenToestand::volgLijn()            {}
void LijnVolgenToestand::behandelKruising()    {}
void LijnVolgenToestand::behandelMarkeringen() {}
void LijnVolgenToestand::LijnKwijt()           {}
