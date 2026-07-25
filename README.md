# CYD Air Monitor

An ESP32 firmware project that turns a **Cheap Yellow Display (CYD)** into a tabletop temperature and humidity monitor. It reads a **DHT11** sensor and shows live values on an **LVGL** UI.

This project is based on the [cyd-lvgl-template](https://github.com/Deejpotter/cyd-lvgl-template) display/touch/LVGL foundation.

## Supported hardware

| PlatformIO env | Board | Touch | DHT11 pin |
|----------------|-------|-------|-----------|
| `2432s028r` (default) | ESP32-2432S028R (original CYD) | Resistive (XPT2046) | GPIO 27 |
| `jc2432w328r` | JC2432W328R | Resistive (XPT2046) | GPIO 21 |
| `jc2432w328c` | JC2432W328C | Capacitive (CST820) | GPIO 22 |
| `jc4827w543r` | JC4827W543R (Guition 4.3") | Resistive (XPT2046) | GPIO 17 (P4) |

Most CYD boards use a **2.8" ST7789** at **320×240**. The JC4827W543R uses a **4.3" NV3041A** at **480×272** (ESP32-S3, Arduino_GFX). Pin mappings and touch type are defined per environment in `platformio.ini`.

**Interactive docs:** [Board Explorer & guides](docs-site/README.md) — includes [gotchas](/cyd-air-monitor/boards/gotchas) and [references](/cyd-air-monitor/boards/references). Run `cd docs-site && npm install && npm run dev`, or view on [GitHub Pages](https://deejpotter.github.io/cyd-air-monitor/) after deploy.

Connect the DHT11 data pin to the GPIO listed above (VCC to 3.3 V, GND to GND).

## Requirements

- [Visual Studio Code](https://code.visualstudio.com/) with the [PlatformIO](https://platformio.org/) extension
- A supported CYD board
- A DHT11 temperature/humidity sensor

## Quick start

```bash
git clone https://github.com/Deejpotter/cyd-air-monitor.git
cd cyd-air-monitor
pio run -e 2432s028r              # build (change env for your board)
pio run -e 2432s028r --target upload
pio device monitor                # serial output at 115200 baud
```

On **Linux**, if serial upload/monitor hits permission denied, add yourself to `dialout` and open a **new terminal** (or run `newgrp dialout`):

```bash
sudo usermod -aG dialout $USER
```

On first build, `scripts/copy_template.py` copies display config files from `template files/` into `.pio/libdeps/<env>/`. Do not edit files under `.pio/` directly — change the templates and rebuild.

**Display orientation** varies by board revision. See [docs/DISPLAY_CONFIG.md](docs/DISPLAY_CONFIG.md) for the full matrix and tuning guide.

## Settings UI

Tap the **gear icon** on the dashboard to open Settings:

- **WiFi** — scans for nearby networks, pick an SSID from the dropdown, enter password, and connect. Credentials are saved to NVS and the device auto-reconnects on boot.
- **Web dashboard** — when WiFi is connected, open **http://cydmon.local** in a browser on the same network for live readings and settings (WiFi, display rotation, touch cal). Same NVS as the touchscreen.
- **Touch Test** — live touch coordinates and a touch dot. On resistive boards, min/max calibration can be adjusted and saved to NVS.

Settings persist across reboots in ESP32 NVS (Arduino `Preferences` library, namespace `cydmon`). Use **Forget WiFi** on the WiFi screen to clear saved credentials.

## Project structure

```
src/
├── main.cpp                    # App entry: sensors, UI, scheduler
├── TemplateCode.{h,cpp}        # Display, touch, and LVGL setup
├── MainInterface.{h,cpp}       # Dashboard and screen navigation
├── WiFiSettingsScreen.{h,cpp}  # WiFi scan, SSID picker, connect
├── TouchConfigScreen.{h,cpp}   # Touch test and resistive calibration
├── WiFiConnectionManager.{h,cpp}
├── WebServerManager.{h,cpp}    # Local HTTP UI + JSON API (cydmon.local)
├── SettingsStore.{h,cpp}       # NVS persistence (WiFi, touch cal)
├── SensorManager.{h,cpp}       # DHT11 polling and change callbacks
├── PeriodicScheduler.*         # Non-blocking task scheduler
└── RGBledDriver.{h,cpp}        # On-board RGB LED (CYD)

template files/           # TFT_eSPI and LVGL config (copied at build time)
platformio.ini            # Board environments and build flags
scripts/copy_template.py  # Pre-build config copy script
```

## Choosing your board environment

Set the environment to match your hardware:

```bash
pio run -e jc2432w328r    # resistive-touch JC2432W328R
pio run -e jc2432w328c    # capacitive-touch JC2432W328C
pio run -e jc4827w543r    # Guition 4.3" ESP32-S3 JC4827W543R
pio run -e 2432s028r      # original ESP32-2432S028R CYD
```

Or change `default_envs` in `platformio.ini`.

## Upstream template

Display, touch, and LVGL plumbing come from [Deejpotter/cyd-lvgl-template](https://github.com/Deejpotter/cyd-lvgl-template). To pull in upstream fixes:

```bash
git remote add upstream https://github.com/Deejpotter/cyd-lvgl-template.git   # once
git fetch upstream
git merge upstream/main    # or rebase, as you prefer
```

Resolve conflicts carefully — this repo adds sensor and UI code on top of the template.

## Troubleshooting

**Display blank or garbled** — Confirm you are building for the correct env. Check that `User_Setup.h` was copied (look for `[copy_template]` lines in the build log).

**Touch not working** — Resistive and capacitive boards use different envs. Do not mix `jc2432w328r` and `jc2432w328c`.

**DHT11 reads NaN** — Check wiring and that you are using the DHT pin for your env (see table above). Allow a few seconds after power-on for the first valid reading.

**Serial permission denied (Linux)** — `sudo usermod -aG dialout $USER`, then open a new terminal. For JC4827 use `/dev/ttyACM0`.

**LVGL layout clipped** — Rotation/resolution must match between TFT_eSPI (`User_Setup.h`) and `TemplateCode`. Rebuild after changing templates.

## Resources

- [ESP32 Cheap Yellow Display (community hub)](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display)
- [CYD pinout (ESP32-2432S028R)](https://randomnerdtutorials.com/esp32-cheap-yellow-display-cyd-pinout-esp32-2432s028r/)
- [SquareLine CYD template discussion](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/discussions/102)
- [LVGL documentation](https://docs.lvgl.io/)

## License

See the upstream [cyd-lvgl-template](https://github.com/Deejpotter/cyd-lvgl-template) repository for licensing of the original template code.
