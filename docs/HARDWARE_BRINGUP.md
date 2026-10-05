# Hardware bring-up checklist

Prep-only until the board physically arrives — see CLAUDE.md Milestone 6.
Nothing here has been run against real hardware yet. When the board is in
hand: work through this in order, fix whatever breaks, update this file
with what actually worked, then commit.

## 0. Before touching the board

- [ ] Confirm the board is the exact model: **Waveshare ESP32-S3-Touch-LCD-1.28**
  (not the touch-less `ESP32-S3-LCD-1.28` — different board, different
  wiring, easy to confuse since the names and wiki pages are nearly
  identical).
- [ ] Open Waveshare's own wiki page and demo code for this exact board in
  a browser/clone, in hand, before writing a single pin number into code.
  Do not trust the pin table below as final — see "known pin conflict"
  below for exactly why.
- [ ] `pio run -e esp32-s3` on this repo as-is still builds clean (sanity
  check the toolchain before involving real hardware).

## 1. First power-on

- [ ] Connect via USB-C, confirm the board enumerates as a serial device
  (`pio device list` or `ls /dev/serial/by-id/`).
- [ ] Flash the current `esp32-s3` env as-is (`pio run -e esp32-s3 -t upload`).
  It won't show anything on the real panel yet (the flush callback in
  `src/main_esp32.cpp` is still a stub, see below) but this confirms the
  board flashes and the Arduino/LVGL stack boots without crashing — check
  serial monitor for a clean boot, no panics.

## 2. Pin verification — do this before writing `board_pins.h`

Researched from Waveshare's wiki and community demo repos (not yet
confirmed against the physical board or Waveshare's actual demo code in
hand), reasonably confident pins:

| Signal | Pin | Confidence |
|---|---|---|
| LCD DC (data/command) | GPIO8 | consistent across sources |
| LCD CS (chip select) | GPIO9 | consistent across sources |
| LCD SCLK | GPIO10 | consistent across sources |
| LCD MOSI | GPIO11 | consistent across sources |
| Touch (CST816S) SDA | GPIO6 | consistent across sources |
| Touch (CST816S) SCL | GPIO7 | consistent across sources |
| IMU (QMI8658) SDA | GPIO6 (shared bus w/ touch) | consistent across sources |
| IMU (QMI8658) SCL | GPIO7 (shared bus w/ touch) | consistent across sources |

**Known pin conflict — resolve this first, don't guess:**

- **LCD backlight**: one source says **GPIO2**, another (citing Waveshare's
  own `DEV_Config.h`) says **GPIO40**.
- **LCD reset**: one source says **GPIO14**, another (same Waveshare
  `DEV_Config.h` citation) says **GPIO12**.
- **LCD MISO**: one source says **GPIO12** (conflicting with that same
  source's own reset pin above — internally inconsistent), another says
  **-1 / not connected** (plausible — this is a write-only SPI display,
  MISO may genuinely be unused).
- IMU interrupt pin(s) are also unconfirmed — candidates GPIO3/GPIO4 came
  up but weren't cross-verified.

This is likely two different sources describing two different board
revisions, or one of them being wrong — exactly the situation CLAUDE.md's
"never guess pin numbers" rule exists for. Resolve by checking the
schematic PDF or demo code Waveshare ships with *this* board in hand
(product page → Resources → demo), not a second web search.

- [ ] Once confirmed, write every pin into **`include/board_pins.h`** —
  one file, nothing scattered across drivers — with a comment citing
  where each one was confirmed (schematic page, demo file + line, or
  physical continuity test).

## 3. Display

- [ ] Replace the stub `flush_cb` in `src/main_esp32.cpp` with a real
  GC9A01 driver (LVGL has one built in — `LV_USE_GC9A01` — or LovyanGFX if
  the built-in one fights the SPI timing) wired to the pins from
  `board_pins.h`.
- [ ] Flash, confirm the same round-mask + clock + world + Pennyworth UI
  that already works in the simulator now renders on the physical panel.
  No new app logic needed here — if `pennyworth_ui_init()` runs and the
  panel lights up correctly, Milestone 6's display portion is done.
- [ ] Check orientation/mirroring — round GC9A01 panels are easy to get
  mirrored or rotated 180°; fix via the driver's rotation setting, not by
  changing app code.

## 4. Touch

- [ ] Bring up the CST816S over I2C, confirm `pio run -e esp32-s3` style
  logging shows touch coordinates on contact.
- [ ] Wire a touch-down event to the existing `wake_on_tap_cb` path in
  `pennyworth_ui.cpp` (single tap, no change needed there — it's already
  hardware-agnostic, just needs a real `LV_INDEV_TYPE_POINTER` indev
  instead of the simulator's mouse).

## 5. IMU + double-tap calibration

- [ ] Bring up the QMI8658 over the shared I2C bus, read raw accelerometer
  data, confirm sane values at rest (~1g on one axis).
- [ ] Implement accel-spike → `pennyworth_input_raw_tap()` in
  `src/main_esp32.cpp`'s `loop()` (there's already a TODO comment marking
  exactly where). Start with a simple threshold-crossing detector on the
  magnitude of acceleration change; the double-tap *logic* itself
  (`lib/pennyworth_input/double_tap.*`) is already done and unit-tested —
  this step only needs to turn real accelerometer spikes into calls to
  `pennyworth_input_raw_tap()`.
- [ ] Calibrate the spike threshold empirically: tap the pendant as worn
  (not held flat on a desk — the accel signature differs), tune until
  deliberate double-taps register reliably and single accidental bumps
  (dropping it, bumping a table) don't. Record the chosen threshold and
  *why* in this file once tuned.

## 6. Power

- [ ] Fill in the two stub functions in `lib/pennyworth_power/pennyworth_power.cpp`
  (the `#else` / non-simulator branch): `pennyworth_power_enter_sleep()`
  configures `esp_sleep_enable_ext0_wakeup()` on the IMU interrupt pin
  from `board_pins.h` and calls `esp_light_sleep_start()` or
  `esp_deep_sleep_start()`; `pennyworth_power_enter_wake()` handles
  whatever re-init is needed after waking from sleep (display/touch may
  need re-initializing after deep sleep specifically — light sleep
  shouldn't).
- [ ] Measure actual current draw asleep vs. awake, sanity-check against
  the 2-5 day battery target in CLAUDE.md §6 with the battery capacity
  actually sourced.

## 7. Done

- [ ] All of the above flashed, verified, committed with a real DEVLOG
  entry (what worked, what didn't, final pin table, measured battery
  numbers).
- [ ] Phase 1 is complete. Summarize for the user before starting Phase 2
  (iPhone companion app + BLE), per CLAUDE.md §8.
