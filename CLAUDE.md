# Pennyworth — project playbook (nickname: "pet watch")

> **Claude Code: read this whole file before doing anything.** Every design decision below is FINAL — do not re-ask the user about them. Work through the Roadmap in order, autonomously. Only stop to ask the user when a step is listed under "Ask the user only for". After every milestone: build, commit, update DEVLOG.md, then continue to the next one.

---

## 1. What this is
A DIY wearable **pendant** (worn on a necklace): a calm cat-butler companion and assistant named **Pennyworth**, running on an ESP32-S3 round touch LCD. Later phases add an iPhone companion app and a voice AI on the user's home server.

The user's own words: a portable assistant he built himself. It's also a **public portfolio project** (GitHub + LinkedIn), so code quality, README, and devlog matter.

## 2. Hardware
- **Board:** Waveshare **ESP32-S3-Touch-LCD-1.28**
  - 1.28" round 240×240 LCD (GC9A01-class driver), capacitive touch (CST816-class), 6-axis IMU (QMI8658-class: accelerometer + gyro), USB-C, LiPo connector with onboard charging, ESP32-S3 with Wi-Fi + BLE.
  - **Verify every chip and pin against the official Waveshare wiki/demo code for this exact board. Never guess pin numbers.** Put all pins in one file (`include/board_pins.h`).
- **Not on the board (don't build features needing them):** GPS, magnetometer/compass, heart rate, mic, speaker, vibration motor.
- **The board has NOT arrived yet.** Everything in Phase 1 must be developed and tested in a **desktop simulator** first, and structured so it runs on hardware with only a pin/driver config change.

## 3. Pennyworth — the character
- Butler cat inspired by the early-2010s animated Batman's Alfred: **round glasses, black suit, black bow tie**. Dignified, composed.
- **NOT a Tamagotchi.** No hunger, health, death, feeding, or care mechanics. He never needs anything from the user.
- **Asleep (idle):** curled in a ball in a corner of the screen, time displayed.
- **Awake:** sits up, eyes open, glasses glint, attentive. Curls back to sleep after inactivity.
- Small idle life: occasional breathing, ear twitch, tail flick, slow blink. Subtle, not busy.
- Personality (used in Phase 3 AI): calm, formal, slightly dry British butler, loyal, occasional cat purrs.
- Art: original pixel art sprites designed for a 240×240 round screen. Keep everything inside the circle (corners are invisible). Big, simple, readable shapes.

## 4. Screen and world
- **Same background asleep or awake.** One consistent day/night world.
- **Time-based tone** with smooth transitions: warm amber morning → bright clean day → warm evening → dim cool night.
- **Day:** a small decorative sun in a corner (NOT a real sun position).
- **Night:** dim screen, softly twinkling decorative stars, and a **moon showing the real current lunar phase**, calculated from the date with a standard algorithm (no lookup tables or calendar data).
- **Time** is the main readout, large and legible.
- **Explicitly dropped — do not build:** step counter, activity rings, distance, compass, GPS/location, real sun position, weather.
- Time source: set from the computer at flash time / RTC for now; later synced from the iPhone over BLE.

## 5. Interaction
- **Wake = double-tap**, detected with the IMU (single taps happen by accident on a pendant; reject them). Touchscreen tap may also wake as a fallback.
- In Phase 3: after waking, saying **"Pennyworth"** starts a conversation. Mic only listens after a double-tap (saves battery).
- Auto-sleep after a short period of no interaction.
- **Persist state** (settings, last state) to flash so a reboot or dead battery doesn't reset anything.
- Launcher/menu: minimal, custom, only the user's own screens. No bloat.

## 6. Power and enclosure (design context)
- Battery target **2–5 days per charge**. Bigger swappable LiPo.
- Screen off or very dim most of the time; **aggressive deep/light sleep**; wake on IMU interrupt.
- Case (user is sourcing): deliberately roomy for a future speaker, vibration motor, mic. Sweat/rain resistant, not waterproof. USB-C reachable from outside with a plug.

## 7. Phases
1. **Phase 1 — Pennyworth on the ESP32** ← current phase
2. **Phase 2 — iPhone companion app + BLE.** iOS has no Web Bluetooth, so it must be a **native app** (prefer Flutter or React Native). Syncs time, settings; later relays voice.
3. **Phase 3 — AI + voice.** Ollama on the user's home server with RAG over his own notes/data. I2S mic + speaker + amp added to the pendant. Speech-to-text (e.g. Whisper) and text-to-speech run on the phone/server, never on the ESP32. Graceful fallback when the server is unreachable (Pennyworth says so politely, doesn't freeze).

