# Soccer Shooting Wall — Firmware

Firmware for the DIY Smart Soccer Target Wall (ESP32-S3 + CD74HC4067 mux +
piezo impact sensors + WS2812B LEDs), built with [PlatformIO](https://platformio.org/).

## Hardware assumed

Per the project plan's block diagram:

```
Soccer Ball -> Target Panel -> Piezo Sensor -> Protection/Peak-Hold Circuit
  -> CD74HC4067 Mux -> ESP32-S3 -> 74AHCT125 Level Shifter -> WS2812B LEDs
```

All pin numbers, the hit threshold, and timing constants live in
`include/config.h`. **They are starting-point placeholders** — the plan
specifies the analog front-end at a block-diagram level, not exact wiring, so
tune `HIT_THRESHOLD` against your real peak-hold circuit's output and adjust
the mux/LED pin assignments to match how you actually wire the Feather.

## Repo layout

```
platformio.ini
include/config.h        Pin map, thresholds, timing (edit this per your build)
lib/game_core/          Hardware-free game logic (unit tested, no Arduino deps)
  ILedController.h       LED abstraction so game logic doesn't touch NeoPixel directly
  GameMode.h             Interface every game mode implements
  HitArbiter.{h,cpp}     False-hit mitigation: amplitude arbitration + lockout
  GridClearGame.{h,cpp}  MVP game mode
src/                     ESP32-S3 firmware (Arduino framework)
  SensorArray.{h,cpp}    Mux scanning -> HitArbiter
  LedController.{h,cpp}  Adafruit_NeoPixel-backed ILedController
  GameEngine.{h,cpp}     idle -> countdown -> playing -> results state machine
  main.cpp               Wires it all together
test/test_game_core/    Native unit tests for lib/game_core (no hardware needed)
```

## MVP game: Grid Clear

Every configured zone (`ZONE_COUNT` in `config.h`) lights up amber/red as a
live target. Hitting a zone turns it green and it stays cleared for the rest
of the round. The round ends the instant all zones are cleared, or when the
time limit (`GRID_GAME_DURATION_MS`) runs out first — whichever comes first.
Results (hits, timing per zone, wasted hits on already-cleared zones) print
to Serial.

Control it over the Serial monitor (115200 baud):
- `start` — begin a round (countdown, then play)
- `reset` — abort back to idle at any time

## False-hit mitigation

Implemented in `HitArbiter` per the plan's rules: each scan cycle, every
zone's reading above `HIT_THRESHOLD` becomes a candidate; candidates are
compared for `ARBITRATION_WINDOW_MS` (10–20ms) and only the strongest wins,
so simultaneous crosstalk between panels can't double-register. The winning
zone is then locked out for `HIT_LOCKOUT_MS` (100–250ms) so its own ringing
can't re-trigger it. This logic is hardware-independent and fully unit
tested — see `test/test_game_core/test_hit_arbiter.cpp`.

## Building and flashing

```
pio run -e featheresp32s3 -t upload
pio device monitor
```

## Running the unit tests

```
pio test -e native
```

This compiles and runs `lib/game_core` against `test/test_game_core` on your
desktop — no ESP32 or sensors required. Use it whenever you change hit
detection or game logic.

## Adding a new game mode

Implement the `GameMode` interface (`lib/game_core/GameMode.h`):
`begin()`, `onHit()`, `update()`, `render()`, `isGameOver()`, `result()`.
Since it only depends on `ILedController`, you can write and unit-test a new
mode (random target pattern, two-player split-grid, reaction drills, etc.)
the same way `GridClearGame` is tested, before ever touching real hardware.
Then swap it into `GameEngine`/`main.cpp` in place of `GridClearGame`.

Modes planned per the project roadmap but not yet implemented: random-pattern
grid, two-player left/right split (6 zones each). These will likely need a
zone layout (rows/columns, which zones belong to which player) that isn't
needed by Grid Clear — add that to `config.h` when you build them.

## Scaling from Phase 1 to Phase 3

Bump `ZONE_COUNT` in `config.h` (1 -> 4 -> 12) as you move from the single
panel prototype through the four-panel and twelve-panel builds. The
CD74HC4067 supports up to 16 channels, so no mux changes are needed until
past 12 zones.
