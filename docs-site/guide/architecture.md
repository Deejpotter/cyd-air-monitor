# Architecture

Firmware layout for contributors and fork maintainers.

## Entry point

`main.cpp` wires:

- `TemplateCode` — display, touch, LVGL
- `SensorManager` — DHT11 polling
- `MainInterface` — dashboard and navigation
- `PeriodicScheduler` — non-blocking tasks

## Source modules

```
src/
├── main.cpp
├── TemplateCode.{h,cpp}      # Display/touch/LVGL (multi-board)
├── DisplayConfig.h           # Resolution & rotation per MODEL_* macro
├── UiLayout.h                # UI dimensions scale with resolution
├── MainInterface.{h,cpp}     # Dashboard, settings menu, screen routing
├── WiFiSettingsScreen.*      # Scan, SSID picker, keyboard, connect
├── TouchConfigScreen.*       # Touch test + resistive calibration UI
├── WiFiConnectionManager.*   # Non-blocking WiFi state machine
├── SettingsStore.*           # NVS: WiFi + touch cal
├── SensorManager.*           # DHT11
├── PeriodicScheduler.*       # Timed callbacks
└── RGBledDriver.*            # CYD RGB LED (stub on boards without LED)
```

## Multi-board strategy

Each PlatformIO env defines `-DMODEL_*` which selects code paths in `TemplateCode`:

| Boards | Display stack | Touch |
|--------|---------------|-------|
| 2432S028R, JC2432W328R/C | TFT_eSPI ST7789 | XPT2046 or bb_captouch |
| JC4827W543R | Arduino_GFX NV3041A QSPI | XPT2046 separate SPI |

`DisplayConfig.h` sets `SCREEN_WIDTH/HEIGHT`. `UiLayout.h` derives header, keyboard, and padding sizes.

## Build pipeline

```
platformio.ini env
    → scripts/copy_template.py (pre-build)
    → template files/User_Setup.h → .pio/libdeps/<env>/TFT_eSPI/
    → template files/lv_conf.h → .pio/libdeps/<env>/
    → compile src/
```

JC4827 env skips TFT_eSPI — copy script logs skip when library absent.

## LVGL integration

- Single partial buffer: `SCREEN_WIDTH * SCREEN_HEIGHT / 10`
- `flushDisplay` → TFT_eSPI `pushColors` or Arduino_GFX canvas bitmap
- Pointer input from `readTouchpad` → unified `TouchSample` struct

## Upstream template

Display/touch/LVGL foundation from [cyd-lvgl-template](https://github.com/Deejpotter/cyd-lvgl-template). This repo adds sensor UI, WiFi, settings, and JC4827 support on top.

```bash
git remote add upstream https://github.com/Deejpotter/cyd-lvgl-template.git
git fetch upstream
git merge upstream/main
```

Resolve conflicts carefully — app code lives in `MainInterface`, `SettingsStore`, etc.

## Docs site

Interactive documentation lives in `docs-site/` (VitePress SSG). References are curated in `.vitepress/data/references.ts`.

```bash
cd docs-site
npm install
npm run dev      # local preview
npm run build    # static output → docs-site/.vitepress/dist
```

Board metadata: `boards.ts`. Gotchas: `gotchas.ts`. References: `references.ts` — update all three when adding a new env.
