// =============================================================
//  RobotConfig  <<utility>>
//  Alle instellingen op EEN plek: snelheden, kleurdrempels,
//  PID-waarden. Tijdens het testen pas je deze tientallen keren
//  aan; door ze hier te bundelen hoef je niet door de code te zoeken.
//  (Waarden zijn placeholders - afstellen tijdens het testen.)
// =============================================================
#pragma once

class RobotConfig {
public:
  // --- Snelheden (0-400) — afstellen tijdens testen ---
  static constexpr int SNELHEID_MAX      = 400;
  static constexpr int SNELHEID_NORMAAL  = 300;   // basissnelheid op zwart (zet hoger voor vol gas)
  static constexpr int SNELHEID_GRIJS    = 300;   // langzamer zodra een grijs-richting onthouden is
  static constexpr int SNELHEID_GROEN    = 100;
  static constexpr int SNELHEID_WIP      = 120;
  static constexpr int SNELHEID_ZOEKEN   = 150;
  static constexpr int SNELHEID_BOCHT    = 300;   // pivotsnelheid bij afslaan op een kruispunt
  static constexpr int KRUIS_MS          = 350;   // hoe lang pivoten op een kruispunt (ms)

  // --- Kleurdrempels op de RAUWE lijn= waarden (~0=wit .. ~2000=zwart) ---
  // De positie/lijnvolgen gebruikt readLine (gekalibreerd); grijs/kruispunt
  // gebruiken de RAUWE waarden, want kalibratie poetst grijs weg.
  // !! Afstellen via 'lijn=' in de Serial Monitor.
  static constexpr int DREMPEL_ZWART     = 1000;  // RAUW: > dit = zwart
  static constexpr int DREMPEL_GRIJS_L   = 300;   // RAUW grijs-band — !! meet grijs en stel bij
  static constexpr int DREMPEL_GRIJS_H   = 700;   // (700..1000 = dode overgangszone, telt niet als grijs)
  static constexpr int GRIJS_BEVESTIG    = 1;     // grijs onthouden op de eerste goede meting (snel)
  static constexpr int DREMPEL_CONTRAST  = 200;   // (ongebruikt; lijnvolgen draait op readLine)

  // Grijs-detectie RELATIEF t.o.v. het witniveau (drift-vast: rauwe waarden
  // schuiven met licht/accu). Grijs = zoveel BOVEN de laagste sensor:
  static constexpr int GRIJS_DELTA_L   = 150;   // duidelijk boven wit (lager = grijs eerder gepakt)
  static constexpr int GRIJS_DELTA_H   = 900;   // maar geen (bijna-)zwart
  static constexpr int GRIJS_RECHT_MAX = 600;   // (ongebruikt: straight-gate vervangen door binnenbuur-filter)

  // --- Afslaan op een splitsing (grijs = richtingshint) ---
  static constexpr int DREMPEL_SPLITS_MIN = 2;    // >= zoveel sensoren zwart + buitenrand zwart = splitsing
  static constexpr int SPLITS_BIAS        = 2000; // hoe hard naar de gekozen tak duwen (fout-schaal +-2000)
  static constexpr unsigned long GRIJS_GELDIG_MS = 1500; // onthouden richting vervalt na deze tijd
  static constexpr unsigned long GRIJS_LOCK_MS   = 500;  // na een grijs-detectie zo lang GEEN nieuwe richting aannemen
  static constexpr unsigned long AFSLAG_MS       = 350;  // VASTE duur van de afslag-bias (langer = scherper afslaan)
  static constexpr int DREMPEL_GROEN_L   = 0;
  static constexpr int DREMPEL_GROEN_H   = 0;
  static constexpr int DREMPEL_BRUIN_L   = 0;
  static constexpr int DREMPEL_BRUIN_H   = 0;

  // --- Timing & afstand ---
  static constexpr int   GEHEUGEN_MS     = 0;
  static constexpr int   DOORRIJDEN_CM   = 0;
  // Door het team gemeten en getest: 0.019 cm per encodertick/pulse.
  // (afstand = gemiddelde van links+rechts ticks * deze waarde)
  static constexpr float CM_PER_PULSE    = 0.019f;

  // --- PD-regeling (fout = lijnpositie - 2000, bereik +-2000) — branch-waarden ---
  // KP lager = rustiger sturen; KD hoger = meer demping (minder slingeren).
  static constexpr float KP = 0.40f;
  static constexpr float KD = 2.0f;
  static constexpr int   LIJN_DODEZONE = 200;  // binnen +-deze fout: rechtdoor (niet bijsturen)
};
