# AI agent guide for cyd-air-monitor

This repo is a PlatformIO (Arduino/ESP32) LVGL project targeting the Cheap Yellow Display (CYD) boards:

- jc2432w328r: 2.8" 320x240, ST7789, resistive touch (XPT2046)
- jc2432w328c: 2.8" 320x240, ST7789, capacitive touch (CST820)

## Project Purpose

Temperature and humidity monitor using DHT11 sensor on GPIO21 with card-based UI.

Big picture

- LVGL + TFT_eSPI glue lives in `src/TemplateCode.{h,cpp}`. It initializes TFT, LVGL buffers/drivers, and (when enabled) touch.
- UI code lives in `src/MainInterface.{h,cpp}`. Card-based design with temperature (orange) and humidity (blue) displays. Keep it LVGL-only (no hardware logic).
- DHT11 sensor driver lives in `src/EnvSensor.{h,cpp}` with custom bit-bang protocol implementation.
- App entry is `src/main.cpp`: creates/initializes TemplateCode, UI, and EnvSensor.
- Per-board config is driven by PlatformIO environments in `platformio.ini` and the TFT_eSPI `User_Setup.h` copied by a pre-build script.

Critical workflows

- Build:
  - Default env: `jc2432w328r`. Use: `platformio run -e jc2432w328r` or `-e jc2432w328c`.
- Upload:
  - `platformio run -e jc2432w328r --target upload`
- First build sets up libs:
  - A pre:extra script `scripts/copy_template.py` copies template files into `.pio/libdeps/<env>/`:
    - `template files/<env>/User_Setup.h` → `.pio/libdeps/<env>/TFT_eSPI/User_Setup.h`
    - `template files/lv_conf.h` → `.pio/libdeps/<env>/lv_conf.h`
  - Do not edit files under `.pio/`; edit templates in `template files/` instead.
- Reset/clean:
  - To fully refresh libdeps after template changes: delete `.pio/` or run `git clean -fdx`, then rebuild.

Display and orientation

- TFT, LVGL, and touch must agree on rotation and resolution (320x240 or 240x320):
  - TFT_eSPI rotation is typically controlled in the copied `User_Setup.h`.
  - Template glue sets LVGL resolution from `tft.width()/tft.height()` after rotation to keep them in sync.
- If the layout does not fill the screen: check rotation and `User_Setup.h` for the active env; rebuild so the pre-script recopies configs.

Project-specific patterns

- Hardware abstraction:
  - Keep pin and device-specific logic in `src/drivers/` and system glue in `src/system/`.
  - Keep UI logic in `src/ui/` and pass data via clean interfaces (e.g., `MainInterface::setTemperature`).
- Sensors:
  - Use `SensorManager` with `PeriodicScheduler` for non-blocking updates; call `onChange(...)` to notify UI.
- Touch type selection:
  - Envs typically define one of the macros in `platformio.ini` that the code may use (`TOUCH_TYPE_RESISTIVE` / `TOUCH_TYPE_CAPACITIVE`). Ensure it matches the selected environment and templates.

External deps & integration points

- TFT_eSPI: configured by `User_Setup.h` (pins, driver, SPI frequencies). File is provided per env under `template files/<env>/` and injected into `.pio/libdeps` by the pre-script.
- LVGL: configured via `template files/lv_conf.h` and injected similarly. LVGL display driver is wired in `TemplateCode.cpp`.
- XPT2046 (resistive) / CST820 (capacitive): drivers live under `src/drivers/` (CST820 custom), initialized in `TemplateCode.cpp` when enabled.

Conventions for contributions

- Do not modify files in `.pio/libdeps/...`; change `template files/` or `platformio.ini` and rebuild to propagate configs.
- Prefer adding new hardware modules in `src/drivers/` and expose simple methods that UI or system glue can call.
- Keep `main.cpp` minimal: initialize TemplateCode, initialize UI, wire sensors/scheduler, and call `update()` regularly.

Examples

- Add a new sensor: put driver wrapper in `src/sensors/`, schedule periodic `update()` with `PeriodicScheduler`, and update UI by calling `MainInterface` setters.
- Change display rotation: update the env’s `template files/<env>/User_Setup.h` rotation/pin definitions as used by TFT_eSPI, clean `.pio/`, rebuild.

Devices & developer setup (current context)

