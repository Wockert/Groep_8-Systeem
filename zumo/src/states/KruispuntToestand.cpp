#include "KruispuntToestand.h"
#include "../core/ZumoRobot.h"
#include "../config/RobotConfig.h"
#include "LijnVolgenToestand.h"
#include "StartToestand.h"

// De richting komt mee uit LijnVolgenToestand (die zag het grijs het eerst).
// Zo gaat de informatie niet verloren als de markering bij de overdracht al
// gepasseerd is — herdetecteren hier was de grootste bron van gemiste afslagen.
KruispuntToestand::KruispuntToestand(ZumoRobot& robot, int richting)
: RobotToestand(robot) {
  grijsLinks  = (richting < 0);
  grijsRechts = (richting > 0);
}

void KruispuntToestand::enter() {
  grijsCm = robot.getSensorData().distanceCm;   // start van het grijs-venster (in cm)
  robot.getHardware().print("Kruispunt");
  if (grijsLinks)  Serial.println(F("[GRIJS] LINKS (van lijnvolger)"));
  if (grijsRechts) Serial.println(F("[GRIJS] RECHTS (van lijnvolger)"));
}

void KruispuntToestand::update() {
  // --- Nog een keer A = stoppen (zelfde vangnet als de lijnvolger) ---
  const SensorData& s = robot.getSensorData();
  if (!s.buttonA) {
    aLosgelaten = true;
  } else if (aLosgelaten) {
    robot.setState(new StartToestand(robot));
    return;   // oude toestand is hierna verwijderd
  }

  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // 1) Bezig met afslaan? Hard naar de gekozen tak sturen voor AFSLAG_MS.
  //    Bewust NIET via stuurPD: die clampt de correctie (boog i.p.v. pivot)
  //    en AFSLAG_MS is afgesteld op een scherpe draai. setMotorSpeeds
  //    begrenst zelf al naar -400..400.
  if (bezigMetAfslaan) {
    int positie   = robot.getSensorData().linePosition;
    int fout      = (positie - 2000) + afslagRichting * RobotConfig::SPLITS_BIAS;
    int correctie = (int)(RobotConfig::KP * fout);
    // Begrensd op de normale pivotsnelheid: ongelimiteerd satureerde dit op
    // +-400 (de hardste draai die de robot kan) terwijl alle andere pivots
    // op SNELHEID_BOCHT staan — overshoot-risico precies op de splitsing.
    correctie = constrain(correctie, -RobotConfig::SNELHEID_BOCHT,
                                      RobotConfig::SNELHEID_BOCHT);
    robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_GRIJS + correctie,
                                       RobotConfig::SNELHEID_GRIJS - correctie);
    if (millis() - afslagTijd > RobotConfig::AFSLAG_MS) {
      Serial.println(F("[AFSLAG] klaar"));
      robot.setState(new LijnVolgenToestand(robot));
    }
    return;   // (na setState is deze toestand verwijderd)
  }

  // 2) Lijn (bijna) kwijt? Zelfde drempel als de lijnvolger (gekalibreerd
  //    < 200) — de oude check (een sensor > DREMPEL_ZWART=700) vond een
  //    dunne/vale lijn al "kwijt" en gooide dan de onthouden richting weg.
  //    MET een richting betekent lijn-weg hier: we staan op de knik van de
  //    splitsing -> direct afslaan die kant op (de richting NIET verliezen).
  //    Zonder richting: terug naar de lijnvolger (die heeft het herstel).
  unsigned int maxW = 0;
  for (int i = 0; i < 5; i++)
    if ((unsigned int)s.lineValues[i] > maxW) maxW = (unsigned int)s.lineValues[i];
  if (maxW < 200) {
    if (grijsLinks || grijsRechts) {
      bezigMetAfslaan = true;
      afslagRichting  = grijsLinks ? -1 : +1;
      afslagTijd      = millis();
      Serial.println(F("[AFSLAG] start (lijn weg op de splitsing)"));
    } else {
      robot.setState(new LijnVolgenToestand(robot));
    }
    return;
  }

  // 3) Grijs onthouden/bevestigen (met lock + ontdubbeling) — voor het geval
  //    de richting nog niet bekend was, of er een tweede markering komt.
  behandelMarkeringen();

  // 4) Splitsing met een onthouden richting? Pas afslaan na SPLITS_BEVESTIG
  //    metingen OP RIJ: een enkele brede-zwart-meting (hobbel, schuine
  //    lijnrand) mag de afslag niet aftrappen.
  if (lijn.isSplitsing() && (grijsLinks || grijsRechts)) {
    if (++splitsTeller >= RobotConfig::SPLITS_BEVESTIG) {
      bezigMetAfslaan = true;
      afslagRichting  = grijsLinks ? -1 : +1;   // -1 = links, +1 = rechts
      afslagTijd      = millis();
      Serial.println(F("[AFSLAG] start"));
      return;
    }
  } else {
    splitsTeller = 0;   // reeks onderbroken -> opnieuw tellen
  }

  // 5) Geen splitsing binnen de geldige AFSTAND? Grijs was loos -> terug.
  //    (Afstand i.p.v. tijd: zo telt het venster in cm baan, los van snelheid.)
  if (s.distanceCm - grijsCm > RobotConfig::GRIJS_GELDIG_CM) {
    robot.setState(new LijnVolgenToestand(robot));
    return;
  }

  // 6) Anders: de lijn blijven volgen tot de splitsing (langzamer: na grijs
  //    is SNELHEID_GRIJS verlaagd zodat de afslag beheerst genomen wordt).
  volgLijn();
}

