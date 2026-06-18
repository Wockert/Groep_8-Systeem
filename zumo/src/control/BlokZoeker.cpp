#include "BlokZoeker.h"
#include "../hardware/ZumoHardware.h"
#include "../config/BlokZoekConfig.h"

void BlokZoeker::resetAanval() {
  besteRichting = 0;
  besteWaarde = 0;

  aanvalActief = false;
  klaar = false;

  aanvalStartTijd = 0;
  laatsteBlokGezienTijd = 0;
  negeerZwarteRandTot = 0;

  laatsteVerschil = 0;
}

bool BlokZoeker::isKlaar() const {
  return klaar;
}

bool BlokZoeker::zoekEnDuwBlok(ZumoHardware& hardware) {
  if (klaar) {
    hardware.stopMotors();
    return true;
  }

  hardware.readProximity();

  uint8_t links  = hardware.getProxLeft();
  uint8_t rechts = hardware.getProxRight();

  bool blokGezien =
    links >= BlokZoekConfig::DREMPEL_BLOK ||
    rechts >= BlokZoekConfig::DREMPEL_BLOK;

  if (blokGezien) {
    if (!aanvalActief) {
      aanvalActief = true;
      aanvalStartTijd = millis();
      laatsteBlokGezienTijd = millis();

      negeerZwarteRandTot =
        millis() + BlokZoekConfig::MIN_AANVAL_TIJD_VOOR_RAND_MS;
    }

    laatsteBlokGezienTijd = millis();
    laatsteVerschil = (int)links - (int)rechts;

    rijMaxNaarBlok(hardware, links, rechts);
    return false;
  }

  if (aanvalActief) {
    rijDoorMetLaatsteCorrectie(hardware);
    return klaar;
  }

  scanRondom(hardware);

  return klaar;
}

void BlokZoeker::rijMaxNaarBlok(
  ZumoHardware& hardware,
  uint8_t linksSensor,
  uint8_t rechtsSensor
) {
  bool magStoppenOpRand =
    aanvalActief &&
    millis() - aanvalStartTijd >= BlokZoekConfig::MIN_AANVAL_TIJD_VOOR_RAND_MS;

  if (magStoppenOpRand && zwarteRandGezien(hardware)) {
    stopNaUitduwen(hardware);
    aanvalActief = false;
    return;
  }

  int som = (int)linksSensor + (int)rechtsSensor;
  int verschil = (int)linksSensor - (int)rechtsSensor;

  laatsteVerschil = verschil;

  int basisSnelheid;

  if (som >= 8) {
    basisSnelheid = BlokZoekConfig::SNELHEID_NAAR_BLOK_HOOG;
  }
  else if (som >= 5) {
    basisSnelheid = BlokZoekConfig::SNELHEID_NAAR_BLOK_MID;
  }
  else {
    basisSnelheid = BlokZoekConfig::SNELHEID_NAAR_BLOK_LAAG;
  }

  int correctie = verschil * BlokZoekConfig::BLOK_STUUR_CORRECTIE;

  correctie = constrain(
    correctie,
    -BlokZoekConfig::BLOK_MAX_CORRECTIE,
     BlokZoekConfig::BLOK_MAX_CORRECTIE
  );

  int linksMotor  = basisSnelheid - correctie;
  int rechtsMotor = basisSnelheid + correctie;

  linksMotor = constrain(
    linksMotor,
    BlokZoekConfig::BLOK_MIN_MOTORSNELHEID,
    BlokZoekConfig::SNELHEID_MAX
  );

  rechtsMotor = constrain(
    rechtsMotor,
    BlokZoekConfig::BLOK_MIN_MOTORSNELHEID,
    BlokZoekConfig::SNELHEID_MAX
  );

  hardware.setMotorSpeeds(linksMotor, rechtsMotor);
}

