# AI agent guide for cyd-air-monitor

This repo is a PlatformIO (Arduino/ESP32) LVGL project: a **temperature/humidity monitor** for Cheap Yellow Display (CYD) boards, built on top of [cyd-lvgl-template](https://github.com/Deejpotter/cyd-lvgl-template).

Supported boards (see `platformio.ini`):

- `2432s028r` (default): ESP32-2432S028R, resistive touch (XPT2046)
- `jc2432w328r`: JC2432W328R, resistive touch (XPT2046)
- `jc2432w328c`: JC2432W328C, capacitive touch (CST820 via bb_captouch)

## Architecture

- `src/TemplateCode.{h,cpp}` — Singleton that initializes TFT_eSPI, LVGL buffers/drivers, and touch input.
- `src/MainInterface.{h,cpp}` — LVGL UI only; displays temperature and humidity labels.
- `src/SensorManager.{h,cpp}` — DHT11 polling with change callbacks.
- `src/PeriodicScheduler.{h,cpp}` — Non-blocking periodic tasks.
- `src/RGBledDriver.{h,cpp}` — On-board RGB LED control.
- `src/main.cpp` — Wires TemplateCode, MainInterface, SensorManager, and scheduler.

Per-board config is driven by PlatformIO environments in `platformio.ini` and TFT_eSPI `User_Setup.h` copied by a pre-build script.

## Critical workflows

- Build: `pio run -e 2432s028r` (or `-e jc2432w328r` / `-e jc2432w328c`)
- Upload: `pio run -e 2432s028r --target upload`
- First build: `scripts/copy_template.py` copies from `template files/` into `.pio/libdeps/<env>/`:
  - `template files/<env>/User_Setup.h` → `.pio/libdeps/<env>/TFT_eSPI/User_Setup.h`
  - `template files/lv_conf.h` → `.pio/libdeps/<env>/lv_conf.h`
- Do not edit files under `.pio/`; edit `template files/` and rebuild.
- Clean libdeps after template changes: delete `.pio/` or `git clean -fdx`, then rebuild.

## DHT11 pins (defined in main.cpp)

- `2432s028r`: GPIO 27
- `jc2432w328c`: GPIO 22
- `jc2432w328r`: GPIO 21

## Project-specific patterns

- Keep hardware logic out of `MainInterface`; pass data via `setTemperature()` / `setHumidity()`.
- Use `SensorManager` + `PeriodicScheduler` for non-blocking sensor reads.
- Touch type is selected by env macros in `platformio.ini` (`TOUCH_TYPE_RESISTIVE` / capacitive via `CST820_TOUCH`).
- Keep `main.cpp` minimal — no duplicate display/touch init; `TemplateCode` owns that.

## Upstream

- Origin: `https://github.com/Deejpotter/cyd-air-monitor.git`
- Upstream template: `https://github.com/Deejpotter/cyd-lvgl-template.git`

When syncing upstream, expect conflicts in shared template files (`TemplateCode.*`, `platformio.ini`, `template files/`).

## Quick file map

- `platformio.ini` — environments, build flags, pre-build script
- `scripts/copy_template.py` — copies display configs into libdeps
- `template files/` — source-of-truth for TFT_eSPI and LVGL config
- `src/TemplateCode.*` — LVGL + TFT + touch glue
- `src/MainInterface.*` — LVGL UI
- `src/SensorManager.*` — DHT11 driver wrapper

## Common issues

- Wrong env for your board → display/touch/DHT pin mismatch.
- LVGL clipped → rotation/resolution mismatch between `User_Setup.h` and `TemplateCode`; rebuild after fixing templates.
- `[copy_template] Skipping (not found): User_Setup.h` — add `template files/<env>/User_Setup.h` for that environment.