void KruispuntToestand::exit() {}

// Lijn volgen tijdens het naderen (langzaam): continue P-sturing, zelfde
// stijl als de hoofdlijnvolger. Bewust NIET meer via stuurPD — die heeft
// nog een dode zone + eigen D-geheugen (ander rijgevoel, bang-bang-risico).
void KruispuntToestand::volgLijn() {
  int fout      = robot.getSensorData().linePosition - 2000;
  int correctie = (int)(RobotConfig::KP * fout);
  correctie = constrain(correctie, -RobotConfig::SNELHEID_BOCHT,
                                    RobotConfig::SNELHEID_BOCHT);
  robot.getHardware().setMotorSpeeds(RobotConfig::SNELHEID_GRIJS + correctie,
                                     RobotConfig::SNELHEID_GRIJS - correctie);
}

// Onthoudt een grijze markering aan de zijkant.
void KruispuntToestand::behandelMarkeringen() {
  LineSensorAnalyse& lijn = robot.getLijnAnalyse();

  // LOCK: net grijs gelatcht? Dan GRIJS_LOCK_CM lang geen nieuwe richting
  // aannemen, zodat hij niet meteen de andere kant detecteert en flipt.
  if ((grijsLinks || grijsRechts) &&
      robot.getSensorData().distanceCm - grijsCm < RobotConfig::GRIJS_LOCK_CM) {
    return;
  }

  // Links: pas onthouden na GRIJS_BEVESTIG metingen achter elkaar grijs.
  if (lijn.grijsTapeLinks()) {
    if (++grijsLinksTeller >= RobotConfig::GRIJS_BEVESTIG && !grijsLinks) {
      grijsLinks = true;
      grijsCm    = robot.getSensorData().distanceCm;
      Serial.println(F("[GRIJS] LINKS onthouden"));
    }
  } else {
    grijsLinksTeller = 0;
  }

  // Rechts: idem.
  if (lijn.grijsTapeRechts()) {
    if (++grijsRechtsTeller >= RobotConfig::GRIJS_BEVESTIG && !grijsRechts) {
      grijsRechts = true;
      grijsCm     = robot.getSensorData().distanceCm;
      Serial.println(F("[GRIJS] RECHTS onthouden"));
    }
  } else {
    grijsRechtsTeller = 0;
  }
}
