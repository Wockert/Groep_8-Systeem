# Zumo Lijnvolger — Eindontwerp

Software voor een **Pololu Zumo32U4** robot die volledig autonoom een lijnparcours
aflegt, onderweg hindernissen herkent en als eindopdracht het eindblok uit de cirkel
duwt. De code is opgebouwd volgens een **state machine** met losse serviceklassen,
zodat elke klasse klein, begrijpelijk en onderhoudbaar blijft.

---

## 1. Doel van het project

Een robot een parcours laten afleggen om te oefenen met het programmeren van embedded
systemen. Van een professional wordt verwacht dat de software **systematisch,
gestructureerd en goed ontworpen** is, zodat taken verdeeld kunnen worden en de code
onderhoudbaar blijft. Alle ingeleverde code moet je zelf volledig kunnen verklaren en
moet passen binnen je eigen ontwerp.

## 2. Het doel: de wedstrijd

> De Zumo legt de route **zo snel mogelijk** af en duwt het **eindblok uit de ring**.

| Regel | Waarde |
|-------|--------|
| Winnaar | snelste tijd over de complete route |
| Pogingen | 3 per Zumo (inclusief ongeldige) |
| Tijdslimiet | maximaal 5 minuten |
| Besturing | volledig autonoom (anders diskwalificatie) |
| Parcours | rechthoek 300 × 150 cm, witte ondergrond |
| Lijn | zwarte lijn, 1,5 – 2,5 cm breed |

## 3. Het parcours en de hindernissen

Het parcours bevat een reeks situaties die de robot moet herkennen en correct afhandelen:

- **Startlijn** — een zwarte lijn loodrecht op de route, die de route kruist; markeert de start.
- **Zwarte lijn** — gewoon volgen, zo snel mogelijk.
- **Kruispunten** — kunnen voorkomen op de route.
- **Hiaten** — onderbrekingen in de lijn van maximaal 5 cm.
- **Groene lijn** — wit oppervlak met groene lijn; hier mag **maximaal halve snelheid** gereden worden.
- **Grijze richtingslijn (één kant)** — bij de eerstvolgende kruising of t-splitsing de route in díe richting vervolgen.
- **Grijze richtingslijn (links én rechts)** — de wip/helling; de Zumo **pauzeert tot de pitch onder de 0 graden** komt.
- **Bruine lijn** — de Zumo rijdt **20 cm vooruit** en gaat dan het **eindblok** zoeken om het uit de cirkel te duwen.

## 4. Hoe het ontwerp het doel oplost

Elke regel uit het reglement is terug te vinden in een toestand of serviceklasse. Dít is
de reden dat het ontwerp is opgesplitst zoals hieronder:

| Wedstrijdregel / hindernis | Toestand | Ondersteunende klasse(n) / config |
|----------------------------|----------|-----------------------------------|
| Start bij zwarte startlijn | `StartToestand` | `LineSensorAnalyse.startLijnGezien()` |
| Zwarte lijn volgen, zo snel mogelijk | `LijnVolgenToestand` | `RijController.stuurPD()`, `KP`, `KD`, `SNELHEID_MAX` |
| Kruispunten afhandelen | `LijnVolgenToestand` | `behandelKruising()` |
| Hiaten ≤ 5 cm (lijn even kwijt) | `LijnVolgenToestand` | `LijnKwijt()`, `laatsteLijnGezien`, `GEHEUGEN_MS` |
| Groene lijn → halve snelheid | `GroeneLijnToestand` | `LineSensorAnalyse.isGroeneLijn()`, `SNELHEID_GROEN` |
| Grijze richtingslijn links/rechts | `LijnVolgenToestand` | `grijsTapeLinks()`, `grijsTapeRechts()`, `behandelMarkeringen()`, `grijsLinksGezien`, `grijsRechtsGezien` |
| Grijs links **én** rechts → wachten tot pitch < 0° (wip) | `WipOmhoogToestand` → `WipWachtToestand` → `WipOmlaagToestand` | `grijsTapeBeiden()`, `PitchDetector.isOmhoog/isVlak/isOmlaag()` |
| Bruine lijn → 20 cm vooruit, dan zoeken | `BlokZoekToestand` | `LineSensorAnalyse.isBruineLijn()`, `DOORRIJDEN_CM`, `CM_PER_PULSE` |
| Eindblok uit de cirkel duwen | `BlokZoekToestand` | `BlokZoeker.scanRondom()`, `draaiNaarBlok()`, `duwTotRand()`, `SNELHEID_ZOEKEN` |
| Klaar — robot stoppen | `StopToestand` | `RijController.stop()`, `ZumoHardware.playDoneSound()` |

## 5. Projectstructuur

```
zumo/
├── zumo.ino                      ← entry point: maakt ZumoRobot, delegeert setup()/loop()
├── README.md                     ← dit document
└── src/
    ├── core/
    │   ├── ZumoRobot.h / .cpp     ← coordinator
    │   └── RobotToestand.h        ← abstracte basisklasse (enter/update/exit)
    ├── config/
    │   ├── RobotConfig.h          ← alle instellingen en drempelwaarden (utility)
    │   └── SensorData.h           ← momentopname van alle sensorwaarden (snapshot)
    ├── hardware/
    │   └── ZumoHardware.h / .cpp  ← facade over de Zumo32U4-bibliotheek
    ├── sensors/
    │   ├── LineSensorAnalyse.h / .cpp   ← lijnsensor → betekenis
    │   └── PitchDetector.h / .cpp       ← IMU → omhoog/vlak/omlaag
    ├── control/
    │   ├── RijController.h / .cpp       ← motorsturing + PD-regeling
    │   └── BlokZoeker.h / .cpp          ← eindblok zoeken en duwen
    └── states/
        ├── StartToestand.h / .cpp
        ├── LijnVolgenToestand.h / .cpp
        ├── GroeneLijnToestand.h / .cpp
        ├── WipOmhoogToestand.h / .cpp
        ├── WipWachtToestand.h / .cpp
        ├── WipOmlaagToestand.h / .cpp
        ├── BlokZoekToestand.h / .cpp
        └── StopToestand.h / .cpp
```

