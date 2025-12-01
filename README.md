# CYD Air Monitor

## Overview

This project implements a temperature and humidity monitor for the JC2432W328R (Cheap Yellow Display) ESP32 board using a DHT11 sensor. The display shows real-time temperature and humidity readings in a card-based UI design.

The user interface is built using LVGL with custom card components featuring color-coded displays (warm orange for temperature, cool blue for humidity). The code is uploaded to the CYD using PlatformIO.

### Key Features

- DHT11 sensor integration on GPIO21
- Card-based UI with temperature and humidity displays
- Custom bit-bang DHT11 protocol implementation
- Color-coded visual design
- 2-second update interval for sensor readings

## Hardware Requirements

- JC2432W328R ESP32 Board
  - 2.8-inch TFT display (320x240 resolution)
  - ST7789 V2 display driver
  - ESP32-WROOM microcontroller
  - Built-in USB-C connector
  - Resistive touch screen

- DHT11 Temperature/Humidity Sensor
  - Connected to GPIO21 (DATA pin)
  - VCC: 3.3V or 5V
  - GND: Ground
  - 10kΩ pull-up resistor recommended (some modules have built-in)

- JC2432W328C ESP32 Board
  - 2.8-inch TFT display (320x240 resolution)
  - ST7789 V2 display driver
  - ESP32-WROOM microcontroller
  - Built-in USB-C connector
  - Capacitive touch screen (CST820 controller)

- JC4827W543R ESP32 Board
  - 4.82-inch TFT display (540x480 resolution)
  - ST7789 V2 display driver
  - ESP32-WROOM microcontroller
  - Built-in USB-C connector

## Development Environment Setup

## Required Software

- Visual Studio Code
- PlatformIO IDE Extension
- LVGL

## Libraries

```ini
lib_deps = 
   bodmer/TFT_eSPI@^2.5.42
   lvgl/lvgl@^8.3.6
```

```txt
project/
├── src/
│   ├── main.cpp              # Main program logic
│   ├── MainInterface.h       # UI class definition
│   ├── MainInterface.cpp     # Card-based UI implementation
│   ├── EnvSensor.h          # DHT11 sensor interface
│   ├── EnvSensor.cpp        # Custom DHT11 bit-bang protocol
│   ├── TemplateCode.h       # Display/LVGL initialization
│   ├── TemplateCode.cpp     # Display driver glue code
│   └── RGBledDriver.*       # RGB LED support
├── template files/          # Config templates
│   ├── lv_conf.h           # LVGL configuration
│   └── jc2432w328r/
│       └── User_Setup.h    # TFT_eSPI pin configuration
├── scripts/
│   └── copy_template.py    # Pre-build config copy script
├── platformio.ini          # PlatformIO configuration
└── README.md
```

## Display Pin Configurations

### JC2432W328R

```cpp
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  -1
#define TFT_BL   27
// Touch (resistive)
#define TOUCH_CS 33
#define TOUCH_IRQ 39
```

> Note: The pinout above is confirmed for the JC2432W328R (resistive). Some online sources mention pins 21 and 27 may be switched on other models; this template uses the configuration from the [env:jc2432w328r] in `platformio.ini`.

### JC2432W328C

```cpp
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  -1
#define TFT_BL   27
// Touch (capacitive, CST820)
#define I2C_SDA 33
#define I2C_SCL 32
#define TOUCH_INT 21
```

> Note: The pinout above is typical for the JC2432W328C (capacitive, CST820). The CST820 touch controller uses I2C. Confirm your board's silkscreen or schematic if you encounter issues.

### JC4827W543R

## Models and Orientation

This template supports both CYD variants with a unified display pipeline. The only difference is the touch interface:

- JC2432W328R (resistive): XPT2046 over SPI
- JC2432W328C (capacitive): CST820 over I2C

Model selection and orientation are controlled via build flags per environment in `platformio.ini`:

- `MODEL_JC2432W328R` or `MODEL_JC2432W328C` (set by the selected environment)
- `TFT_ROTATION` selects the screen orientation for both display and touch

Rotation values (same as TFT_eSPI):

- 0: Portrait (USB at bottom)
- 1: Landscape CW
- 2: Portrait 180°
- 3: Landscape CCW

Defaults in this repo:

- `env:jc2432w328r`: `-DMODEL_JC2432W328R -DTFT_ROTATION=3` (working landscape)
- `env:jc2432w328c`: `-DMODEL_JC2432W328C -DTFT_ROTATION=0` (portrait)

Change orientation by editing `TFT_ROTATION` in the environment you build.

## Building and Flashing

### 1. Clone the repository

```bash
git clone git@github.com:Makerstore/actuator-controller.git
```

### 2. Open in VS Code with PlatformIO

Open the project in Visual Studio Code with the PlatformIO extension installed.

Templates are copied automatically before build.

The pre-build script `scripts/copy_template.py` copies the needed files from `template files/` into `.pio/libdeps/<env>/`:

- `template files/lv_conf.h` -> `.pio/libdeps/<env>/lv_conf.h`
- `template files/<env>/User_Setup.h` -> `.pio/libdeps/<env>/TFT_eSPI/User_Setup.h`