void BlokZoeker::rijDoorMetLaatsteCorrectie(ZumoHardware& hardware) {
  bool magStoppenOpRand =
    aanvalActief &&
    millis() - aanvalStartTijd >= BlokZoekConfig::MIN_AANVAL_TIJD_VOOR_RAND_MS;

  if (magStoppenOpRand && zwarteRandGezien(hardware)) {
    stopNaUitduwen(hardware);
    aanvalActief = false;
    return;
  }

  int correctie =
    laatsteVerschil * BlokZoekConfig::BLOK_STUUR_CORRECTIE;

  correctie = constrain(
    correctie,
    -BlokZoekConfig::LAATSTE_CORRECTIE_MAX,
     BlokZoekConfig::LAATSTE_CORRECTIE_MAX
  );

  int linksMotor =
    BlokZoekConfig::SNELHEID_AANVAL_VERLOREN - correctie;

  int rechtsMotor =
    BlokZoekConfig::SNELHEID_AANVAL_VERLOREN + correctie;

  linksMotor = constrain(
    linksMotor,
    BlokZoekConfig::BLOK_MIN_MOTORSNELHEID,
    BlokZoekConfig::SNELHEID_MAX
  );

  rechtsMotor = constrain(
    rechtsMotor,
    BlokZoekConfig::BLOK_MIN_MOTORSNELHEID,
    BlokZoekConfig::SNELHEID_MAX
  );

  hardware.setMotorSpeeds(linksMotor, rechtsMotor);
}

int BlokZoeker::scanRondom(ZumoHardware& hardware) {
  besteWaarde = 0;
  besteRichting = 0;

  unsigned long startTijd = millis();

  while (millis() - startTijd < BlokZoekConfig::SCAN_ROND_MS) {
    if (!aanvalActief && zwarteRandGezien(hardware)) {
      keerOmBijRand(hardware);
      return besteRichting;
    }

    hardware.setMotorSpeeds(
      BlokZoekConfig::SNELHEID_ZOEKEN,
      -BlokZoekConfig::SNELHEID_ZOEKEN
    );

    hardware.readProximity();

    uint8_t links  = hardware.getProxLeft();
    uint8_t rechts = hardware.getProxRight();

    int waarde = links + rechts;

    if (waarde > besteWaarde) {
      besteWaarde = waarde;

      if (links > rechts) {
        besteRichting = -1;
      } else {
        besteRichting = 1;
      }
    }

    if (
      links >= BlokZoekConfig::DREMPEL_BLOK ||
      rechts >= BlokZoekConfig::DREMPEL_BLOK
    ) {
      return besteRichting;
    }
  }

  startTijd = millis();

  while (millis() - startTijd < BlokZoekConfig::VERPLAATS_ZOEK_MS) {
    if (!aanvalActief && zwarteRandGezien(hardware)) {
      keerOmBijRand(hardware);
      return besteRichting;
    }

    hardware.setMotorSpeeds(190, 140);
  }

  return besteRichting;
}

bool BlokZoeker::zwarteRandGezien(ZumoHardware& hardware) {
  if (millis() < negeerZwarteRandTot) {
    return false;
  }

  unsigned int sensorWaarden[5];

  hardware.readRawLine(sensorWaarden);

  for (int i = 0; i < 5; i++) {
    if (sensorWaarden[i] >= BlokZoekConfig::DREMPEL_ZWART_L) {
      return true;
    }
  }

  return false;
}

void BlokZoeker::keerOmBijRand(ZumoHardware& hardware) {
  hardware.setMotorSpeeds(-210, -210);
  delay(220);

  hardware.setMotorSpeeds(210, -210);
  delay(300);

  hardware.stopMotors();
}

void BlokZoeker::stopNaUitduwen(ZumoHardware& hardware) {
  hardware.setMotorSpeeds(
    BlokZoekConfig::SNELHEID_DUWEN,
    BlokZoekConfig::SNELHEID_DUWEN
  );

  delay(BlokZoekConfig::DOORDUWEN_NA_RAND_MS);

  hardware.stopMotors();

  klaar = true;
}