> **Waarom alles onder `src/`?** De Arduino-build compileert automatisch alle bestanden
> die náást de `.ino` staan, maar **negeert** willekeurige submappen daar. De enige
> uitzondering is een map met de naam **`src/`**: die wordt **recursief** meegecompileerd.
> Daarom staan alle klassen onder `src/`, netjes gegroepeerd per verantwoordelijkheid.

## 6. Architectuur

De software werkt volgens het **state-machine-principe**:

- **`ZumoRobot`** is de coördinator. Hij doet zelf het werk niet, maar houdt de actieve
  toestand bij en roept elke ronde `update()` aan op die toestand. Schakelen gebeurt via
  `setState()`.
- **`RobotToestand`** is de abstracte basisklasse. Elke toestand heeft dezelfde drie
  methodes — `enter()` (bij activeren), `update()` (elke ronde), `exit()` (bij verlaten) —
  zodat `ZumoRobot` niet per toestand hoeft te weten wat hij moet aanroepen.
- De **serviceklassen** (`ZumoHardware`, `LineSensorAnalyse`, `PitchDetector`,
  `RijController`, `BlokZoeker`) doen het echte werk en worden door de toestanden gebruikt.

**Datastroom per ronde:**

```
hardware  →  SensorData (snapshot)  →  actieve toestand beslist  →  RijController / hardware voert uit
```

Doordat alle sensorwaarden eerst in één `SensorData`-momentopname worden gezet, ziet elke
klasse in die ronde exact dezelfde meting en ontstaan er geen tegenstrijdige beslissingen.

## 7. Klassenoverzicht

| Klasse | Map | Verantwoordelijkheid | Waarom apart |
|--------|-----|----------------------|--------------|
| `ZumoRobot` | core | Stuurt alles aan, houdt de actieve toestand bij | Anders wordt één klasse honderden regels lang |
| `RobotToestand` | core | Abstracte basis voor alle toestanden | Eén vaste interface (enter/update/exit) voor de robot |
| `RobotConfig` | config | Alle snelheden, kleurdrempels en PID-waarden | Tijdens testen pas je deze tientallen keren aan — één plek |
| `SensorData` | config | Momentopname van alle sensorwaarden | Iedereen ziet dezelfde meting in dezelfde ronde |
| `ZumoHardware` | hardware | Facade voor motoren, sensoren, LCD, IMU | Rest van de code praat niet direct met de hardware |
| `LineSensorAnalyse` | sensors | Bepaalt: groen, bruin, grijs of niets | Drempelwaarden op één plek i.p.v. in elke toestand |
| `PitchDetector` | sensors | IMU → `isOmhoog/isVlak/isOmlaag()` | Toestanden hoeven ruwe pitch-waarden niet te interpreteren |
| `RijController` | control | Motorsnelheden + PD-regeling | PD-logica niet in elke toestand dupliceren |
| `BlokZoeker` | control | Eindblok zoeken en uit de cirkel duwen | Aparte, complexe eindopdracht |

## 8. De toestanden

| Toestand | Situatie |
|----------|----------|
| `StartToestand` | Begin van de rit; wacht tot de robot mag starten |
| `LijnVolgenToestand` | Hoofdtoestand; volgt de lijn met PD-sturing, behandelt kruisingen en markeringen |
| `GroeneLijnToestand` | Groene lijn gedetecteerd; rijdt op halve snelheid |
| `WipOmhoogToestand` | Rijdt de wip/helling omhoog |
| `WipWachtToestand` | Wacht op de wip tot de pitch onder 0° komt |
| `WipOmlaagToestand` | Rijdt de wip/helling naar beneden |
| `BlokZoekToestand` | Na de bruine lijn + 20 cm: zoekt het eindblok |
| `StopToestand` | Robot stopt; motoren uit |

## 9. Ontwerpprincipes

1. **Eén verantwoordelijkheid per klasse** — elke klasse doet één ding, zodat de code
   klein en overzichtelijk blijft en je weet waar je iets moet aanpassen.
2. **`RobotConfig` als één plek voor instellingen** — snelheden en drempelwaarden staan
   niet verspreid door de code, dus aanpassen tijdens het testen kost weinig tijd.
3. **`SensorData` als momentopname** — alle klassen werken met dezelfde meting per ronde,
   wat tegenstrijdige beslissingen voorkomt.

## 10. Bouwen & gebruiken

1. Open de map `zumo/` in de **Arduino IDE** (de map en `zumo.ino` moeten dezelfde naam hebben).
2. Installeer de **`Zumo32U4`-bibliotheek** (via Library Manager) — `ZumoHardware` bouwt hierop.
3. Stel de placeholder-waarden in **`src/config/RobotConfig.h`** af (snelheden, kleurdrempels, `KP`/`KD`) tijdens het testen.
4. Compileer en upload naar de Zumo32U4.

## 11. Status

Dit is op dit moment de **structuur (skeleton)** van het project:

- Alle klassen, attributen en methode-signaturen uit het UML-ontwerp staan vast.
- Methode-bodies zijn nog **leeg** (de code compileert, maar bevat nog geen gedrag).
- De toestandsovergangen en de logica per toestand worden nog ingevuld.
