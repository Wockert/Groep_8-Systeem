# Onderzoek — Zumo32U4 Lijnvolger (implementatiegids)

Verified technical research for filling in the empty method bodies of this project.
Produced by a multi-source, adversarially fact-checked deep-research pass
(21 sources fetched, 97 claims extracted, 25 verified — **25 confirmed, 0 refuted**;
all API/physics claims backed by Pololu primary sources).

> **How to read this document.** Every section maps onto a class/state in `src/` and to
> the constants in [`RobotConfig.h`](src/config/RobotConfig.h). Confidence is flagged per claim:
>
> - ✅ **FIRM** — confirmed verbatim in Pololu docs / library source / official examples.
> - 🟡 **STANDARD** — accepted practice, but not vendor-specified; tune on the robot.
> - 🔴 **MEASURE ON-TRACK** — no published value exists; you *must* calibrate it yourself.

---

## 0. TL;DR — the one thing that decides this project

A competition-ready Zumo32U4 line follower is **fully implementable** with the official
Pololu library. Almost everything (PD line following, motors, encoders, IMU pitch, the
block push) has a directly-copyable starting point from an official example.

**The single load-bearing risk is colour discrimination.** There is **no RGB/colour sensor**
on the robot. Black / white / **green / brown / grey** must all be told apart from the
**five down-facing IR-reflectance line sensors**. Pololu frames these as "light vs dark"
detectors and publishes **zero** reflectance values for coloured surfaces. The colour is
physically *attemptable* (the sensors output a continuous value, not a 1-bit black/white),
but the threshold bands (`DREMPEL_GROEN/BRUIN/GRIJS_*`) can only be pinned down by measuring
the **actual contest swatches, under contest lighting, at the real sensor ride-height**.
Budget the most test time here. 🔴

There is also a **structural hardware conflict** you must decide on early — see §1.

---

## 1. ⚠️ STRUCTURAL FINDING — 5 line sensors vs. the side proximity sensors (READ FIRST)

✅ **FIRM.** On the Zumo32U4 front sensor array, the **inner line sensors DN2 and DN4 share
pins 20 (A2) and 4 with the LEFT/RIGHT proximity sensors** via solderable jumpers. An
**assembled robot ships configured for three down-facing line sensors** (DN1/DN3/DN5), with
the jumpers selecting the **proximity** sensors on those pins.

Consequence: **you cannot have all 5 line sensors AND the left/right proximity sensors at the
same time** on a stock robot. `Zumo32U4LineSensors::initFiveSensors()` needs the jumpers set
to line-sensor mode, which disconnects LFT/RGT proximity.

This collides with the current skeleton, which assumes **both** `lineValues[5]` (5 sensors,
used for colour voting) **and** proximity-based block search. Decide one of:

| Option | Line sensors | Proximity | Trade-off |
|--------|-------------|-----------|-----------|
| **A — 3 + 3** (stock default) | DN1/DN3/DN5 (`initThreeSensors()`) | full 3-way | Fewer sensors for colour voting; `readLine()` → 0–2000, centre 1000 |
| **B — 5 + front-only** | DN1–DN5 (`initFiveSensors()`) | FRONT only | Best colour voting; block search has no left/right bearing |
| **C — rewire** | 5 | 3 | Most capable, but modifying the robot may be **against the rules** (no Zumo changes) |

> **Design impact (not changing code per your instruction — just flagging):** if you pick
> Option A, `SensorData.lineValues[5]` and `LineSensorAnalyse.sensorWaarden[5]` become `[3]`,
> and `readLine()` returns **0–2000 (centre = 1000)**, so the PD `error = position - 1000`.
> If you pick Option B, the `BlokZoeker` bearing logic loses left/right and must scan by
> rotating the whole robot. **This choice changes the maths in several states — settle it first.**

