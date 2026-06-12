#include "StartToestand.h"
#include "../core/ZumoRobot.h"
#include "LijnVolgenToestand.h"
#include "RechtdoorToestand.h"

StartToestand::StartToestand(ZumoRobot& robot) : RobotToestand(robot) {}

// Eenmalig kalibreren bij de EERSTE druk op A (overleeft toestandswissels,
// dus na een stop met A wordt er niet opnieuw gekalibreerd).
static bool lijnGekalibreerd = false;

void StartToestand::enter() {
  robot.getHardware().stopMotors();
  robot.getHardware().print("Druk A = start");
  aLosgelaten = false;   // kwam hij hier via een stop met A, dan moet A eerst los
}

void StartToestand::update() {
  const SensorData& s = robot.getSensorData();

  // --- Knop B → grijs ijken (NA het kalibreren met knop A!) ---
  // Zet de Zumo zo neer dat één sensor op GRIJS ligt en de rest op WIT.
  // De hoogste gekalibreerde waarde = de grijs-sensor. Die wordt opgeslagen
  // (isGrijs() hanteert dan die waarde +- GRIJS_MARGE, net zoals zwart een
  // vaste drempel heeft) en geprint, zodat je 'm ook met de hand in
  // RobotConfig (DREMPEL_GRIJS_L/H) kunt zetten.
  if (!s.buttonB) {
    bLosgelaten = true;
  } else if (bLosgelaten) {
    // Zonder lijnkalibratie zijn de gekalibreerde waarden onzin: eerst A.
    if (!lijnGekalibreerd) {
      robot.getHardware().print("Eerst A (kalib)");
      Serial.println(F("[GRIJS IJKEN] eerst kalibreren (knop A) voordat je grijs ijkt"));
      bLosgelaten = false;
      return;
    }
    int hoog = s.lineValues[0], laag = s.lineValues[0];
    for (int i = 1; i < 5; i++) {
      if (s.lineValues[i] > hoog) hoog = s.lineValues[i];
      if (s.lineValues[i] < laag) laag = s.lineValues[i];
    }
    robot.getLijnAnalyse().ijkGrijs(hoog);   // rauwe grijswaarde opslaan

    Serial.print(F("[GRIJS IJKEN] grijs="));
    Serial.print(hoog);
    Serial.print(F("  (wit="));  Serial.print(laag);
    Serial.println(F(")"));

    String tekst = "Grijs=" + String(hoog);
    robot.getHardware().print(tekst);
    bLosgelaten = false;   // volgende meting vereist opnieuw indrukken
    return;
  }

  // Knop A → eerste keer: kalibreren en DIRECT starten; daarna gewoon starten.
  // (Pas na een NIEUWE druk: eerst loslaten.)
  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    if (!lijnGekalibreerd) {
      robot.getHardware().print("Kalibreren...");
      if (robot.getHardware().kalibreerLijn()) {   // robot moet OP de lijn staan
        lijnGekalibreerd = true;
      } else {
        // Mislukt (sensor zag te weinig contrast): NIET gaan rijden met een
        // kapotte kalibratie. Zet de robot op de lijn en druk opnieuw A.
        robot.getHardware().print("Kalib FOUT! A=retry");
        aLosgelaten = false;   // nieuwe poging vereist een NIEUWE druk op A
        return;
      }
    }
    // Baanplan opnieuw vanaf checkpoint 1, met de huidige encoderstand
    // als nulpunt voor de afstandsvensters.
    robot.getBaanPlan().reset(robot.getSensorData().distanceCm);
    robot.setState(new LijnVolgenToestand(robot));
    return;   // belangrijk: oude toestand is hierna verwijderd
  }

  // Knop C → 10 seconden rechtdoor rijden (test). Pas na een NIEUWE druk:
  // kom je net uit de test terug terwijl C nog vast zit, dan niet herstarten.
  if (!s.buttonC) {
    cLosgelaten = true;
  } else if (cLosgelaten) {
    robot.setState(new RechtdoorToestand(robot));
    return;   // belangrijk: oude toestand is hierna verwijderd
  }
}

void StartToestand::exit() {}
