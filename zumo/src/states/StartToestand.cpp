#include "StartToestand.h"
#include "../core/ZumoRobot.h"
#include "LijnVolgenToestand.h"

StartToestand::StartToestand(ZumoRobot& robot) : RobotToestand(robot) {}

// Kalibratie-stap (overleeft toestandswissels):
// 0=niets, 1=zwart gedaan, 2=groen gedaan (alles klaar -> C mag starten).
static int kalStap = 0;

void StartToestand::enter() {
  robot.getHardware().stopMotors();
  robot.getHardware().print("A=ZWART, dan A=GROEN, C=start");
  aLosgelaten = false;   // kwam hij hier via een stop met A, dan moet A eerst los
}

void StartToestand::update() {
  const SensorData& s = robot.getSensorData();

  // --- Knop B → grijs ijken (NA het zwart kalibreren met knop A!) ---
  // Zet de Zumo zo neer dat één sensor op GRIJS ligt en de rest op WIT.
  if (!s.buttonB) {
    bLosgelaten = true;
  } else if (bLosgelaten) {
    if (kalStap < 1) {
      robot.getHardware().print("Eerst A (zwart kalib)");
      Serial.println(F("[GRIJS IJKEN] eerst zwart kalibreren (knop A) voordat je grijs ijkt"));
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

  // --- Knop A → KLEUREN KALIBREREN (sequentie) ---
  // Elke druk = de volgende kleur, in vaste volgorde: zwart, dan groen. Zet de
  // robot telkens OP die kleur en druk A. Pas na zwart EN groen mag C starten.
  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    aLosgelaten = false;
    if (kalStap == 0) {
      // Stap 1: ZWART — wit/zwart-kalibratie (robot OP de zwarte lijn). Dit zet
      // de gekalibreerde 0..1000-schaal die alle volgende metingen nodig hebben.
      robot.getHardware().print("ZWART kalibreren (draait)...");
      if (robot.getHardware().kalibreerLijn()) {   // robot moet OP de lijn staan
        kalStap = 1;
        robot.getHardware().print("Zet op GROEN, druk A");
        Serial.println(F("[KALIBRATIE] zwart klaar -> zet op GROEN, druk A"));
      } else {
        // Mislukt (sensor zag te weinig contrast): NIET verder.
        robot.getHardware().print("Kalib FOUT! A=retry");
      }
    } else if (kalStap == 1) {
      // Stap 2: GROEN (robot OP de groene lijn) — zelfde heen-en-weer-veeg als
      // zwart, maar nu MEET hij de groen-piek (sensor 1-3). Zet groenNiveau
      // zodat isGroeneLijn() de geijkte band (groenNiveau +- GROEN_MARGE) gebruikt.
      robot.getHardware().print("GROEN ijken (draait)...");
      int groen = robot.getHardware().kalibreerGroenVegend();
      robot.getLijnAnalyse().ijkGroen(groen);
      kalStap = 2;
      robot.getHardware().print("Klaar! Druk C = start");
      Serial.print(F("[KALIBRATIE] groen op "));
      Serial.print(groen);
      Serial.println(F(" -> alles klaar, C = start"));
    }
    return;
  }

  // --- Knop C → STARTEN (na zwart + groen kalibreren) ---
  if (!s.buttonC) {
    cLosgelaten = true;
  } else if (cLosgelaten) {
    cLosgelaten = false;
    if (kalStap < 2) {
      robot.getHardware().print("Eerst A: zwart + groen");
      Serial.print(F("[START] eerst zwart en groen kalibreren (knop A), nu bij stap "));
      Serial.println(kalStap);
      return;
    }
    robot.setState(new LijnVolgenToestand(robot));
    return;   // belangrijk: oude toestand is hierna verwijderd
  }
}

void StartToestand::exit() {}
