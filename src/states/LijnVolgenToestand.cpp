#include "LijnVolgenToestand.h"
#include "BlokZoekToestand.h"

#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "../config/BlokZoekConfig.h"

namespace {
  bool isBruin(unsigned int waarde) {
    return waarde >= BlokZoekConfig::DREMPEL_BRUIN_L &&
           waarde <= BlokZoekConfig::DREMPEL_BRUIN_H;
  }

  bool bruineMarkeringLinksEnRechtsGezien(ZumoHardware& hardware) {
    int bevestigingen = 0;

    for (int meting = 0; meting < BlokZoekConfig::BRUIN_CONFIRMATIES; meting++) {
      unsigned int sensorWaarden[5];

      hardware.readRawLine(sensorWaarden);


      bool bruinLinks =
        isBruin(sensorWaarden[0]) ||
        isBruin(sensorWaarden[1]);

      bool bruinRechts =
        isBruin(sensorWaarden[3]) ||
        isBruin(sensorWaarden[4]);

      if (bruinLinks && bruinRechts) {
        bevestigingen++;
      }

      delay(2);
    }

    bool gezien = bevestigingen >= BlokZoekConfig::BRUIN_CONFIRMATIES;

    if (gezien) {
      Serial.println("Bruine markering links en rechts gezien.");
      hardware.printRawLineSensors();
    }

    return gezien;
  }
}

LijnVolgenToestand::LijnVolgenToestand(ZumoRobot& robot)
: RobotToestand(robot) {}

void LijnVolgenToestand::enter() {
  Serial.println("ENTER: LijnVolgenToestand");
  vorigeFout = 0;
}

void LijnVolgenToestand::update() {
  // =============================================================
  // Eerst controleren of de bruine markering links EN rechts
  // naast de zwarte lijn is gezien.
  // Daarna pas naar blok zoeken.
  // =============================================================
  if (bruineMarkeringLinksEnRechtsGezien(robot.getHardware())) {
    Serial.println("Bruine markering bevestigd. Eerst stukje vooruit, daarna blok zoeken.");

    robot.getHardware().setMotorSpeeds(
      BlokZoekConfig::SNELHEID_DUWEN,
      BlokZoekConfig::SNELHEID_DUWEN
    );

    delay(BlokZoekConfig::DOORRIJDEN_NA_BRUIN_MS);

    robot.getHardware().stopMotors();
    delay(80);

    vorigeFout = 0;

    robot.setState(new BlokZoekToestand(robot));
    return;
  }


  int positie = robot.getHardware().readLine(sensorWaarden);

  int fout      = positie - 2000;
  int afgeleide = fout - vorigeFout;

  int correctie =
    (RobotConfig::KP * fout) +
    (RobotConfig::KD * afgeleide);

  int links = constrain(
    RobotConfig::SNELHEID_NORMAAL + correctie,
    -RobotConfig::SNELHEID_MAX,
     RobotConfig::SNELHEID_MAX
  );

  int rechts = constrain(
    RobotConfig::SNELHEID_NORMAAL - correctie,
    -RobotConfig::SNELHEID_MAX,
     RobotConfig::SNELHEID_MAX
  );

  robot.getHardware().setMotorSpeeds(links, rechts);

  vorigeFout = fout;
}

void LijnVolgenToestand::exit() {
  robot.getHardware().stopMotors();
}
