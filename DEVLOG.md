# DEVLOG

## 2026-10-05 — Milestone 0: project setup
- Created PlatformIO-style folder layout (`src/`, `include/`, `lib/`, `test/`, `assets/`, `tools/`, `docs/`).
- `git init`, added `.gitignore` (PlatformIO build artifacts, OS/editor junk, secrets, cloned voice files).
- Installed PlatformIO Core via `pip install --user platformio` (no system package was available).
- Playbook copied in as `CLAUDE.md` — it's the source of truth for scope and roadmap.
- Next: Milestone 1 — pick a graphics stack that runs on both ESP32-S3 and a desktop simulator, get a round test screen with clock text rendering.