You don’t need to move these files manually.

### 3. Build the project

```bash
pio run
```

### 4. Upload to the device

```bash
pio run --target upload
```

## UI Modifications

The user interface is built using the provided template files. To modify or extend the UI, edit the template source files in the `src/` directory. Implement any new event handlers or logic in your own `.cpp` files as needed.

### Important Notes

- All custom logic should go in `main.cpp` or separate files
- Keep the main loop running smoothly for responsive UI
- No scheduler or sensor manager are included by default; the loop is kept minimal

## Troubleshooting

Common issues and solutions:

### Display Issues

Ensure correct display driver (ST7789_2) is selected
Verify pin configurations match the hardware

### Build Issues

Check PlatformIO.ini configuration
Verify library versions are compatible

### DHT11 Sensor Issues

If sensor reads fail or show incorrect values:

- Verify DHT11 DATA pin connected to GPIO21
- Check VCC connected to 3.3V or 5V (DHT11 works with both)
- Ensure GND is connected
- Add 10kΩ pull-up resistor between DATA and VCC if not using module with built-in resistor
- DHT11 requires minimum 1-2 second interval between reads
- Custom implementation uses interrupt-disabled bit-bang protocol with checksum tolerance (±1 bit)

## DHT11 Implementation Details

This project uses a custom DHT11 implementation rather than a library:

### Protocol Overview

1. Start signal: Pull DATA low for 18ms, then high for 40µs
2. Wait for DHT11 response (80µs low, 80µs high)
3. Read 40 bits: Each bit starts with 50µs low, then high duration determines value
   - 26-28µs high = 0
   - 70µs high = 1
4. Validate checksum: sum of first 4 bytes should equal 5th byte (±1 tolerance)

### Data Format

- Byte 0: Humidity integer
- Byte 1: Humidity decimal (always 0 for DHT11)
- Byte 2: Temperature integer
- Byte 3: Temperature decimal (always 0 for DHT11)
- Byte 4: Checksum

### Special Handling

- Interrupts disabled during read for timing accuracy
- Bit 39 (last bit) uses delay-based timing instead of waiting for pin LOW
- Checksum allows ±1 bit error tolerance
- 2-second minimum interval enforced between reads

### Known Issues

- GPIO21 may conflict with I2C on some CYD variants (used as SDA)
- If reads consistently fail, try different GPIO pin (e.g., GPIO4, GPIO22, GPIO25)
- Bit-bang timing sensitive to CPU load; avoid heavy processing during reads

## Resources

### JC2432W328R links

- Almost correct pinout (pins 21 and 27 are switched): [ESP32 Cheap Yellow Display (CYD) Pinout (ESP32-2432S028R) | Random Nerd Tutorials](https://randomnerdtutorials.com/esp32-cheap-yellow-display-cyd-pinout-esp32-2432s028r/#speaker)
- This specific model of device (not quite accurate): <https://github.com/maxpill/JC2432W328>
- Squareline: <https://squareline.io/downloads>
- Cheap yellow display github: <https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/tree/main>
- Link to Squarline CYD template: <https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/discussions/102>
- Reddit discussion also for this specific model of device: <https://www.reddit.com/r/esp32/comments/1dy5k11/working_cyd_jc2432w328_display_240x320_28_usbc/?rdt=34968>
- ESP32 Marauder CYD port: maxpill/ESP32-Marauder-JC2432W328-Cheap-Yellow-Display: JC2432W328 ESP32Marauder Cheap Yellow Display
- Case 1: [GUITION JC2432W328C cover by Matej's Workshop | Download free STL model | Printables.com](https://www.printables.com/model/913023-guition-jc2432w328c-cover/files)
- Case 2: [ESP32 2.8inch JC2432W328 Case by GrafMax17 - Thingiverse](https://www.thingiverse.com/thing:6892431)

### JC2432W328C links

- Official/Community documentation: [maxpill/JC2432W328 GitHub](https://github.com/maxpill/JC2432W328)
- Community discussion and working code: [Reddit: Working CYD JC2432W328 Display 240x320 2.8" USB-C](https://www.reddit.com/r/esp32/comments/1dy5k11/working_cyd_jc2432w328_display_240x320_28_usbc/)
- Product listing and specs: [AliExpress JC2432W328C](https://aliexpress.com/item/1005006729707613.html)
- ST7789 display driver datasheet: [ST7789 PDF](https://www.rhydolabz.com/documents/33/ST7789.pdf)
- CST820 touch controller info: [CST820 datasheet (PDF)](https://datasheet.lcsc.com/lcsc/1811141810_FocalTech-Systems-CST820S_C181837.pdf)

### JC4827W543R links

## How template files are applied

A pre-build script copies the needed files automatically:

- `template files/lv_conf.h` -> `.pio/libdeps/<env>/lv_conf.h`
- `template files/<env>/User_Setup.h` -> `.pio/libdeps/<env>/TFT_eSPI/User_Setup.h`

No manual moves are required.
