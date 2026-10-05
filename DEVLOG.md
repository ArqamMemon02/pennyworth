# DEVLOG

## 2026-10-05 — Milestone 0: project setup
- Created PlatformIO-style folder layout (`src/`, `include/`, `lib/`, `test/`, `assets/`, `tools/`, `docs/`).
- `git init`, added `.gitignore` (PlatformIO build artifacts, OS/editor junk, secrets, cloned voice files).
- Installed PlatformIO Core via `pip install --user platformio` (no system package was available).
- Playbook copied in as `CLAUDE.md` — it's the source of truth for scope and roadmap.
- Next: Milestone 1 — pick a graphics stack that runs on both ESP32-S3 and a desktop simulator, get a round test screen with clock text rendering.

## 2026-10-05 — Milestone 1: display stack + simulator
- **Graphics choice: LVGL 9.6 with its built-in SDL simulator.** Picked over LovyanGFX because LVGL ships an official SDL desktop backend (`lv_sdl_window_create`) out of the box — the simulator deliverable needs no custom wrapper — and the widget/timer/animation system LVGL already has is exactly what Milestones 2-5 need (day/night transitions, state-machine-driven sprites, a launcher). LovyanGFX is lighter and has great GC9A01 support, but has no desktop simulator story, so hitting "round mask + clock text in the simulator" would've meant writing that bridge by hand first. Heavier dependency, but it's the lazier path to everything the roadmap actually asks for.
- Config: copied LVGL's `lv_conf_template.h` into `include/lv_conf.h` (enabled via `LV_CONF_INCLUDE_SIMPLE`), flipped it on, enabled Montserrat 28 for the clock face, and made `LV_USE_SDL` conditional on a `PENNYWORTH_SIMULATOR` build flag so the ESP32 build doesn't need SDL headers.
- Hardware-agnostic UI code lives in `lib/pennyworth_ui/` — a single `pennyworth_ui_init()` that draws the round mask (visualizes the physical display's circular visible area) and a live clock label. Called identically from both entry points.
- Two entry points, selected per-env via `build_src_filter`: `src/main_native.cpp` (SDL window + mouse input, `lv_timer_handler()` loop) and `src/main_esp32.cpp` (Arduino `setup()`/`loop()`, LVGL display buffer in RAM, flush callback is a stub). The ESP32 side has no real panel driver yet — the board hasn't arrived and CLAUDE.md says never guess pins — so `flush_cb` just acks the buffer. Milestone 6 replaces that stub with a real GC9A01 driver once pins are verified into `include/board_pins.h`.
- Gotcha: PlatformIO's "native" platform build doesn't put the project's `include/` dir on the search path when compiling library sources (only the final app TU), so LVGL's own `lv_conf.h` lookup failed until `-Iinclude` was added explicitly to both envs' `build_flags`.
- Verified: `pio run -e native -t exec` opens an SDL window showing the round mask with a live `HH:MM:SS` clock (confirmed via screenshot). `pio run -e esp32-s3` builds clean too — 34.6% RAM, 16.5% flash.
- Next: Milestone 2 — day/night tone system with smooth time-based transitions, sun corner for day, twinkling stars + real moon phase for night (unit-tested against known dates).
