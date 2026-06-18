// =============================================================
//  GroeneLijnToestand  (module uit main)
//  Volgt de GROENE lijn met exact dezelfde regel-logica als de
//  zwarte lijnvolger: PID (P + lekkende I + gefilterde D),
//  progressief sturen en geleidelijke adaptieve snelheid — alleen
//  met SNELHEID_GROEN als maximum. readLine werkt ook op groen:
//  de gekalibreerde groenwaarden (~200..350) zijn genoeg voor de
//  gewogen positieberekening. Geen scherpe-bocht-pivots: het
//  groene stuk heeft geen 90-graden-knikken. Ziet hij
//  GROEN_BEVESTIG rondes op rij geen groen meer, dan terug naar
//  de zwarte lijnvolger.
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class GroeneLijnToestand : public RobotToestand {
private:
  int  vorigeFout  = 0;
  int  dGefilterd  = 0;   // gladgestreken afgeleide (ruisfilter op de D-term)
  long foutSom     = 0;   // opgetelde fout voor de I-term
  int  basis       = 0;   // huidige basissnelheid (verandert geleidelijk)
  int  zwartTeller = 0;   // rondes op rij niet-groen (debounce voor de exit)
  bool aLosgelaten = false;   // A moet eerst los voordat hij als stopknop telt

public:
  GroeneLijnToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
