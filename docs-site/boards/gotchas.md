# Gotchas & pitfalls

Real bring-up issues for CYD-family boards. Filter by hardware and topic.

<BoardGotchas />

## Display vs touch rotation

::: warning Golden rule
**Fix display orientation first.** Then tune touch with `TOUCH_ROTATION` / `TOUCH_MIRROR_X/Y` / calibration — without changing display rotation again.
:::

| Board | Display trick | Touch trick |
|-------|---------------|-------------|
| 2432S028R | rot **1** + MADCTL **0x20** | rot 1, mirror X+Y, separate SPI |
| JC2432W328R | rot **3** | rot 1, mirror X+Y, shared SPI |
| JC2432W328C | rot **1** | portrait→landscape map, I²C INT=21 or **36** |
| JC4827W543R | rot **2** (try 0 if wrong) | rot 0, cal 350–3500, may mirror X |

## MADCTL cheat sheet

ST7789 (CYD 2.8") and NV3041A (JC4827) use different drivers but the same idea: `setRotation()` sets mirror bits you may not want.

| Flag | Value | Effect |
|------|-------|--------|
| MV | 0x20 | Swap X/Y (landscape) |
| MX | 0x40 | Mirror horizontal |
| MY | 0x80 | Mirror vertical |

**2432S028R:** rotation 1 applies MX+MV; override to **MV only (0x20)** fixes short-axis flip.

**JC4827W543R:** use `TFT_DISPLAY_ROTATION` via Arduino_GFX `setRotation()` — **do not** write raw MADCTL after init (desyncs the driver). Try rotation **2**; if one axis wrong, try **0**. See [Arduino_NV3041A source](https://github.com/moononournation/Arduino_GFX/blob/master/src/display/Arduino_NV3041A.cpp).

## References

<ReferencesList board-id="" />

More links: [full references index](/boards/references).

## Build mistakes

- Editing `.pio/libdeps/` — overwritten every build; use `template files/` and `platformio.ini`
- JC4827 with TFT_eSPI env — blank screen; needs Arduino_GFX
- JC4827 library name — `GFX Library for Arduino@1.4.9`, not `Arduino_GFX`
- Linux upload — add user to `dialout` or use `sudo pio run … --target upload`
- JC4827 port — usually `/dev/ttyACM0` (S3 USB CDC), not `ttyUSB0`

## Touch calibration

| Board type | In-app Touch Test | Compile-time |
|------------|-------------------|--------------|
| Resistive | Save min/max to NVS | `TOUCH_X/Y_MIN/MAX`, mirror flags |
| Capacitive | Diagnostic only | `CST820_INT`, `CAP_TOUCH_NATIVE_LANDSCAPE` |

## Further reading

- [References & links](/boards/references) — full curated index
- [DISPLAY_CONFIG.md](https://github.com/Deejpotter/cyd-air-monitor/blob/main/docs/DISPLAY_CONFIG.md) in the repo
- [ESP32-Cheap-Yellow-Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display) community hub
