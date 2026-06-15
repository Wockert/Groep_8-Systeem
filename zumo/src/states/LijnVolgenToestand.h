// =============================================================
//  LijnVolgenToestand
//  Volgt de zwarte lijn met PD-sturing (logica uit branch
//  Lijnsensorvolgen-Verbeteren). Voor bochten van 90 graden en
//  scherper: ziet een buitenrand-sensor de lijn afbuigen, dan
//  draait hij die kant op tot de lijn weer netjes in het midden
//  ligt — anders schiet hij de hoek voorbij en keert hij om.
//  Nogmaals A = stoppen (terug naar StartToestand).
// =============================================================
#pragma once

#include "../core/RobotToestand.h"

class LijnVolgenToestand : public RobotToestand {
private:
  unsigned int sensorWaarden[5] = {0};  // gekalibreerde meting van readLine (0..1000 per sensor)
  int  vorigeFout  = 0;                 // voor de D-term (verandering van de fout)
  int  dGefilterd  = 0;                 // gladgestreken afgeleide (ruisfilter op de D-term)
  long foutSom     = 0;                 // opgetelde fout voor de I-term (op-het-midden-trekker)
  int  basis       = 0;                 // huidige basissnelheid (verandert geleidelijk)
  bool aLosgelaten = false;             // A moet eerst los voordat hij als stopknop telt
  bool cLosgelaten = false;             // C moet eerst los voordat een nieuwe druk een checkpoint overslaat
  bool inBocht     = false;             // bezig een scherpe bocht uit te draaien?
  int  bochtKant   = 0;                 // -1 = links draaien, +1 = rechts draaien
  int  randTeller  = 0;                 // hoeveel rondes op rij een rand zwart ziet (anti-ruis)
  int  grijsTeller = 0;                 // hoeveel rondes op rij grijs gezien (debounce overdracht)
  int  groenTeller = 0;                 // hoeveel rondes op rij groen gezien (debounce overdracht)
  int  pitchTeller = 0;                 // hoeveel rondes op rij neus omhoog/omlaag (helling/wip)
  float randZwartCm = -1000.0f;         // afgelegde-weg-stand toen een buitensensor zwart zag (-1000 = nooit)
  int  randZwartKant = 0;               // welke kant dat was: -1 = links, +1 = rechts
  long bochtTicksL = 0;                 // encoderstanden bij de start van een pivot,
  long bochtTicksR = 0;                 // voor de draaihoek-bewaking (odometrie)
  float naBochtCm     = -1000.0f;       // afgelegde weg bij de laatste pivot-exit (cooldown)
  float zwartZijCm  = -1000.0f;         // afgelegde weg toen de lijn duidelijk opzij lag (-1000 = nooit)
  int  zwartZijKant = 0;                // aan welke kant dat was: -1 = links, +1 = rechts
  bool  grijsVorigeRonde  = false;      // DEBUG: zag de vorige ronde al grijs? (print alleen op de overgang)
  bool  inStippelZone     = false;      // bezig een stippellijn (meerdere streepjes) te overbruggen?
  float stippelZoneStartCm = 0.0f;      // begin van de zone (vangnet: max lengte)
  float stippelGatCm       = 0.0f;      // afstand bij het laatste gat (einde-detectie: lang aaneengesloten zwart)

public:
  LijnVolgenToestand(ZumoRobot& robot);
  void enter()  override;
  void update() override;
  void exit()   override;
};
