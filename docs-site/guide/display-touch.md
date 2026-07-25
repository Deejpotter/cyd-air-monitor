# Display & Touch

CYD-family boards share concepts but **different envs** — display driver, rotation, and touch mapping all vary.

## Configuration layers

```
platformio.ini build flags
        ↓
DisplayConfig.h  (resolution, rotation defaults)
        ↓
TemplateCode     (TFT_eSPI or Arduino_GFX + touch init)
        ↓
SettingsStore    (runtime touch cal from NVS, resistive only)
```

## Quick reference matrix

| Env | Panel | Driver | Size | Touch | Display rot | Touch setup |
|-----|-------|--------|------|-------|-------------|-------------|
| `2432s028r` | ST7789 | TFT_eSPI | 320×240 | XPT2046 | 1 + MADCTL 0x20 | rot 1, mirror X+Y |
| `jc2432w328r` | ST7789 | TFT_eSPI | 320×240 | XPT2046 | 3 | rot 1, mirror X+Y |
| `jc2432w328c` | ST7789 | TFT_eSPI | 320×240 | CST820 I²C | 1 | native portrait → landscape |
| `jc4827w543r` | NV3041A | Arduino_GFX QSPI | 480×272 | XPT2046 | 0 | rot 0, no mirror |

## JC4827W543R external connectors

Rear silkscreen (JST 1.25 mm unless noted). Full table on the [JC4827W543R board page](/boards/jc4827w543r#connectors).

| Connector | Pins (silkscreen → GPIO) |
|-----------|--------------------------|
| **P2** | IO46→46, IO9→9, IO14→14, IO5→5 (general-purpose GPIO) |
| **P3** | IO6→6, IO7→7, IO15→15, IO16→16 (often used for I²S / speaker) |
| **P4** | GND, 3.3V, IO17→17 (**DHT11 data**), IO18→18 (free / I²C SCL) — **JST 1.25 mm** |
| **P5** | Same signals as P4 — **4-pin 2.54 mm male header** (dupont-friendly, not JST) |
| **P1** | +5V, RXD→44, TXD→43, GND (UART0 — USB-C CDC used for flash/serial) |
| **P6** | BAT+, BAT− (Li-ion via IP5306 charger) |
| **P7** | Speaker (NS4168 amp) |
| **LCD1** | Wide **FFC/FPC** — factory flex to the 4.3″ LCD + touch assembly |
| **FPC1** | Small **FFC/FPC** at top edge — auxiliary flex (often unused; revision-specific) |
| **TF1** | microSD slot (CS ≈ GPIO 10) |
| **USB1** | USB-C power + native CDC (`/dev/ttyACM0`) |
| **SW1** | Onboard user button |
| **S1–S4** | Unpopulated button solder pads |

<BoardMatrix />

## Key concept: display ≠ touch rotation

Using the same rotation number for display and touch often causes **one axis flipped**. Tune them independently:

- **Display** — `TFT_DISPLAY_ROTATION`, `TFT_MADCTL_OVERRIDE` (ST7789 only)
- **Touch** — `TOUCH_ROTATION`, `TOUCH_MIRROR_X`, `TOUCH_MIRROR_Y`

## ST7789 MADCTL (2432S028R)

| Rotation | MADCTL | Symptom |
|----------|--------|---------|
| 1 | MX + MV | Long axis OK, **short axis flipped** |
| 3 | MY + MV | Short axis OK, **long axis flipped** |

Fix: `TFT_DISPLAY_ROTATION=1` + `TFT_MADCTL_OVERRIDE=0x20` (MV only).

## Resistive XPT2046

Build flags in `platformio.ini`:

- `TOUCH_X_MIN/MAX`, `TOUCH_Y_MIN/MAX` — raw ADC range
- `TOUCH_ROTATION` — passed to `XPT2046_Touchscreen::setRotation()`
- `TOUCH_MIRROR_X/Y` — applied after linear map

Runtime calibration via **Settings → Touch Test → Save Cal** writes to NVS.

## Capacitive CST820 (JC2432W328C)

Native portrait coords with landscape display:

```
screen_x = native_y
screen_y = 239 - native_x
```

Set `-DCAP_TOUCH_NATIVE_LANDSCAPE=1`.

I²C: SDA=33, SCL=32, RST=25, INT=21 (or **36** on some revisions).

## JC4827W543R (4.3" S3)

- **Not ST7789** — uses `Arduino_ESP32QSPI` + `Arduino_NV3041A`
- Canvas buffer flushed after `lv_timer_handler()`
- Native 480×272 landscape — UI scales via `UiLayout.h`
- Separate touch SPI: CS=38, IRQ=3, MOSI/MISO/CLK=11/13/12

## Tuning workflow

1. Flash the closest env from [Board Explorer](/boards/)
2. If **display** wrong on one axis → MADCTL or display rotation
3. If **touch** wrong → Touch Test cal (resistive) or mirror flags
4. Rebuild for compile-time flag changes — no need to edit `.pio/`

## Legacy markdown reference

The repo also ships [docs/DISPLAY_CONFIG.md](https://github.com/Deejpotter/cyd-air-monitor/blob/main/docs/DISPLAY_CONFIG.md) with community notes and links.
