// =============================================================
//  KruispuntToestand
//  Neemt het over zodra LijnVolgenToestand grijs ziet en krijgt
//  de gedetecteerde richting DIRECT mee via de constructor (de
//  markering is bij de overdracht vaak al gepasseerd, dus zelf
//  herdetecteren is onbetrouwbaar). Rijdt langzamer (SNELHEID_GRIJS)
//  naar de splitsing, slaat dan scherp af (pivot-bias voor
//  AFSLAG_MS) en geeft daarna terug aan LijnVolgenToestand.
//  Raakt hij onderweg de lijn kwijt, dan ook terug naar
//  LijnVolgenToestand — die heeft het herstel (pivot/stippellijn).
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class KruispuntToestand : public RobotToestand {
private:
  bool grijsLinks  = false;        // onthouden richting
  bool grijsRechts = false;
  int  grijsLinksTeller  = 0;      // ontdubbeling
  int  grijsRechtsTeller = 0;
  float grijsCm = 0.0f;            // afgelegde weg bij start grijs-venster (lock + vervaltermijn)
  int  splitsTeller = 0;           // metingen op rij splitsing gezien (debounce afslag)
  bool bezigMetAfslaan = false;    // bezig de tak op te sturen?
  int  afslagRichting  = 0;        // -1 = links, +1 = rechts
  unsigned long afslagTijd = 0;    // start van de afslag
  long afslagTicksL = 0;           // encoderstand bij start afslag (draaihoek-meting:
  long afslagTicksR = 0;           // afslag pas klaar na een minimale gedraaide hoek)
  bool aLosgelaten = false;        // A moet eerst los voordat hij als stopknop telt

  void volgLijn();
  void behandelMarkeringen();      // onthoudt grijs links/rechts (lock + ontdubbeling)

public:
  // richting: -1 = links onthouden, +1 = rechts, 0 = nog onbekend
  KruispuntToestand(ZumoRobot& robot, int richting = 0);
  void enter()  override;
  void update() override;
  void exit()   override;
};
