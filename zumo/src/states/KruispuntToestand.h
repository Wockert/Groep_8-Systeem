// =============================================================
//  KruispuntToestand
//  Neemt het over zodra LijnVolgenToestand grijs ziet. Onthoudt de
//  grijs-richting (met ontdubbeling + lock), blijft de lijn volgen
//  tot de splitsing, slaat dan af (PD-bias voor AFSLAG_MS) en
//  schakelt daarna terug naar LijnVolgenToestand. Alle grijs- en
//  afslag-logica zit hier bij elkaar.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class KruispuntToestand : public RobotToestand {
private:
  bool grijsLinks  = false;        // onthouden richting
  bool grijsRechts = false;
  int  grijsLinksTeller  = 0;      // ontdubbeling
  int  grijsRechtsTeller = 0;
  unsigned long grijsTijd  = 0;    // start grijs-venster (lock + vervaltermijn)
  bool bezigMetAfslaan = false;    // bezig de tak op te sturen?
  int  afslagRichting  = 0;        // -1 = links, +1 = rechts
  unsigned long afslagTijd = 0;    // start van de afslag

  void volgLijn();
  void behandelMarkeringen();      // onthoudt grijs links/rechts (lock + ontdubbeling)

public:
  KruispuntToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
