#include "LijnVolgenToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "GroeneLijnToestand.h"

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot)
: RobotToestand(robot) {}

void LijnVolgenToestand::enter() {
    Serial.println("ENTER: LijnVolgenToestand");
    vorigeFout    = 0;
    groenTeller   = 0;
    printTeller   = 0;
    laatsteLinks  = RobotConfig::SNELHEID_NORMAAL;
    laatsteRechts = RobotConfig::SNELHEID_NORMAAL;
}

void LijnVolgenToestand::update() {
    // Lijn 1x uitlezen via de façade: 0..4000, 2000 = midden.
    // readLine() vult tegelijk sensorWaarden[] met de gekalibreerde
    // metingen (0..1000 per sensor).
    int positie = robot.getHardware().readLine(sensorWaarden);

    // Hoogste sensorwaarde: ziet minstens 1 sensor nog duidelijk zwart?
    unsigned int maxWaarde = 0;
    for (int i = 0; i < 5; i++)
        if (sensorWaarden[i] > maxWaarde) maxWaarde = sensorWaarden[i];

    // laatsteLinks/laatsteRechts zijn nu members (gezet onderaan deze functie).

    const unsigned int DREMPEL_LIJN  = 200;  // < dit => geen lijn onder de sensoren
    const int          DREMPEL_DRAAI = 150;  // verschil L-R waarboven hij "aan het draaien" was

    // --- Lijn kwijt: was hij aan het DRAAIEN (bocht) of reed hij RECHT (gat)? ---
    if (maxWaarde < DREMPEL_LIJN) {
        if (abs(laatsteLinks - laatsteRechts) > DREMPEL_DRAAI) {
            // Was al aan het draaien -> scherpe bocht -> dezelfde harde draai
            // vasthouden tot de lijn weer onder de sensoren komt (geen tijdslimiet).
            robot.getHardware().setMotorSpeeds(laatsteLinks, laatsteRechts);
        } else {
            // Reed ~rechtdoor -> gat in de lijn -> strak rechtdoor oversteken
            // (geen restdraai, anders draait hij rondjes in een groot gat).
            robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_NORMAAL,
                                               RobotConfig::SNELHEID_NORMAAL);
        }
        return;
    }


    if (++printTeller >= 10) {
        printTeller = 0;
        Serial.print("max = ");
        Serial.println(maxWaarde);
    }

    // --- Groene lijn? -> pas overschakelen na een paar bevestigingen ---
    // De debounce-teller voorkomt dat een korte piek bij een gat of scherpe
    // bocht hem ten onrechte naar GroeneLijnToestand laat springen.
    robot.getLijnAnalyse().updateWaarden(sensorWaarden);
    if (robot.getLijnAnalyse().isGroeneLijn()) {
        if (++groenTeller >= RobotConfig::GROEN_BEVESTIG) {
            robot.setState(new GroeneLijnToestand(robot));
            return;
        }
    } else {
        groenTeller = 0;
    }

    int fout      = positie - 2000;
    int afgeleide = fout - vorigeFout;
    int correctie = (RobotConfig::KP * fout) + (RobotConfig::KD * afgeleide);

    // In scherpe bochten (grote fout) gas terugnemen zodat hij niet voorbij
    // de bocht schiet maar netjes kan draaien.
    int basis = (abs(fout) > 1000) ? RobotConfig::SNELHEID_ZOEKEN
                                   : RobotConfig::SNELHEID_NORMAAL;

    int links  = constrain(basis + correctie, -400, 400);
    int rechts = constrain(basis - correctie, -400, 400);

    robot.getHardware().setMotorSpeeds(links, rechts);

    vorigeFout    = fout;
    laatsteLinks  = links;
    laatsteRechts = rechts;
}


void LijnVolgenToestand::exit() {
    robot.getHardware().stopMotors();
}