- OS: Windows, VS Code with PlatformIO.
- Terminals used: PlatformIO build/upload tasks per env, sometimes manual `platformio run` in bash.
- Boards used: jc2432w328r (resistive) and jc2432w328c (capacitive).

Quick file map

- `platformio.ini`: environments and build flags; pre-build script configured here.
- `scripts/copy_template.py`: copies template display configs into libdeps for the active env.
- `template files/`: source-of-truth configs for TFT_eSPI (`User_Setup.h`) and LVGL (`lv_conf.h`), per env if needed.
- `src/TemplateCode.*`: LVGL + TFT glue and (optional) touch input glue.
- `src/MainInterface.*`: LVGL UI with card-based temperature/humidity display. Keep hardware-agnostic.
- `src/EnvSensor.*`: DHT11 sensor driver with custom bit-bang protocol implementation.

## DHT11 Sensor Implementation (CRITICAL)

**Current sensor implementation uses custom bit-bang protocol** in `src/EnvSensor.cpp`:

### Protocol Details

1. **Start signal**: Pull GPIO21 LOW for 18ms, then HIGH for 40µs
2. **Wait for response**: DHT11 pulls low (80µs), then high (80µs)
3. **Read 40 bits**: Each bit has 50µs LOW, then HIGH duration determines value:
   - 26-28µs HIGH = bit 0
   - 70µs HIGH = bit 1
4. **Validate checksum**: byte[0] + byte[1] + byte[2] + byte[3] should equal byte[4] (±1 tolerance)

### Data Format

- Byte 0: Humidity integer (DHT11 has no decimal)
- Byte 1: Humidity decimal (always 0)
- Byte 2: Temperature integer  
- Byte 3: Temperature decimal (always 0)
- Byte 4: Checksum

### Critical Implementation Details

- **Interrupts disabled** during entire read sequence for timing accuracy
- **Bit 39 special handling**: Uses `delayMicroseconds(60)` instead of polling for LOW (timing quirk)
- **Checksum tolerance**: Allows ±1 bit error to handle minor timing variations
- **Minimum interval**: 2-second delay enforced between reads
- **GPIO21 conflicts**: May interfere with I2C (SDA on some CYD variants)

### Troubleshooting

- If reads fail: verify wiring (VCC, GPIO21 DATA, GND, 10kΩ pull-up)
- If checksum errors persist: timing may be off due to CPU load
- If GPIO21 conflicts: try GPIO4, GPIO22, or GPIO25
- **DO NOT switch to SimpleDHT library** - it consistently fails with error 2832 on this hardware
- Custom implementation has proven reliable with checksum tolerance

### UI Integration

- `MainInterface::update()` calls `envSensor.read()` every 2 seconds
- `MainInterface::updateSensorDisplay()` formats values as integers (%.0f)
- Temperature card: orange border (0xFF6B35), warm background
- Humidity card: blue border (0x4A90E2), cool background
- Dark background: 0x1a1d29
- Font: lv_font_montserrat_28
- Shows "--°C" and "--%%" when sensor unavailable

Notes

- If behavior differs between envs, first verify the env-specific `User_Setup.h` actually matches the hardware and was copied (see build logs from the pre-script).
- If LVGL layout is clipped or scaled, the cause is almost always rotation/resolution mismatch between TFT_eSPI `User_Setup.h` and LVGL init; fix the template and rebuild.

MCP tools (if available)

If an MCP server is running in your workspace, prefer its tools for these actions:

- Build/upload (non-interactive):
  - Run: `platformio run -e jc2432w328r` (or `-e jc2432w328c`)
  - Upload: `platformio run -e jc2432w328r --target upload`
- Reset libdeps/config quickly:
  - `git reset --hard HEAD` then `git clean -fdx`, followed by a fresh build.
- Validate template copies:
  - Inspect build output for lines from `scripts/copy_template.py` confirming `User_Setup.h` and `lv_conf.h` were copied for the active env.
- File operations:
  - Read/Write project files via the editor APIs; do not modify `.pio/libdeps/*` directly—change `template files/` and rebuild.
- Git hygiene:
  - Use `git status`, `git diff`, and `git checkout -- <file>` to revert unintended edits before builds.

Provide tool IDs/commands if your MCP exposes named tools (e.g., run-command, git, list-files) so agents can call them directly. On Windows, prefer absolute paths (e.g., `C:\Users\...\cyd-air-monitor`).
