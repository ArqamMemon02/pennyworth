# Pennyworth

A DIY wearable pendant: a calm cat-butler companion named **Pennyworth**, running on an ESP32-S3 round touch LCD. Worn on a necklace, shows the time against a day/night world, wakes on a double-tap. Later phases add an iPhone companion app and an on-device voice assistant backed by a home Ollama server.

Pennyworth is a butler cat in the style of early-2010s animated Batman's Alfred — round glasses, black suit, bow tie. He's not a Tamagotchi: no hunger, health, or care mechanics, just a quiet presence that wakes when you reach for him.

## Status

**Phase 1, Milestone 0** — repo scaffolding. The hardware (Waveshare ESP32-S3-Touch-LCD-1.28) hasn't arrived yet, so everything is being built and tested in a desktop simulator first.

See [DEVLOG.md](DEVLOG.md) for session-by-session progress and [CLAUDE.md](CLAUDE.md) for the full project playbook (hardware, design decisions, roadmap).

## Hardware

Waveshare ESP32-S3-Touch-LCD-1.28 — 1.28" round 240×240 LCD, capacitive touch, 6-axis IMU, Wi-Fi + BLE.

## Build

```
pio run -e esp32-s3        # build for hardware
pio run -e native -t exec  # build and run the desktop simulator
```

(Simulator environment lands in Milestone 1.)

## Roadmap

1. ~~Milestone 0 — repo and tooling~~
2. Milestone 1 — display stack + simulator
3. Milestone 2 — the world (day/night, moon phase)
4. Milestone 3 — Pennyworth sprites + state machine
5. Milestone 4 — input + power logic
6. Milestone 5 — launcher/menu
7. Milestone 6 — hardware bring-up

Phase 2 (iPhone app + BLE) and Phase 3 (AI + voice) follow once Phase 1 is complete.
