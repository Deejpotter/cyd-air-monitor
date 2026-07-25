# Display and touch configuration

> **Interactive version:** [Board Explorer](https://deejpotter.github.io/cyd-air-monitor/boards/) and [Display & Touch guide](https://deejpotter.github.io/cyd-air-monitor/guide/display-touch) in the docs site (`docs-site/` — run `npm run dev` locally).

CYD boards share the same ST7789 panel size (240×320 native portrait) but differ in
backlight pin, touch controller, and how the panel/touch are wired. **Display rotation
and touch rotation are independent** — using the same number for both often causes
one axis to appear flipped.

## Quick reference

| Env | Board | Touch | Display `setRotation` | MADCTL override | Touch setup |
|-----|-------|-------|----------------------|-----------------|-------------|
| `2432s028r` | ESP32-2432S028R (dual USB) | XPT2046 SPI | **1** | **0x20** (MV only) | rot **1**, mirror X+Y |
| `jc2432w328r` | JC2432W328R resistive | XPT2046 (shared SPI) | **3** | — | rot **1**, mirror X+Y |
| `jc2432w328c` | JC2432W328C capacitive | CST820/CST816 I²C | **1** | — | native portrait → landscape map |
| `jc4827w543r` | JC4827W543R (Guition 4.3") | XPT2046 SPI | **2** | — | rot **0**, mirror **X** |

Build with the env that matches your hardware:

```bash
pio run -e 2432s028r --target upload
pio run -e jc2432w328r --target upload
pio run -e jc2432w328c --target upload
```

## Why rotation 1 and 3 both look wrong (2432S028R)

ST7789 `setRotation()` sets MADCTL mirror bits as well as swap:

| Rotation | MADCTL bits | Landscape size | Typical symptom on 2432S028R |
|----------|-------------|----------------|------------------------------|
| 1 | MX + MV | 320×240 | Long axis OK, **short axis flipped** |
| 3 | MY + MV | 320×240 | Short axis OK, **long axis flipped** |

Fix: keep `TFT_DISPLAY_ROTATION=1` and set `TFT_MADCTL_OVERRIDE=0x20` (MV only, no MX/MY).
This is applied after `setRotation()` in `TemplateCode.cpp`.

## Resistive touch (XPT2046)

Configured via `platformio.ini`:

- `TOUCH_ROTATION` — passed to `XPT2046_Touchscreen::setRotation()`
- `TOUCH_X_MIN/MAX`, `TOUCH_Y_MIN/MAX` — raw ADC calibration
- `TOUCH_MIRROR_X`, `TOUCH_MIRROR_Y` — flip mapped coordinates

Random Nerd Tutorials and the Arduino forum note: if touch feels upside-down, try
`touchscreen.setRotation(3)` instead of `1` **without** changing display rotation.

## Capacitive touch (JC2432W328C, CST820/CST816)

The chip reports coordinates in **native portrait** (240×320). With display
`setRotation(1)` (landscape 320×240), use the empirically validated transform
(from [kmendelev/jc2432w328c-examples](https://github.com/kmendelev/jc2432w328c-examples)):

```
screen_x = native_y
screen_y = 239 - native_x
```

Enable with `-DCAP_TOUCH_NATIVE_LANDSCAPE=1` (default for `jc2432w328c` env).

`bb_captouch` `setOrientation()` rotates the opposite direction to the panel on some
odd rotations (see [LovyanGFX #711](https://github.com/lovyan03/LovyanGFX/issues/711)).
The native landscape map avoids that class of mismatch.

### Capacitive troubleshooting

| Symptom | Things to try |
|---------|----------------|
| No touch at all | Check I²C: SDA=33, SCL=32, RST=25. Some boards use INT=21, others INT=36 — set `CST820_INT` in `platformio.ini`. |
| Touch rotated 90° | Confirm `CAP_TOUCH_NATIVE_LANDSCAPE=1` and display rot 1. |
| Touch mirrored on one axis | Add `-DTOUCH_MIRROR_X=1` or `-DTOUCH_MIRROR_Y=1`. |
| Wrong colours | Try `-DTFT_RGB_ORDER=TFT_BGR` in env (some ST7789V3 panels). |

## Runtime touch calibration (in-app)

The firmware includes a **Settings → Touch Test** screen:

| Board type | What the screen shows | Runtime tuning |
|------------|----------------------|----------------|
| Resistive (2432S028R, JC2432W328R) | Raw ADC, mapped X/Y, touch dot | Adjust `Xmin/Xmax/Ymin/Ymax` via +/- buttons; **Save Cal** stores to NVS |
| Capacitive (JC2432W328C) | Panel coords, mapped X/Y, touch dot | Diagnostic only — chip is pre-calibrated; use `TOUCH_MIRROR_X/Y` or `CST820_INT` in `platformio.ini` |

Saved resistive calibration overrides compile-time `TOUCH_X_MIN/MAX` defaults on boot.

### Community best practices (summary)

**Resistive XPT2046 (2432S028R, JC2432W328R)**

- Always calibrate — raw ADC ranges vary by panel and wiring ([Random Nerd Tutorials](https://randomnerdtutorials.com/esp32-cheap-yellow-display-cyd-resistive-touchscreen-calibration/), [LVGL forum](https://forum.lvgl.io/t/inconsistency-between-display-rotation-and-touchscreen-coordinate-rotation/20242)).
- Simple `map(raw, min, max, 0, width-1)` works for many CYD boards; advanced 6-point alpha/beta/delta transforms ([CF20852 calibration sketch](https://github.com/CF20852/ESP32-2432S028-Touchscreen-Calibration)) handle skewed panels better but need a dedicated calibration flow.
- **Display rotation and touch rotation are independent** — fix display with `TFT_DISPLAY_ROTATION` / `TFT_MADCTL_OVERRIDE`, then tune touch with `TOUCH_ROTATION` and `TOUCH_MIRROR_X/Y` without changing display rotation.
- Use conservative SPI touch clock (~2.5 MHz); 2432S028R uses a **separate SPI bus** for touch (GPIO 32/39/25 + CS 33).
- Mirror both axes is common on CYD resistive boards after mapping ([lvgl_micropython CYD discussion](https://github.com/lvgl-micropython/lvgl_micropython/discussions/281)).

**Capacitive CST820/CST816 (JC2432W328C)**

- I²C: SDA=33, SCL=32; RST=25; INT=21 or **36** depending on board revision.
- Use `bb_captouch` with `getSamples()` — auto-detects CST820 ([bitbank2/bb_captouch](https://github.com/bitbank2/bb_captouch)).
- Chip reports native portrait coords; with display rot 1 use `screen_x = native_y`, `screen_y = 239 - native_x` (`CAP_TOUCH_NATIVE_LANDSCAPE=1`, from [kmendelev/jc2432w328c-examples](https://github.com/kmendelev/jc2432w328c-examples)).
- Prefer native landscape map over `setOrientation()` — LovyanGFX #711 notes orientation API can invert vs panel on odd rotations.
- Some vendor guides run CST820 with RST/INT disabled (`-1`); this project uses RST=25, INT=21 with fallback note for INT=36.

**Runtime vs compile-time**

- Compile-time flags in `platformio.ini` are the baseline per env.
- Resistive min/max can be tuned at runtime and saved to NVS via the Touch Test screen.
- Mirror/rotation flags remain compile-time (requires rebuild) — community consensus is these are board-specific and rarely changed in the field.

## Tuning a new board

1. Pick the closest env and flash it.
2. If **display** is wrong on one axis only, try `TFT_MADCTL_OVERRIDE` (see ST7789 MADCTL: MV=0x20, MX=0x40, MY=0x80).
3. If **touch** is wrong, adjust `TOUCH_MIRROR_X/Y` or `TOUCH_ROTATION` without changing display rotation.
4. Rebuild — no need to edit `.pio/` files.

## References

- [Random Nerd Tutorials CYD pinout](https://randomnerdtutorials.com/cheap-yellow-display-esp32-2432s028r/)
- [witnessmenow ESP32-Cheap-Yellow-Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display)
- [kmendelev JC2432W328C examples](https://github.com/kmendelev/jc2432w328c-examples)
- [boxwrench CYD reference](https://github.com/boxwrench/DIST_ESP32-CYD-Tester/blob/main/docs/ESP32_CYD_REFERENCE.md)
