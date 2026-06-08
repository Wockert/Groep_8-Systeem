#include "KruispuntToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "LijnVolgenToestand.h"

KruispuntToestand::KruispuntToestand(ZumoRobot& robot) : RobotToestand(robot) {}

void KruispuntToestand::enter() {
  grijsTijd = millis();   // start van het grijs-venster
  robot.getHardware().print("Kruispunt");
}

void KruispuntToestand::update() {
  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // 1) Bezig met afslaan? Bias naar de gekozen tak voor AFSLAG_MS, dan terug.
  if (bezigMetAfslaan) {
    int positie = robot.getSensorData().linePosition;
    int fout    = (positie - 2000) + afslagRichting * RobotConfig::SPLITS_BIAS;
    robot.getRijController().stuurPD(RobotConfig::SNELHEID_GRIJS, fout);
    if (millis() - afslagTijd > RobotConfig::AFSLAG_MS) {
      Serial.println(F("[AFSLAG] klaar"));
      robot.setState(new LijnVolgenToestand(robot));
    }
    return;   // (na setState is deze toestand verwijderd)
  }

  // 2) Grijs onthouden/bevestigen (met lock + ontdubbeling).
  behandelMarkeringen();

  // 3) Splitsing met een onthouden richting? Begin de afslag.
  if (lijn.isSplitsing() && (grijsLinks || grijsRechts)) {
    bezigMetAfslaan = true;
    afslagRichting  = grijsLinks ? -1 : +1;   // -1 = links, +1 = rechts
    afslagTijd      = millis();
    Serial.println(F("[AFSLAG] start"));
    return;
  }

  // 4) Geen splitsing op tijd? Grijs was loos -> terug naar lijnvolgen.
  if (millis() - grijsTijd > RobotConfig::GRIJS_GELDIG_MS) {
    robot.setState(new LijnVolgenToestand(robot));
    return;
  }

  // 5) Anders: de lijn blijven volgen tot de splitsing.
  volgLijn();
}

void KruispuntToestand::exit() {}

// Lijn volgen met PD (langzamer, want we naderen een afslag).
void KruispuntToestand::volgLijn() {
  int positie = robot.getSensorData().linePosition;
  int fout    = positie - 2000;
  robot.getRijController().stuurPD(RobotConfig::SNELHEID_GRIJS, fout);
}

// Onthoudt een grijze markering aan de zijkant.
void KruispuntToestand::behandelMarkeringen() {
  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // LOCK: net grijs gelatcht? Dan GRIJS_LOCK_MS lang geen nieuwe richting
  // aannemen, zodat hij niet meteen de andere kant detecteert en flipt.
  if ((grijsLinks || grijsRechts) && millis() - grijsTijd < RobotConfig::GRIJS_LOCK_MS) {
    return;
  }

  // Links: pas onthouden na GRIJS_BEVESTIG metingen achter elkaar grijs.
  if (lijn.grijsTapeLinks()) {
    if (++grijsLinksTeller >= RobotConfig::GRIJS_BEVESTIG && !grijsLinks) {
      grijsLinks = true;
      grijsTijd  = millis();
      Serial.println(F("[GRIJS] LINKS onthouden"));
    }
  } else {
    grijsLinksTeller = 0;
  }

  // Rechts: idem.
  if (lijn.grijsTapeRechts()) {
    if (++grijsRechtsTeller >= RobotConfig::GRIJS_BEVESTIG && !grijsRechts) {
      grijsRechts = true;
      grijsTijd   = millis();
      Serial.println(F("[GRIJS] RECHTS onthouden"));
    }
  } else {
    grijsRechtsTeller = 0;
  }
}
