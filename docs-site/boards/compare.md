# Compare boards

<BoardMatrix />

## Driver stack

| Env | MCU | Panel | Display library | Touch bus |
|-----|-----|-------|-----------------|-----------|
| `2432s028r` | ESP32 | ST7789 320×240 | TFT_eSPI | XPT2046 **separate SPI** |
| `jc2432w328r` | ESP32 | ST7789 320×240 | TFT_eSPI | XPT2046 **shared SPI** |
| `jc2432w328c` | ESP32 | ST7789 320×240 | TFT_eSPI | CST820 **I²C** |
| `jc4827w543r` | ESP32-S3 | NV3041A 480×272 | **Arduino_GFX QSPI** | XPT2046 separate SPI |

## DHT11 data pin

| Env | GPIO |
|-----|------|
| `2432s028r` | 27 |
| `jc2432w328r` | 21 |
| `jc2432w328c` | 22 |
| `jc4827w543r` | 17 (P4 IO17) |

## What breaks if you mix them up

| Mistake | Symptom |
|---------|---------|
| `jc2432w328r` on capacitive board | Display may work, **touch dead** |
| `jc2432w328c` on resistive board | **Touch dead** |
| Any TFT_eSPI env on JC4827 | **Blank display** |
| 2432S028R env on JC2432R | Wrong rotation, wrong DHT pin |
| Same number for display + touch rotation | **One axis flipped** |

See [Gotchas](/boards/gotchas) for fixes. [References →](/boards/references)