Sources: [LineSensors class](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_line_sensors.html),
[LineSensorTest.ino](https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/LineSensorTest/LineSensorTest.ino),
[User's guide 0J63/3.5](https://www.pololu.com/docs/0J63/3.5)

---

## 2. Zumo32U4 library API map → `ZumoHardware` (facade)

✅ **FIRM.** The official library covers every subsystem the contest needs. `ZumoHardware`
should wrap exactly these:

| Library class | Use in this project | Key methods |
|---------------|--------------------|-------------|
| `Zumo32U4LineSensors` | line + colour | `initThreeSensors()` / `initFiveSensors()`, `calibrate()`, `readCalibrated(v)`, `readLine(v)`, `emittersOn/Off()`, `resetCalibration()` |
| `Zumo32U4Motors` | drive | `setSpeeds(left, right)` (static) |
| `Zumo32U4Encoders` | odometry | `getCountsLeft()`, `getCountsRight()`, `getCountsAndResetLeft/Right()` |
| `Zumo32U4IMU` | pitch (seesaw) | `init()`, `readAcc()`, `readGyro()`, `read()`, vectors `a`, `g`, `m`; `configureForFaceUphill()` |
| `Zumo32U4ProximitySensors` | end-block | `initThreeSensors()`, `read()`, `countsFront/Left/RightWith{Left,Right}Leds()` |
| `Zumo32U4ButtonA` | start trigger | `waitForButton()`, `isPressed()` |
| `Zumo32U4Buzzer` | done sound | `playFrequency()` / `play()` |
| `Zumo32U4LCD` / `Zumo32U4OLED` | debug display | `print()`, `gotoXY()`, `clear()` |
| `Zumo32U4` main class | battery | `readBatteryMillivolts()` |

Source: [library landing page](https://pololu.github.io/zumo-32u4-arduino-library/)

---

## 3. Line-sensor calibration & reading → `ZumoHardware`, `LineSensorAnalyse`

✅ **FIRM.** `Zumo32U4LineSensors` inherits the standard **QTRSensorsRC** algorithms (it
overrides none):

- **`calibrate()`** accumulates per-sensor **min/max** raw readings across repeated calls
  (10 reads batched per call, *not* reset between calls).
- **`readCalibrated(v)`** normalises each sensor to **0–1000**:
  `value = ((reading − calMin) × 1000) / (calMax − calMin)`, clamped to `[0,1000]`
  (0 = at/below calibrated min, 1000 = at/above calibrated max).
- **`readLine(v)`** returns the reflectance-weighted average
  `(0·v0 + 1000·v1 + 2000·v2 + 3000·v3 + 4000·v4) / Σv` → **0–4000, centre 2000** (5-sensor,
  dark-line mode). It **remembers the last position** when the line is lost.

**Calibration routine for the contest (→ a method in `StartToestand.enter()` or `ZumoHardware.init()`):**

1. `lineSensors.emittersOn()` (default).
2. Loop ~100–200×: `lineSensors.calibrate()` while the robot **rotates in place** so every
   sensor sweeps fully across the **black line** and the **white surface**. 🟡
3. **For colour bands you must additionally** sweep each sensor across **green / grey / brown**
   swatches (or run a separate characterisation pass that prints `readCalibrated()` for each
   colour). 🔴 — without this the colour states have no thresholds.

Sources: [LineSensors class](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_line_sensors.html),
[QTRSensors guide 0J19/3](https://www.pololu.com/docs/0J19/3)

---

## 4. 🔴 THE CRUX — colour discrimination by reflectance bands → `LineSensorAnalyse`, `RobotConfig.DREMPEL_*`

🔴 **MEASURE ON-TRACK.** This is the make-or-break problem and the **only** requirement with
no documented Pololu support.

**What's confirmed (physics):** the QTR sensors measure reflectance by **RC-decay timing** —
shorter decay = more reflection — producing a **continuous** value, not a binary reading. So
multi-band discrimination is *physically possible*. ✅

**What's NOT available:** no vendor source publishes 950 nm reflectance for green/brown/grey.
Pololu explicitly frames the sensors as **"light vs dark"** and notes reliability is best when
**only black/white** (no intermediate colours) must be separated.

**Concrete strategy (the best the evidence supports):**

- In dark-line mode, after calibration expect **black ≈ near 1000**, **white ≈ near 0** on the
  `readCalibrated()` 0–1000 scale. Green/grey/brown fall in **intermediate bands** whose exact
  position depends on the specific pigment, lighting, and sensor height. 🔴
- Define bands as `[L, H]` windows and classify a colour when a **majority of sensors** agree
  (voting suppresses single-sensor noise). This is exactly what your
  `DREMPEL_GROEN_L/H`, `DREMPEL_BRUIN_L/H`, `DREMPEL_GRIJS_L/H` pairs are for.
- **Expected ambiguities:** **green vs grey** and **brown vs black** are the likely
  confusions. If voting can't separate them, add a secondary discriminator: an
  **emitters-off ambient** reading, or a motion/multi-sample signature.

**Mapping to `LineSensorAnalyse`:**

| Method | Logic (after `updateWaarden`) |
|--------|------------------------------|
| `lijnZichtbaar()` | any/most sensors above `DREMPEL_ZWART` (line present) |
| `isGroeneLijn()` | majority of sensors inside `[DREMPEL_GROEN_L, DREMPEL_GROEN_H]` |
| `isBruineLijn()` | majority inside `[DREMPEL_BRUIN_L, DREMPEL_BRUIN_H]` |
| `grijsTapeLinks()` | left-side sensor(s) inside grey band |
| `grijsTapeRechts()` | right-side sensor(s) inside grey band |
| `grijsTapeBeiden()` | both sides in grey band → **seesaw trigger** |
| `startLijnGezien()` | all/most sensors black at once (perpendicular start line) |

> ⚠️ Treat **every** `DREMPEL_*` number as a placeholder until measured. Do a dedicated
> "colour characterisation" sketch first: print `readCalibrated()` over each real swatch and
> read the bands straight off the data.

Sources: [product/961](https://www.pololu.com/product/961), [docs 0J13](https://www.pololu.com/docs/0J13),
[docs 0J63/3.5](https://www.pololu.com/docs/0J63/3.5),
[forum: colour sensor](https://forum.pololu.com/t/color-sensor/3644),
[forum: QTR-RC multiple colours](https://forum.pololu.com/t/qtr-rc-to-detect-multiple-colored-lines/5775)

---

## 5. PD line following → `RijController.stuurPD()`, `LijnVolgenToestand`, `RobotConfig.KP/KD`

✅ **FIRM — directly copyable from the official `LineFollower.ino`:**

```cpp
int16_t position = lineSensors.readLine(values);   // 0..4000, centre 2000 (5 sensors)
int16_t error    = position - 2000;
int16_t speedDifference = error / 4 + 6 * (error - lastError);   // KP = 1/4, KD = 6
lastError = error;

int16_t leftSpeed  = (int16_t)maxSpeed + speedDifference;
int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;
leftSpeed  = constrain(leftSpeed,  0, (int16_t)maxSpeed);   // wheels never reverse
rightSpeed = constrain(rightSpeed, 0, (int16_t)maxSpeed);
motors.setSpeeds(leftSpeed, rightSpeed);
```

- **No integral term** — Pololu states it's "generally not very useful for line following."
- `KP = 1/4`, `KD = 6`, `maxSpeed = 400` are Pololu's "decent default for many Zumo motor
  choices" — explicitly a **starting point requiring per-robot trial-and-error**, *not* an
  optimum for fastest time. 🟡
- **Note the integer division** `error/4`: if you store `KP` as a float in `RobotConfig`, use
  `KP = 0.25`, `KD = 6.0` and compute `speedDifference = KP*error + KD*(error-lastError)`.

**Tuning method** 🟡 (from the "trial and error" instruction):
1. Start `KP = 0.25`, `KD = 6`, `maxSpeed` moderate (~200).
2. Raise `KP` until it tracks tight corners without lagging.
3. Raise `KD` until oscillation/overshoot on straights damps out.
4. *Only then* raise `maxSpeed` toward 400. Reduce `maxSpeed` to cut corner overshoot.

> If you chose **Option A (3 sensors)** in §1: `readLine()` → **0–2000, centre 1000**, so
> `error = position − 1000`.

Sources: [LineFollower.ino](https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/LineFollower/LineFollower.ino),
[Motors class](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_motors.html),
[PID write-up](https://towardinfinity.medium.com/pid-for-line-follower-11deb3a1a643),
[robotresearchlab](http://robotresearchlab.com/2019/02/12/how-to-program-a-line-following-robot/)

---

## 6. Gaps, intersections & grey-direction memory → `LijnVolgenToestand`, `ZumoRobot`

🟡 **STANDARD.** None of this is in the official examples — it's standard line-follower
technique with **no cited numbers**; implement and tune it yourself.

- **Motor API ceiling** ✅: `setSpeeds(int16 left, int16 right)`, range **−400..400** (clamped
  in firmware). This fixes `SNELHEID_MAX = 400`.
- **Gaps ≤ 5 cm** → `LijnKwijt()`: when all sensors read white, **latch `lastError`** (or the
  last-seen side) and **drive straight at reduced speed** until the line reappears **or**
  `GEHEUGEN_MS` expires. `readLine()`'s "remember last position" helps a brief gap, but a
  robust ≥5 cm design should latch explicitly. `ZumoRobot.laatsteLijnGezien` (`millis()` stamp)
  + `RobotConfig.GEHEUGEN_MS` are exactly this. 🟡
- **Intersections / T-splits** → `behandelKruising()`: detect when **all/most calibrated
  sensors exceed the black threshold** at once. Don't trust `readLine()` there (it saturates).
- **Grey direction** → `behandelMarkeringen()`: when `grijsTapeLinks/Rechts()` fires, set
  `ZumoRobot.grijsLinksGezien` / `grijsRechtsGezien`; at the **next** intersection, **command a
  turn** in that direction instead of following `readLine()`. Clear the flag after acting.

Source: [Motors class](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_motors.html)

---

## 7. Speed tiers → `RobotConfig.SNELHEID_*`

| Constant | Suggested start | Basis |
|----------|----------------|-------|
| `SNELHEID_MAX` | **400** | ✅ firmware ceiling |
| `SNELHEID_NORMAAL` | ~250–300, raise toward 400 | 🟡 tune after PD is stable |
| `SNELHEID_GROEN` | **200** | ✅ rule-mandated half of 400 |
| `SNELHEID_WIP` | ~100–150 | 🟡 controlled on the ramp |
| `SNELHEID_ZOEKEN` | ~200 (approach), veer 250, ram 400 | ✅ from Sumo example (§9) |

---

## 8. IMU pitch for the seesaw → `PitchDetector`, `WipOmhoog/Wacht/OmlaagToestand`

✅ **FIRM (API).** `Zumo32U4IMU` exposes public vectors `a`, `g`, `m` (`int16` `.x/.y/.z`).
`readAcc()` refreshes `a`, `readGyro()` refreshes `g`, `read()` refreshes all. Call one of these
**every loop before** reading the vectors. `configureForFaceUphill()` sets accel **±2 g
(16384 LSB/g)**; turn/balance configs set gyro **±2000 dps @ 833 Hz**. The chip is an
**LSM6DS33** (accel+gyro). The library does **not** state units — scaling comes from the datasheet.

**Pololu's `FaceUphill.ino` is accelerometer-only and gives slope MAGNITUDE, not sign:** ✅

```cpp
imu.readAcc();
int16_t x = imu.a.x, y = imu.a.y;
int32_t magnitudeSquared = (int32_t)x*x + (int32_t)y*y;
// 16384 = ~1 g; 16384 * sin(5°) = 1427  → asin(1427/16384) ≈ 5.0°
if (magnitudeSquared > (int32_t)1427*1427) { /* on an incline > 5° */ }
```

**The contest needs SIGNED pitch** ("wait until pitch < 0°"), which FaceUphill cannot give.
Compute it from the accel and fuse with the gyro in a **complementary filter** 🟡:

```cpp
// accel gives an absolute (noisy) pitch; ratio of raw counts → no accel scaling needed
float accelPitch = atan2f(a.x, a.z) * 57.2958f;          // axis depends on mounting — confirm!
float gyroRate   = g.y * 0.070f;                          // ±2000 dps → ~0.070 dps/LSB (LSM6DS33)
pitch = alpha * (pitch + gyroRate * dt) + (1 - alpha) * accelPitch;   // alpha ≈ 0.95–0.98
```

- **Which axis is "forward pitch" depends on board mounting** — likely `a.x` with the array
  forward, but **confirm empirically** (print the vectors while tilting the robot). 🔴
- `atan2` of raw accel counts needs **no** scaling (it's a ratio); only the **gyro** needs the
  ~0.070 dps/LSB sensitivity, and `dt` should be **measured** with `micros()` (loop isn't fixed-rate).
- **Gyro bias zeroing** (average `g.y` while stationary at start) and the `alpha` / threshold
  values must be tuned on the real ramp. 🟡🔴

**Mapping to the Wip states:**

| State | Logic |
|-------|-------|
| `WipOmhoogToestand` | entered on `grijsTapeBeiden()`; `PitchDetector.isOmhoog()` (pitch > +threshold) |
| `WipWachtToestand` | at the tip; **wait until `pitch < 0`** (rule) before continuing |
| `WipOmlaagToestand` | `isOmlaag()` (pitch < −threshold) until `isVlak()` (≈0) → back to line following |

Sources: [IMU class](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_i_m_u.html),
[Zumo32U4IMU.h](https://raw.githubusercontent.com/pololu/zumo-32u4-arduino-library/master/src/Zumo32U4IMU.h),
[FaceUphill.ino](https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/FaceUphill/FaceUphill.ino),
[zumosegway IMU notes](https://zumosegway.readthedocs.io/en/latest/zumo/imu.html)

---

## 9. Encoder odometry → `RijController.rijdAfstand()` / `draaiGraden()`, `RobotConfig.CM_PER_PULSE`

✅ **FIRM (counts).** `Zumo32U4Encoders::getCountsLeft()/getCountsRight()`.
**12 counts per motor-shaft revolution** (both edges, both channels). After the gearbox:

```
counts per WHEEL rev = gear_ratio × 12
75:1 HP (true 75.81:1)  →  75.81 × 12 ≈ 909.7 counts/wheel-rev
```

🔴 **MEASURE.** Wheel diameter ≈ 3.7–3.9 cm (circumference ≈ 11.6–12.3 cm), so:

```
CM_PER_PULSE = wheel_circumference / counts_per_wheel_rev
            ≈ 12.0 cm / 909.7 ≈ 0.0132 cm/count        (placeholder)
20 cm drive  = 20 / CM_PER_PULSE ≈ ~1515 counts          (DOORRIJDEN_CM = 20, rule ✅)
```

- **Verify the actual motor gear ratio** (50:1 / 75:1 / 100:1) printed on your motors before
  fixing the constant — 50:1 → `50.xx×12`, 100:1 → `100.xx×12`.
- **Measure the real wheel circumference** (roll a known distance and back-solve) — the 12 CPR
  and 909.7 figures are firm, but the cm conversion needs your wheel.
- `rijdAfstand()`: reset counts, drive until `|counts| ≥ target`, stop. `draaiGraden()`:
  encoder count-delta vs wheel track **or** the **gyro** (often better angular accuracy). 🟡

Sources: [User's guide 0J63/3.4](https://www.pololu.com/docs/0J63/3.4),
[forum: measure distance with encoders](https://forum.pololu.com/t/measure-distance-using-encoders-zumo-32u4/19076)

---

## 10. End-block search & push → `BlokZoeker`, `BlokZoekToestand`

✅ **FIRM — maps directly onto the official `SumoProximitySensors.ino`** (functionally identical
to "scan for block, push out of circle"). Uses the 3 front proximity sensors
(`proxSensors.initThreeSensors()`; left = pin 20, front = pin 22, right = pin 4):

```cpp
proxSensors.read();                                   // once per loop
uint8_t sum  = proxSensors.countsFrontWithRightLeds() + proxSensors.countsFrontWithLeftLeds();
int8_t  diff = proxSensors.countsFrontWithRightLeds() - proxSensors.countsFrontWithLeftLeds();
// counts are 0..6; higher = more IR reflected (size/reflectivity/proximity dependent)

if (sum >= 4) motors.setSpeeds(rammingSpeed, rammingSpeed);   // rammingSpeed = 400 → push out
else if (diff >= 1) motors.setSpeeds(veerSpeedHigh, veerSpeedLow);   // 250, 0  → veer right
else if (diff <= -1) motors.setSpeeds(veerSpeedLow, veerSpeedHigh);  // veer left
else motors.setSpeeds(forwardSpeed, forwardSpeed);            // forwardSpeed = 200
// scan trigger: countsFront…Leds() >= 2
```

Map to `BlokZoeker`: `scanRondom()` rotates and watches `sum`/`diff` to find the bearing →
`besteRichting`/`besteWaarde`; `draaiNaarBlok()` turns toward the stronger side;
`duwTotRand()` rams at 400.

- ⚠️ **Pin-conflict reminder (§1):** the left/right proximity sensors share pins 20/4 with line
  sensors DN2/DN4. The block-search code above needs the proximity sensors, which on a stock
  robot means **only 3 line sensors** are available for the line/colour phase.
- 🔴 **Circle-boundary detection** (where the ring edge is, when the block is "out") is **not**
  covered by any example — detect it with the **line sensors** (white-outside-the-ring or a
  boundary line). No cited numbers.

Sources: [ProximitySensors class](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_proximity_sensors.html),
[SumoProximitySensors.ino](https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/SumoProximitySensors/SumoProximitySensors.ino),
[docs 0J63/3.6](https://www.pololu.com/docs/0J63/3.6)

---

## 11. State-machine best practices → `ZumoRobot`, `RobotToestand`, `SensorData`

✅ **FIRM (pattern in official examples).**

- **One sensor snapshot per loop.** At the top of `ZumoRobot.update()`, fill **one** `SensorData`
  (`readCalibrated`/`readLine`, optional `proxSensors.read()`, `imu.read()`), then feed *all*
  states from that snapshot — exactly the reason `SensorData` exists.
- **No `delay()`.** Use `millis()` deadlines for the 20 cm drive, `GEHEUGEN_MS` gap memory, and
  the pitch-wait. (Official examples use a `timeInThisState()`-style pattern.)
- **`setSpeeds()` once per iteration** with the final clamped values.
- **Battery compensation** 🟡: scale motor output by `readBatteryMillivolts()` as the battery
  sags, so behaviour stays consistent across a 5-minute run (recommended, not in verified claims).

Sources: [library landing](https://pololu.github.io/zumo-32u4-arduino-library/),
[FSM with millis()](https://forum.arduino.cc/t/using-millis-instead-of-delay-in-a-fsm/621986)

---

## 12. Recommended `RobotConfig.h` starting values (consolidated)

> ✅ firm · 🟡 tune · 🔴 measure on-track. **All `DREMPEL_*` colour bands are placeholders.**

| Constant | Start value | Flag |
|----------|------------|------|
| `SNELHEID_MAX` | 400 | ✅ |
| `SNELHEID_NORMAAL` | 250 → 400 | 🟡 |
| `SNELHEID_GROEN` | 200 | ✅ (rule) |
| `SNELHEID_WIP` | 120 | 🟡 |
| `SNELHEID_ZOEKEN` | 200 | ✅ |
| `DREMPEL_ZWART` | ~700 | 🔴 |
| `DREMPEL_GRIJS_L / _H` | — / — | 🔴 measure |
| `DREMPEL_GROEN_L / _H` | — / — | 🔴 measure |
| `DREMPEL_BRUIN_L / _H` | — / — | 🔴 measure |
| `GEHEUGEN_MS` | 150 | 🟡 |
| `DOORRIJDEN_CM` | 20 | ✅ (rule) |
| `CM_PER_PULSE` | ~0.0132 | 🔴 measure wheel |
| `KP` | 0.25 | 🟡 |
| `KD` | 6.0 | 🟡 |

---

## 13. What you MUST resolve on the real robot/track (open questions)

1. **Colour bands** 🔴 — what `readCalibrated()` 0–1000 ranges do the actual green/brown/grey
   swatches produce at ride-height under contest light? Are green-vs-grey and brown-vs-black
   separable by majority voting, or is a secondary discriminator (emitters-off ambient) needed?
2. **IMU forward axis + filter** 🔴🟡 — which axis is forward pitch on your mounting, and what
   `alpha` + ascent/tip/descent thresholds detect the seesaw tip and the `pitch < 0` crossing
   without false triggers from vibration?
3. **Odometry** 🔴 — measured wheel circumference + confirmed gear ratio → exact `CM_PER_PULSE`
   and the count target for 20 cm; encoder-turns vs gyro-turns for acting on a grey direction.
4. **Speed envelope** 🟡 — max reliable `maxSpeed` (and re-tuned `KP`/`KD`) on straights vs
   corners for the 1.5–2.5 cm line, and `GEHEUGEN_MS` / intersection thresholds that survive
   5 cm gaps and T-splits at competitive speed.
5. **§1 hardware choice** ⚠️ — 3 line + 3 prox, 5 line + front prox, or rewire? Settle first.

---

## 14. Sources

**Primary (Pololu docs, library source, official examples) — highest confidence:**
- Library: [landing](https://pololu.github.io/zumo-32u4-arduino-library/) ·
  [LineSensors](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_line_sensors.html) ·
  [Motors](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_motors.html) ·
  [IMU](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_i_m_u.html) ·
  [ProximitySensors](https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_proximity_sensors.html)
- Examples: [LineFollower.ino](https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/LineFollower/LineFollower.ino) ·
  [FaceUphill.ino](https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/FaceUphill/FaceUphill.ino) ·
  [SumoProximitySensors.ino](https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/SumoProximitySensors/SumoProximitySensors.ino) ·
  [LineSensorTest.ino](https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/LineSensorTest/LineSensorTest.ino)
- User's guide: [0J63/3.4 encoders](https://www.pololu.com/docs/0J63/3.4) ·
  [0J63/3.5 line/prox pins](https://www.pololu.com/docs/0J63/3.5) ·
  [0J63/3.6 proximity](https://www.pololu.com/docs/0J63/3.6) ·
  [QTRSensors 0J19/3](https://www.pololu.com/docs/0J19/3) ·
  [reflectance product 961](https://www.pololu.com/product/961) · [0J13](https://www.pololu.com/docs/0J13)

**Forum / practitioner (corroborating, lower confidence):**
- [Pololu forum: colour sensor](https://forum.pololu.com/t/color-sensor/3644) ·
  [QTR-RC multiple colours](https://forum.pololu.com/t/qtr-rc-to-detect-multiple-colored-lines/5775) ·
  [encoder distance](https://forum.pololu.com/t/measure-distance-using-encoders-zumo-32u4/19076)
- PID write-ups: [towardinfinity](https://towardinfinity.medium.com/pid-for-line-follower-11deb3a1a643) ·
  [robotresearchlab](http://robotresearchlab.com/2019/02/12/how-to-program-a-line-following-robot/) ·
  [gupta.jay](https://medium.com/@gupta.jay/line-follower-robot-algorithm-optimizations-for-better-line-following-64297aeed17e)
- IMU: [zumosegway](https://zumosegway.readthedocs.io/en/latest/zumo/imu.html) ·
  FSM: [Arduino millis FSM](https://forum.arduino.cc/t/using-millis-instead-of-delay-in-a-fsm/621986)

---

*Research method: 5 search angles → 21 sources fetched → 97 claims → 25 adversarially
verified (3-vote, needs 2/3 to refute). Result: 25 confirmed, 0 refuted. Verified facts are
firm; everything flagged 🟡/🔴 is your tuning/measurement work.*
