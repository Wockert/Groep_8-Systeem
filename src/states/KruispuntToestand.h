#pragma once

#include "../core/RobotToestand.h"

// Toestand op een kruispunt: volgt de lijn en slaat af op basis van de onthouden grijze markering.
class KruispuntToestand : public RobotToestand {
private:
  bool grijsLinks  = false;   // grijze markering links onthouden -> linksaf
  bool grijsRechts = false;   // grijze markering rechts onthouden -> rechtsaf
  int  grijsLinksTeller  = 0;  // bevestigingstellers voor de grijsdetectie
  int  grijsRechtsTeller = 0;
  float grijsCm = 0.0f;        // afstand waarop het grijs werd gezien
  int  splitsTeller = 0;       // bevestigingsteller voor de splitsing
  bool bezigMetAfslaan = false;
  int  afslagRichting  = 0;    // -1 links, +1 rechts
  unsigned long afslagTijd = 0;  // starttijd van de afslagmanoeuvre
  bool aLosgelaten = false;

  void volgLijn();             // gewone PD-koers over het kruispunt
  void behandelMarkeringen();  // grijze markeringen detecteren en onthouden

public:
  KruispuntToestand(ZumoRobot& robot, int richting = 0);
  void enter()  override;
  void update() override;
  void exit()   override;
};
