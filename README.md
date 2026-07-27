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
lib/game_core/            Hardware-free game logic (unit tested, no Arduino deps)
  ILedController.h         LED abstraction so game logic doesn't touch NeoPixel directly
  GameMode.h               Interface every game mode implements
  HitArbiter.{h,cpp}       False-hit mitigation: amplitude arbitration + lockout
  SimpleRng.h              Tiny deterministic PRNG (used by Random Grid)
  KnockoutGame.{h,cpp}     Primary game mode
  RandomGridGame.{h,cpp}   Randomized-target variant
  TwoPlayerSplitGame.{h,cpp}  Left/right competitive variant
src/                       ESP32-S3 firmware (Arduino framework)
  SensorArray.{h,cpp}      Mux scanning -> HitArbiter
  LedController.{h,cpp}    Adafruit_NeoPixel-backed ILedController
  GameEngine.{h,cpp}       idle -> countdown -> playing -> results state machine,
                           holds a swappable list of GameMode instances
  main.cpp                 Wires it all together
test/test_game_core/      Native unit tests for lib/game_core (no hardware needed)
```

## Game modes

All modes share the same Serial control surface (115200 baud) via
`GameEngine`. Knockout is mode 0, so a plain `start` right after boot plays
it:
- `start` — begin a round in whichever mode is currently selected
- `reset` — abort back to idle at any time
- `mode` — cycle to the next mode (only while idle)
- `mode <n>` — jump straight to mode index `n` (0-based; printed at idle)

### Knockout (primary game)

Every configured zone (`ZONE_COUNT` in `config.h`, defaults to 12) lights up
amber/red as a live target. Hitting a zone knocks it out (turns it green) and
it stays down for the rest of the round.

Rules, exactly as specified: every zone must be knocked out within
`KNOCKOUT_DURATION_MS` (ninety seconds), period.
- Clear the whole board before the buzzer -> **score = clear time**
  (lower is better). `KnockoutGame` tracks the fastest clear time seen this
  session as a running best/record (resets on power-cycle — there's no
  persistent storage yet).
- Buzzer goes first -> **score = number of zones knocked out**.

Both the round's score and the session-best clear time print to Serial at
the end of every round.

### Random Grid

Same clear-all-before-the-timer rules as Knockout, but only
`RANDOM_TARGET_COUNT` of the `ZONE_COUNT` zones are chosen as live targets
each round (picked fresh via `SimpleRng`, reseeded from `millis()` at the
start of each round) — the rest of the panel stays dark all round. Harder
than Knockout since players have to recognize which panels are actually
live.

### Two-Player Split

Zones `[0, TWO_PLAYER_LEFT_COUNT)` belong to the left player, the rest to the
right player (`TWO_PLAYER_LEFT_COUNT` defaults to half of `ZONE_COUNT`). Each
side races to clear their own zones independently; whoever clears first wins
immediately, or if `SPLIT_GAME_DURATION_MS` runs out first, whoever cleared
more zones wins (an equal count is a draw). This is written generically
(`zoneCount`/`leftCount` are just constructor params) so it's ready for the
"6 zones per side" layout once you're on the 12-panel (Phase 3) build.

Results (hits, timing per zone, wasted hits on non-target/already-cleared
zones, and — for Two-Player Split — the winner) print to Serial after every
round for all three modes.

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
`begin()`, `onHit()`, `update()`, `render()`, `isGameOver()`, `result()`
(`reseed()` is optional, only needed if your mode uses randomness). Since it
only depends on `ILedController`, you can write and unit-test a new mode
(reaction-time drills, ball-speed scoring, etc.) the same way the existing
three are tested, before ever touching real hardware. Then add an instance of
it to the `gameModes[]` array in `src/main.cpp` — `GameEngine` picks it up
automatically, no engine changes needed.

Note on assumptions: the project plan names "random grid pattern" and
"two players left/right 6 grids" as future modes but doesn't specify their
exact win conditions, so the implementations here made a call — Random Grid
plays like Knockout with a randomized target subset, and Two-Player Split
is a race-to-clear-your-side. If those aren't the rules you had in mind, the
logic to change lives entirely in `RandomGridGame.cpp` / `TwoPlayerSplitGame.cpp`
and their tests — nothing else needs to change.

## Scaling from Phase 1 to Phase 3

`ZONE_COUNT` in `config.h` defaults to 12 (the full wall Knockout is scored
against). Drop it to 1 or 4 while you're still on the single-panel or
four-panel prototype. The CD74HC4067 supports up to 16 channels, so no mux
changes are needed until past 12 zones.
