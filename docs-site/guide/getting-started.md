# Getting Started

CYD Air Monitor turns a **Cheap Yellow Display** (or compatible Guition board) into a tabletop temperature and humidity monitor with WiFi settings and touch calibration.

## Requirements

- [VS Code](https://code.visualstudio.com/) + [PlatformIO](https://platformio.org/) extension
- A supported board — use the [Board Explorer](/boards/) if unsure
- DHT11 sensor (3.3 V, GND, data to board-specific GPIO)

## 1. Clone and open

```bash
git clone https://github.com/Deejpotter/cyd-air-monitor.git
cd cyd-air-monitor
```

Open the folder in VS Code. PlatformIO will detect `platformio.ini`.

## 2. Pick your environment

Each board has a PlatformIO env. **Wrong env = blank display or dead touch.**

| Your hardware | Command |
|---------------|---------|
| Original CYD (2432S028R) | `pio run -e 2432s028r --target upload` |
| JC2432 resistive | `pio run -e jc2432w328r --target upload` |
| JC2432 capacitive | `pio run -e jc2432w328c --target upload` |
| Guition 4.3" JC4827 | `pio run -e jc4827w543r --target upload` |

Or change `default_envs` in `platformio.ini`.

::: tip Interactive picker
The [Board Explorer](/boards/) generates copy-paste commands and shows pin maps for your exact board.
:::

## 3. First build notes

On first build, `scripts/copy_template.py` copies `template files/` into `.pio/libdeps/<env>/`:

- `User_Setup.h` → TFT_eSPI library (ST7789 boards only)
- `lv_conf.h` → LVGL config

**Do not edit files under `.pio/`** — change templates in `template files/` and rebuild.

## 4. Wire the DHT11

| Board env | DHT11 data pin |
|-----------|----------------|
| `2432s028r` | GPIO 27 |
| `jc2432w328r` | GPIO 21 |
| `jc2432w328c` | GPIO 22 |
| `jc4827w543r` | GPIO 17 (P4 IO17) |

VCC → 3.3 V, GND → GND (both on **P4**). Data → **P4 IO17**.

For **JC4827W543R**, use **one sensor at a time** on **P4** (GND, 3.3 V, IO17, IO18):

| Sensor | P4 wiring | Firmware (`src/SensorConfig.h`) |
|--------|-----------|----------------------------------|
| **DHT11** (default) | IO17 → data | `SENSOR_USE_DHT = true`, `SENSOR_USE_BME680 = false` |
| **BME680** | IO17 → SDA, IO18 → SCL | flip the flags above, reflash |

Only one pin role at a time — IO17 is DHT data **or** I2C SDA, not both.

## 5. Serial monitor

```bash
pio device monitor
```

Baud **115200**. Boot logs show touch init and WiFi auto-connect attempts.

## 6. Use the UI

- **Dashboard** — live temp/humidity, WiFi status, gear icon for settings
- **Settings → WiFi** — scan, connect, forget
- **Settings → Touch Test** — coordinates and resistive calibration

See [Settings & WiFi](/guide/settings) and [Display & Touch](/guide/display-touch) for details.

## Upload permissions (Linux)

If upload or `pio device monitor` fails with **Permission denied** on `/dev/ttyUSB*` or `/dev/ttyACM*`:

```bash
sudo usermod -aG dialout $USER
```

Then **open a new terminal** (or log out and back in). Existing shells keep the old group list until then.

**Works immediately in the current shell:**

```bash
newgrp dialout
pio device monitor -p /dev/ttyACM0 -b 115200
```

**Upload (JC4827 example):**

```bash
pio run -e jc4827w543r --target upload --upload-port /dev/ttyACM0
```

If you must use sudo, use the full path (root may not have `pio` on PATH):

```bash
sudo /usr/local/bin/pio device monitor -p /dev/ttyACM0 -b 115200
```

## Next steps

- [Board Explorer](/boards/) — pin maps and env comparison
- [Troubleshooting](/guide/troubleshooting) — interactive symptom guide
- [Architecture](/guide/architecture) — how the firmware is structured
