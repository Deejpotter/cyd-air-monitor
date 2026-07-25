export type GotchaCategory = 'identity' | 'display' | 'touch' | 'build' | 'wiring' | 'pitfall'

export interface Gotcha {
  id: string
  boardIds: string[] | 'all'
  category: GotchaCategory
  title: string
  body: string
  fix?: string
  severity: 'critical' | 'common' | 'tip'
}

export const categoryLabels: Record<GotchaCategory, string> = {
  identity: 'Identify your board',
  display: 'Display',
  touch: 'Touch',
  build: 'Build & flash',
  wiring: 'Wiring',
  pitfall: 'Common mistake',
}

export const gotchas: Gotcha[] = [
  // Cross-board
  {
    id: 'wrong-env',
    boardIds: 'all',
    category: 'pitfall',
    title: 'Wrong PlatformIO env = blank display or dead touch',
    body: 'Every board has its own env. Flashing `jc2432w328r` on a capacitive panel (or `2432s028r` on a JC2432) is the #1 failure mode.',
    fix: 'Match silkscreen / model sticker to env in the Board Explorer before flashing.',
    severity: 'critical',
  },
  {
    id: 'display-not-touch-rot',
    boardIds: 'all',
    category: 'display',
    title: 'Display rotation ≠ touch rotation',
    body: 'Using the same rotation number for display and touch often flips one axis. Fix display first, then tune touch separately.',
    fix: 'Adjust `TFT_DISPLAY_ROTATION` / MADCTL for display; `TOUCH_ROTATION` and `TOUCH_MIRROR_X/Y` for touch — never change both together blindly.',
    severity: 'critical',
  },
  {
    id: 'edit-pio-not-template',
    boardIds: 'all',
    category: 'build',
    title: 'Do not edit files under `.pio/`',
    body: '`scripts/copy_template.py` overwrites TFT_eSPI and LVGL config every build.',
    fix: 'Change `template files/` or `platformio.ini` build flags, then rebuild.',
    severity: 'common',
  },
  {
    id: 'dht-pin-per-board',
    boardIds: 'all',
    category: 'wiring',
    title: 'DHT11 pin changes per board',
    body: 'GPIO 27 on original CYD, 21 on JC2432R, 22 on JC2432C, 17 on JC4827 (P4 IO17). NaN readings are often wrong pin.',
    severity: 'common',
  },
  {
    id: 'linux-dialout',
    boardIds: 'all',
    category: 'build',
    title: 'Linux upload needs dialout group',
    body: 'Permission denied on `/dev/ttyUSB*` or `/dev/ttyACM*` unless user is in `dialout`.',
    fix: '`sudo usermod -aG dialout $USER` then open a new terminal — or run `newgrp dialout` in the current shell.',
    severity: 'common',
  },

  // 2432S028R
  {
    id: '028r-madctl',
    boardIds: ['2432s028r'],
    category: 'display',
    title: 'Rotation 1 and 3 each flip a different axis',
    body: 'ST7789 `setRotation(1)` gives MX+MV (long axis OK, short flipped). Rotation 3 gives MY+MV (opposite). Neither alone is correct.',
    fix: 'Keep `TFT_DISPLAY_ROTATION=1` and set `TFT_MADCTL_OVERRIDE=0x20` (MV only, no MX/MY).',
    severity: 'critical',
  },
  {
    id: '028r-touch-spi',
    boardIds: ['2432s028r'],
    category: 'touch',
    title: 'Touch is on a separate SPI bus',
    body: 'Unlike JC2432R, the original CYD wires XPT2046 to GPIO 32/39/25 (MOSI/MISO/CLK) — not the display SPI pins.',
    severity: 'common',
  },
  {
    id: '028r-backlight',
    boardIds: ['2432s028r'],
    category: 'wiring',
    title: 'Backlight on GPIO 21 (not 27)',
    body: 'JC2432 boards use BL=27. The original 2432S028R uses BL=21. DHT11 is on 27.',
    severity: 'tip',
  },
  {
    id: '028r-resistive-cal',
    boardIds: ['2432s028r'],
    category: 'touch',
    title: 'Resistive calibration varies by panel',
    body: 'Raw ADC ranges differ. Compile-time defaults are a starting point only.',
    fix: 'Settings → Touch Test → adjust min/max → Save Cal (NVS).',
    severity: 'common',
  },

  // JC2432W328R
  {
    id: '328r-not-c',
    boardIds: ['jc2432w328r'],
    category: 'identity',
    title: 'R ≠ C — resistive vs capacitive',
    body: 'JC2432W328**R** has XPT2046 resistive touch. JC2432W328**C** has CST820 I²C capacitive. The letter on the box matters.',
    fix: 'Flash `jc2432w328r` only on resistive hardware.',
    severity: 'critical',
  },
  {
    id: '328r-display-rot3',
    boardIds: ['jc2432w328r'],
    category: 'display',
    title: 'Display rotation 3 (not 1 like original CYD)',
    body: 'Same ST7789 size as other CYD boards but different MADCTL wiring — uses rot 3, not the 2432S028R MADCTL hack.',
    severity: 'common',
  },
  {
    id: '328r-shared-spi',
    boardIds: ['jc2432w328r'],
    category: 'touch',
    title: 'Touch shares display SPI',
    body: 'Only TOUCH_CS=33 is needed; touch clock/data lines are shared with the panel SPI bus.',
    severity: 'tip',
  },

  // JC2432W328C
  {
    id: '328c-not-r',
    boardIds: ['jc2432w328c'],
    category: 'identity',
    title: 'Never flash the resistive env',
    body: 'Capacitive CST820 will not respond to XPT2046 drivers. Display may work; touch will not.',
    fix: 'Use `jc2432w328c` only.',
    severity: 'critical',
  },
  {
    id: '328c-portrait-map',
    boardIds: ['jc2432w328c'],
    category: 'touch',
    title: 'CST820 reports native portrait coords',
    body: 'Chip outputs 240×320 portrait values while the UI is 320×240 landscape.',
    fix: '`CAP_TOUCH_NATIVE_LANDSCAPE=1`: screen_x = native_y, screen_y = 239 − native_x. Do not rely on bb_captouch setOrientation() alone.',
    severity: 'critical',
  },
  {
    id: '328c-int-pin',
    boardIds: ['jc2432w328c'],
    category: 'touch',
    title: 'INT pin varies by board revision',
    body: 'Most builds use CST820_INT=21. Some revisions need INT=36.',
    fix: 'If touch is dead with correct env, change `CST820_INT` in platformio.ini and rebuild.',
    severity: 'common',
  },
  {
    id: '328c-touch-test-only',
    boardIds: ['jc2432w328c'],
    category: 'touch',
    title: 'Touch Test screen is diagnostic only',
    body: 'Capacitive chips are factory-calibrated. The in-app cal save buttons apply to resistive boards only.',
    severity: 'tip',
  },

  // JC4827W543R
  {
    id: '4827-not-tftespi',
    boardIds: ['jc4827w543r'],
    category: 'build',
    title: 'Not TFT_eSPI — uses Arduino_GFX QSPI',
    body: '4.3" NV3041A panel over QSPI (CS=45, SCK=47, D0–D3). Any ST7789 / TFT_eSPI env will not drive this display.',
    fix: 'Flash `jc4827w543r` with `moononournation/GFX Library for Arduino@1.4.9`.',
    severity: 'critical',
  },
  {
    id: '4827-usb-acm',
    boardIds: ['jc4827w543r'],
    category: 'build',
    title: 'Upload port is usually /dev/ttyACM0',
    body: 'ESP32-S3 native USB CDC (`ARDUINO_USB_CDC_ON_BOOT=1`), not CP2102 ttyUSB.',
    severity: 'common',
  },
  {
    id: '4827-madctl-short',
    boardIds: ['jc4827w543r'],
    category: 'display',
    title: 'NV3041A rotation — do not use raw MADCTL override',
    body: 'Rotation 0 (MX+MY) may show long axis OK and short axis flipped. Writing raw MADCTL after init desyncs Arduino_GFX and breaks both axes.',
    fix: 'Use TFT_DISPLAY_ROTATION only (try 2, then 0). See Arduino_NV3041A setRotation in the library source.',
    severity: 'critical',
  },
  {
    id: '4827-no-rgb',
    boardIds: ['jc4827w543r'],
    category: 'wiring',
    title: 'No onboard RGB LED',
    body: 'RGBledDriver stubs out silently. No boot colour feedback.',
    severity: 'tip',
  },
  {
    id: '4827-dht-p2',
    boardIds: ['jc4827w543r'],
    category: 'wiring',
    title: 'External connectors P1–P7 (rear silkscreen)',
    body: 'P2–P4, P6, P7: JST 1.25 mm. P5: 4-pin male header (same as P4 — GND, 3.3V, UART1 IO17/18). P1: +5V serial JST. LCD1: main display flex. FPC1: small top FPC (aux). P6 battery, P7 speaker. TF1 microSD.',
    severity: 'common',
  },
  {
    id: '4827-dht-gpio5',
    boardIds: ['jc4827w543r'],
    category: 'wiring',
    title: 'DHT11 on P4 connector GPIO 17',
    body: 'Wire data to IO17 on P4; 3.3 V and GND on the same connector. IO18 stays free (e.g. I2C SCL for a BME later).',
    severity: 'common',
  },
  {
    id: '4827-touch-x-mirror',
    boardIds: ['jc4827w543r'],
    category: 'touch',
    title: 'Touch X may need inverted cal range',
    body: 'Community configs often swap TOUCH_X_MIN/MAX (3500..350) — calint mirrors X in software.',
    fix: 'Try Settings → Touch Test first; then `TOUCH_MIRROR_X=1` or swapped min/max in platformio.ini.',
    severity: 'common',
  },
  {
    id: '4827-psram',
    boardIds: ['jc4827w543r'],
    category: 'build',
    title: 'Enable PSRAM flags',
    body: 'S3-N4R8 has 8 MB OPI PSRAM. Canvas flush needs `-DBOARD_HAS_PSRAM` and `board_build.arduino.memory_type = qio_opi`.',
    severity: 'common',
  },
]

export function gotchasForBoard(boardId: string): Gotcha[] {
  return gotchas.filter((g) => g.boardIds === 'all' || g.boardIds.includes(boardId))
}

export function gotchasByCategory(boardId?: string): Record<GotchaCategory, Gotcha[]> {
  const list = boardId ? gotchasForBoard(boardId) : gotchas
  const out = {} as Record<GotchaCategory, Gotcha[]>
  for (const cat of Object.keys(categoryLabels) as GotchaCategory[]) {
    out[cat] = list.filter((g) => g.category === cat)
  }
  return out
}