## 8. Roadmap — do these in order
### Milestone 0 — Repo and tooling
1. Create the project folder structure (PlatformIO layout: `src/`, `include/`, `lib/`, `test/`, `assets/`, `tools/`, `docs/`).
2. `git init`, add a sensible `.gitignore` (PlatformIO, OS, editor files, secrets, voice files).
3. Create a public GitHub repo named `pennyworth` with `gh repo create` and push. (If `gh` isn't logged in, ask the user to run `gh auth login`, then continue.)
4. Make sure the PlatformIO extension/CLI is installed; install it if not.
5. Create `platformio.ini` targeting the ESP32-S3 with PSRAM, plus a **native/desktop simulator** environment.
6. Create `DEVLOG.md`, `README.md` (project pitch, status, build steps), and keep this `CLAUDE.md` in the root.
7. Commit: "Milestone 0: project setup".

### Milestone 1 — Display stack + simulator
- Pick ONE graphics approach that works on both the ESP32-S3 and the desktop simulator (e.g. LVGL with an SDL simulator, or LovyanGFX). Choose the lightest option that meets the needs; justify the choice in DEVLOG.md.
- Get a test screen (round mask, clock text) rendering in the simulator.
- Commit.

### Milestone 2 — The world
- Day/night tone system with smooth transitions by time of day.
- Sun corner (day), twinkling stars + real moon phase (night). Unit-test the moon-phase math against known dates.
- Commit.

### Milestone 3 — Pennyworth sprites + state machine
- Original pixel art: asleep (curled), waking, awake (sitting), idle animations, going back to sleep.
- State machine: ASLEEP ↔ WAKING ↔ AWAKE ↔ SLEEPING, driven by events (double-tap, timeout).
- Commit.

### Milestone 4 — Input + power logic
- Double-tap detection from IMU data (with a simulated input path for the desktop — e.g. a key press).
- Sleep/wake power logic, written so it plugs into real deep sleep on hardware.
- State persistence to flash (abstracted so the simulator uses a file).
- Commit.

### Milestone 5 — Launcher/menu
- Minimal custom launcher reachable from the awake state. Only Pennyworth's screens + settings (brightness, sleep timeout).
- Commit.

### Milestone 6 — Hardware bring-up checklist (prep only until the board arrives)
- Write `docs/HARDWARE_BRINGUP.md`: exact steps to flash the board, verify display/touch/IMU, and calibrate double-tap.
- When the user says the board has arrived: follow it, flash, fix issues, commit.

Then stop and summarize Phase 1 for the user before starting Phase 2.

## 9. How to work
- **Autonomy:** don't ask the user to re-confirm anything decided in this file. Make reasonable technical choices yourself, record them in DEVLOG.md with the reason, and keep going.
- **Ask the user only for:** GitHub login, plugging in / flashing the physical board, buying parts, and genuinely new product decisions not covered here.
- **Skills:** use the **Ponytail** skill (simplest solution, reuse libraries and native features, YAGNI) on all code, and the **front-end** skill for the simulator UI, any web preview, and the Phase 2 app. Also use any tools/extensions already available in VS Code or the language toolchain.
- **Optimize hard for the ESP32:** low RAM/flash, no display stutter (partial redraws, dirty rectangles, DMA where supported), aggressive sleep. Optimize what matters on embedded; don't micro-tune what doesn't.
- **Code quality:** small modules, clear names, hardware access behind thin interfaces so the simulator and real board share all logic.
- **Git:** commit every time something works, with clear messages. Push regularly.
- **DEVLOG.md:** after each work session add a dated entry: what was done, what worked, what broke, decisions + why, next step. This is the handoff between sessions — read the latest entry at the start of every session and continue from there.
- **Never commit:** secrets, API keys, or any cloned character voice files.

## 10. Parts list (for the user — reference only)
**Phase 1:** Waveshare ESP32-S3-Touch-LCD-1.28 · LiPo battery (~500–1000 mAh for multi-day; must match the board's connector) · roomy case (3D-printed or bought) · USB-C silicone port plug · O-ring/gasket + conformal coating for sweat resistance · necklace chain (user sourcing).
**Optional soon:** small vibration motor (haptics: buzz on wake, "purr"), LED for mic-on indicator.
**Phase 3:** I2S microphone · small speaker + I2S amp · optional magnetic pogo-pin charging · home server running Ollama.

## 11. Public release rules
- Repo, README, and DEVLOG are public-facing: keep them clean and professional (portfolio project).
- **Never commit or publicly demo a cloned character voice.** Public demos use a generic voice, with a README note that the real voice isn't shared for copyright reasons.
- Pennyworth's art is original; don't use official Batman/Alfred artwork or logos anywhere.
