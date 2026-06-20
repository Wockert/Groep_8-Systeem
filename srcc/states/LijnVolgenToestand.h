#pragma once

#include "../core/RobotToestand.h"

// Hoofdtoestand: volgt de zwarte lijn met PID en handelt bochten, gaten en checkpoints af.
class LijnVolgenToestand : public RobotToestand {
private:
  unsigned int sensorWaarden[5] = {0};
  // PID-toestand voor het lijnvolgen
  int  vorigeFout  = 0;     // fout van vorige stap (D-term)
  int  dGefilterd  = 0;     // gefilterde afgeleide
  long foutSom     = 0;     // opgetelde fout (I-term)
  int  basis       = 0;     // huidige basissnelheid (loopt geleidelijk op/af)
  bool aLosgelaten = false; // debounce knop A (terug naar start)
  // Bocht-afhandeling
  bool inBocht     = false; // bezig met een pivot-bocht
  int  bochtKant   = 0;     // -1 = links, +1 = rechts
  int  randTeller  = 0;     // bevestigingsteller voor zwarte rand
  int  grijsTeller = 0;     // bevestigingsteller grijze markering
  int  groenTeller = 0;     // bevestigingsteller groene lijn
  int  pitchTeller = 0;     // bevestigingsteller helling (wip/omhoog)
  float randZwartCm = -1000.0f;  // afstand waar een aankomende bocht werd aangekondigd
  int  randZwartKant = 0;
  long bochtTicksL = 0;     // encoderstand bij start bocht (voor het meten van de draaihoek)
  long bochtTicksR = 0;
  float naBochtCm     = -1000.0f;  // afstand vlak na een bocht (correctie tijdelijk beperken)
  float zwartZijCm  = -1000.0f;    // laatst gezien zwart aan een buitensensor
  int  zwartZijKant = 0;
  // Overbruggen van een gat in de lijn (stippellijn)
  bool  inStippelZone     = false;
  float stippelZoneStartCm = 0.0f;
  float stippelGatCm       = 0.0f;

public:
  LijnVolgenToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
