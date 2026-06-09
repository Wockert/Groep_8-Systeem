#include "GroeneLijnToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "LijnVolgenToestand.h"

GroeneLijnToestand::GroeneLijnToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void GroeneLijnToestand::enter() {
    Serial.println("ENTER: GroeneLijnToestand");
    vorigeFout  = 0;
    zwartTeller = 0;
}

void GroeneLijnToestand::update() {
    // Lijn uitlezen + de gekalibreerde waarden doorgeven aan de kleuranalyse.
    int positie = robot.getHardware().readLine(sensorWaarden);
    robot.getLijnAnalyse().updateWaarden(sensorWaarden);

    // Niet meer groen? -> terug naar de normale (zwarte) lijnvolger.
    if (!robot.getLijnAnalyse().isGroeneLijn()) {
        robot.setState(new LijnVolgenToestand(robot));
        return;
    }

    // Op groen volgen we de lijn met dezelfde PD, maar op halve snelheid.
    // (Groen heeft geen gaten of scherpe bochten, dus geen extra logica nodig.)
    int fout      = positie - 2000;
    int afgeleide = fout - vorigeFout;
    int correctie = (RobotConfig::KP * fout) + (RobotConfig::KD * afgeleide);

    int links  = constrain(RobotConfig::SNELHEID_GROEN + correctie, -400, 400);
    int rechts = constrain(RobotConfig::SNELHEID_GROEN - correctie, -400, 400);

    robot.getHardware().setMotorSpeeds(links, rechts);

    vorigeFout = fout;
}

void GroeneLijnToestand::exit() {
    robot.getHardware().stopMotors();
}
