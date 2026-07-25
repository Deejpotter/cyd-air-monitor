export interface BoardPin {
  label: string
  gpio: string
  notes?: string
}

export interface BoardFeature {
  title: string
  detail: string
  icon?: string
}

export interface BoardSpec {
  id: string
  env: string
  name: string
  shortName: string
  tagline: string
  mcu: string
  display: string
  resolution: string
  touch: 'resistive' | 'capacitive'
  touchChip: string
  displayDriver: string
  dhtPin: number
  rgbLed: boolean
  defaultEnv: boolean
  uploadPort?: string
  hardwareFeatures: BoardFeature[]
  firmwareFeatures: BoardFeature[]
  highlights: string[]
  notes: string[]
  build: {
    displayRotation: number
    madctlOverride?: string
    touchRotation: number
    touchMirrorX: boolean
    touchMirrorY: boolean
    capNativeLandscape?: boolean
  }
  pins: {
    display?: BoardPin[]
    touch?: BoardPin[]
    sensor?: BoardPin[]
    other?: BoardPin[]
  }
  troubleshooting: { symptom: string; fixes: string[] }[]
  links: { label: string; url: string }[]
}

export const boards: BoardSpec[] = [
  {
    id: '2432s028r',
    env: '2432s028r',
    name: 'ESP32-2432S028R',
    shortName: 'Original CYD',
    tagline: 'The classic Cheap Yellow Display — dual USB, separate touch SPI, and the board most tutorials assume.',
    mcu: 'ESP32',
    display: '2.8" ST7789',
    resolution: '320×240',
    touch: 'resistive',
    touchChip: 'XPT2046',
    displayDriver: 'TFT_eSPI',
    dhtPin: 27,
    rgbLed: true,
    defaultEnv: true,
    uploadPort: '/dev/ttyUSB0',
    hardwareFeatures: [
      { icon: '🖥', title: '2.8" IPS panel', detail: 'ST7789 over SPI at 320×240 landscape — the reference CYD size and pinout.' },
      { icon: '👆', title: 'Resistive touch', detail: 'XPT2046 on its own SPI bus (not shared with the display).' },
      { icon: '🔌', title: 'Dual USB-C', detail: 'Power and serial on separate ports — common on AliExpress "CYD" listings.' },
      { icon: '💡', title: 'RGB status LED', detail: 'Onboard RGB LED driven by firmware during boot and status.' },
    ],
    firmwareFeatures: [
      { icon: '🌡', title: 'DHT11 dashboard', detail: 'Temperature and humidity on GPIO 27 with live LVGL labels.' },
      { icon: '📶', title: 'WiFi settings', detail: 'Scan, connect, and forget networks — credentials saved to NVS.' },
      { icon: '👆', title: 'Touch calibration', detail: 'Settings → Touch Test with min/max adjust and Save Cal to NVS.' },
      { icon: '🔄', title: 'Runtime rotation', detail: 'Settings → Rotate Display cycles 0°/90°/180°/270° with touch following.' },
    ],
    highlights: [
      'Default PlatformIO env for this repo',
      'MADCTL 0x20 override fixes short-axis flip at rotation 1',
      'Touch SPI: MOSI 32, MISO 39, CLK 25, CS 33',
    ],
    notes: [
      'Dual USB-C variant of the classic Cheap Yellow Display.',
      'Touch uses a separate SPI bus from the display.',
      'Display needs MADCTL 0x20 override after setRotation(1).',
    ],
    build: {
      displayRotation: 1,
      madctlOverride: '0x20',
      touchRotation: 1,
      touchMirrorX: true,
      touchMirrorY: true,
    },
    pins: {
      display: [
        { label: 'MISO', gpio: '12' },
        { label: 'MOSI', gpio: '13' },
        { label: 'SCLK', gpio: '14' },
        { label: 'CS', gpio: '15' },
        { label: 'DC', gpio: '2' },
        { label: 'Backlight', gpio: '21' },
      ],
      touch: [
        { label: 'CS', gpio: '33' },
        { label: 'IRQ', gpio: '36' },
        { label: 'MOSI', gpio: '32', notes: 'Separate SPI bus' },
        { label: 'MISO', gpio: '39' },
        { label: 'CLK', gpio: '25' },
      ],
      sensor: [{ label: 'DHT11 data', gpio: '27' }],
    },
    troubleshooting: [
      {
        symptom: 'Display OK but one axis flipped',
        fixes: [
          'Keep TFT_DISPLAY_ROTATION=1 and use TFT_MADCTL_OVERRIDE=0x20.',
          'Do not switch to rotation 3 unless you also retune touch.',
        ],
      },
      {
        symptom: 'Touch offset or inverted',
        fixes: [
          'Use Settings → Touch Test to calibrate min/max and save to NVS.',
          'Try Settings → Rotate Display before editing platformio.ini flags.',
        ],
      },
    ],
    links: [
      {
        label: 'Random Nerd Tutorials pinout',
        url: 'https://randomnerdtutorials.com/esp32-cheap-yellow-display-cyd-pinout-esp32-2432s028r/',
      },
      {
        label: 'ESP32-Cheap-Yellow-Display hub',
        url: 'https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display',
      },
    ],
  },
  {
    id: 'jc2432w328r',
    env: 'jc2432w328r',
    name: 'JC2432W328R',
    shortName: 'JC2432 Resistive',
    tagline: 'Guition JC2432 with resistive film touch — shared SPI, different rotation and DHT pin than the original CYD.',
    mcu: 'ESP32',
    display: '2.8" ST7789',
    resolution: '320×240',
    touch: 'resistive',
    touchChip: 'XPT2046',
    displayDriver: 'TFT_eSPI',
    dhtPin: 21,
    rgbLed: false,
    defaultEnv: false,
    uploadPort: '/dev/ttyUSB0',
    hardwareFeatures: [
      { icon: '🖥', title: '2.8" ST7789', detail: 'Same panel size as the CYD but Guition pinout and rotation defaults.' },
      { icon: '👆', title: 'Resistive XPT2046', detail: 'Plastic film overlay — the R in the model name means resistive.' },
      { icon: '🔗', title: 'Shared touch SPI', detail: 'Touch CS on GPIO 33 shares the display SPI bus (unlike original CYD).' },
      { icon: '📍', title: 'DHT on GPIO 21', detail: 'Backlight on GPIO 27 — swapped vs the original CYD wiring.' },
    ],
    firmwareFeatures: [
      { icon: '🌡', title: 'DHT11 dashboard', detail: 'Sensor data on GPIO 21.' },
      { icon: '📶', title: 'WiFi settings', detail: 'Full scan/connect/forget flow with NVS persistence.' },
      { icon: '👆', title: 'Touch calibration', detail: 'Resistive cal with Save Cal — defaults X 200–3700, Y 240–3800.' },
      { icon: '🔄', title: 'Runtime rotation', detail: 'Rotate the UI in Settings without reflashing.' },
    ],
    highlights: [
      'Model must say R — not the capacitive C variant',
      'Display rotation 3, touch rotation 1, mirror X+Y',
      'Flash jc2432w328r only on resistive panels',
    ],
    notes: [
      'Resistive touch variant; XPT2046 shares the display SPI bus.',
      'Display rotation 3 with touch rotation 1 and mirror both axes.',
    ],
    build: {
      displayRotation: 3,
      touchRotation: 1,
      touchMirrorX: true,
      touchMirrorY: true,
    },
    pins: {
      display: [
        { label: 'MISO', gpio: '12' },
        { label: 'MOSI', gpio: '13' },
        { label: 'SCLK', gpio: '14' },
        { label: 'CS', gpio: '15' },
        { label: 'DC', gpio: '2' },
        { label: 'Backlight', gpio: '27' },
      ],
      touch: [{ label: 'CS', gpio: '33', notes: 'Shared SPI with display' }],
      sensor: [{ label: 'DHT11 data', gpio: '21' }],
    },
    troubleshooting: [
      {
        symptom: 'Touch dead',
        fixes: [
          'Confirm you are using jc2432w328r, not jc2432w328c.',
          'Check TOUCH_CS=33 is set in platformio.ini.',
        ],
      },
      {
        symptom: 'Touch mapped wrong',
        fixes: [
          'Calibrate via Settings → Touch Test.',
          'Try Settings → Rotate Display if axes feel swapped.',
        ],
      },
    ],
    links: [
      {
        label: 'ESP32-Cheap-Yellow-Display',
        url: 'https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display',
      },
    ],
  },
  {
    id: 'jc2432w328c',
    env: 'jc2432w328c',
    name: 'JC2432W328C',
    shortName: 'JC2432 Capacitive',
    tagline: 'Glass capacitive touch on I²C — the C variant looks like the R board but needs a completely different firmware env.',
    mcu: 'ESP32',
    display: '2.8" ST7789',
    resolution: '320×240',
    touch: 'capacitive',
    touchChip: 'CST820 / CST816',
    displayDriver: 'TFT_eSPI',
    dhtPin: 22,
    rgbLed: false,
    defaultEnv: false,
    uploadPort: '/dev/ttyUSB0',
    hardwareFeatures: [
      { icon: '🖥', title: '2.8" ST7789', detail: 'Same display stack as other JC2432 boards — TFT_eSPI at 320×240.' },
      { icon: '✨', title: 'Capacitive glass', detail: 'CST820/CST816 on I²C — no resistive film, smoother multi-touch capable chip.' },
      { icon: '🧭', title: 'Portrait-native touch', detail: 'Chip reports 240×320; firmware maps to landscape UI coordinates.' },
      { icon: '📍', title: 'DHT on GPIO 22', detail: 'I²C touch: SDA 33, SCL 32, RST 25, INT 21 (try 36 on some revisions).' },
    ],
    firmwareFeatures: [
      { icon: '🌡', title: 'DHT11 dashboard', detail: 'Sensor on GPIO 22.' },
      { icon: '📶', title: 'WiFi settings', detail: 'Same settings UI as resistive boards.' },
      { icon: '🔍', title: 'Touch diagnostic', detail: 'Touch Test shows live coords — no NVS cal (fix via build flags).' },
      { icon: '🔄', title: 'Runtime rotation', detail: 'Settings → Rotate Display with LVGL-coordinated touch.' },
    ],
    highlights: [
      'Never flash jc2432w328r on this board — display works, touch does not',
      'CAP_TOUCH_NATIVE_LANDSCAPE=1 for portrait→landscape map',
      'Some revisions need CST820_INT=36 instead of 21',
    ],
    notes: [
      'Capacitive touch on I²C — do not flash the resistive env.',
      'Chip reports native portrait coords; firmware maps to landscape.',
      'Some board revisions use INT=36 instead of INT=21.',
    ],
    build: {
      displayRotation: 1,
      touchRotation: 0,
      touchMirrorX: false,
      touchMirrorY: false,
      capNativeLandscape: true,
    },
    pins: {
      display: [
        { label: 'MISO', gpio: '12' },
        { label: 'MOSI', gpio: '13' },
        { label: 'SCLK', gpio: '14' },
        { label: 'CS', gpio: '15' },
        { label: 'DC', gpio: '2' },
        { label: 'Backlight', gpio: '27' },
      ],
      touch: [
        { label: 'SDA', gpio: '33' },
        { label: 'SCL', gpio: '32' },
        { label: 'RST', gpio: '25' },
        { label: 'INT', gpio: '21', notes: 'Try 36 on some revisions' },
      ],
      sensor: [{ label: 'DHT11 data', gpio: '22' }],
    },
    troubleshooting: [
      {
        symptom: 'No touch at all',
        fixes: [
          'Verify jc2432w328c env (not jc2432w328r).',
          'Check I²C wiring: SDA=33, SCL=32, RST=25.',
          'Try CST820_INT=36 in platformio.ini if INT=21 fails.',
        ],
      },
      {
        symptom: 'Touch rotated 90°',
        fixes: [
          'Confirm CAP_TOUCH_NATIVE_LANDSCAPE=1.',
          'Try Settings → Rotate Display before editing build flags.',
        ],
      },
    ],
    links: [
      {
        label: 'kmendelev JC2432W328C examples',
        url: 'https://github.com/kmendelev/jc2432w328c-examples',
      },
      {
        label: 'bb_captouch library',
        url: 'https://github.com/bitbank2/bb_captouch',
      },
    ],
  },
  {
    id: 'jc4827w543r',
    env: 'jc4827w543r',
    name: 'JC4827W543R',
    shortName: 'Guition 4.3"',
    tagline: 'The Cheap Chonky Display — ESP32-S3, 480×272 QSPI panel, and a different display stack from every 2.8" CYD.',
    mcu: 'ESP32-S3-N4R8',
    display: '4.3" NV3041A',
    resolution: '480×272',
    touch: 'resistive',
    touchChip: 'XPT2046',
    displayDriver: 'Arduino_GFX (QSPI)',
    dhtPin: 5,
    rgbLed: false,
    defaultEnv: false,
    uploadPort: '/dev/ttyACM0',
    hardwareFeatures: [
      { icon: '🖥', title: '4.3" wide panel', detail: 'Native 480×272 landscape — more room for dashboard text and settings.' },
      { icon: '⚡', title: 'QSPI display bus', detail: 'NV3041A via Arduino_GFX, not TFT_eSPI. Requires jc4827w543r env.' },
      { icon: '🧠', title: 'ESP32-S3 + PSRAM', detail: '8 MB flash, PSRAM enabled — USB CDC serial on /dev/ttyACM0.' },
      { icon: '👆', title: 'Resistive touch', detail: 'Separate SPI for XPT2046 — CS 38, with board-specific cal defaults.' },
    ],
    firmwareFeatures: [
      { icon: '🌡', title: 'DHT11 on P2', detail: 'GPIO 5 on the P2 connector — not the same pins as 2.8" boards.' },
      { icon: '📶', title: 'WiFi settings', detail: 'Same LVGL settings screens scaled for the wider display.' },
      { icon: '👆', title: 'Touch calibration', detail: 'Defaults X 350–3500, Y 300–3800 — save via Touch Test.' },
      { icon: '🔄', title: 'Runtime rotation', detail: 'Settings → Rotate Display — useful when panel is mounted upside-down.' },
    ],
    highlights: [
      'TFT_eSPI builds produce a blank display — must use Arduino_GFX',
      'TFT_DISPLAY_ROTATION=2 via setRotation() — no raw MADCTL writes',
      'TOUCH_MIRROR_X=1 for long-axis alignment',
    ],
    notes: [
      'Guition "Cheap Chonky Display" — ESP32-S3 with 8 MB PSRAM.',
      'Display uses QSPI via Arduino_GFX, not TFT_eSPI.',
      'Native landscape 480×272; DHT11 on P2 connector GPIO 5.',
      'No onboard RGB LED.',
    ],
    build: {
      displayRotation: 2,
      touchRotation: 0,
      touchMirrorX: true,
      touchMirrorY: false,
    },
    pins: {
      display: [
        { label: 'QSPI CS', gpio: '45' },
        { label: 'QSPI SCK', gpio: '47' },
        { label: 'QSPI D0', gpio: '21' },
        { label: 'QSPI D1', gpio: '48' },
        { label: 'QSPI D2', gpio: '40' },
        { label: 'QSPI D3', gpio: '39' },
        { label: 'Backlight', gpio: '1' },
      ],
      touch: [
        { label: 'CS', gpio: '38' },
        { label: 'IRQ', gpio: '3' },
        { label: 'MOSI', gpio: '11' },
        { label: 'MISO', gpio: '13' },
        { label: 'CLK', gpio: '12' },
      ],
      sensor: [{ label: 'DHT11 data (P2)', gpio: '5' }],
      other: [{ label: 'USB', gpio: 'USB-C CDC', notes: 'ARDUINO_USB_CDC_ON_BOOT=1' }],
    },
    troubleshooting: [
      {
        symptom: 'Blank display',
        fixes: [
          'Use jc4827w543r env — TFT_eSPI builds will not drive this panel.',
          'Confirm PSRAM flags and Arduino_GFX 1.4.9 in platformio.ini.',
        ],
      },
      {
        symptom: 'Touch inaccurate',
        fixes: [
          'Calibrate via Settings → Touch Test.',
          'Try Settings → Rotate Display or TOUCH_MIRROR_X=1.',
        ],
      },
    ],
    links: [
      {
        label: 'profi-max JC4827W543 examples',
        url: 'https://github.com/profi-max/JC4827W543_4.3inch_ESP32S3_board',
      },
      {
        label: 'calint JC4827W543R',
        url: 'https://github.com/calint/JC4827W543R',
      },
    ],
  },
]

export function getBoard(id: string): BoardSpec | undefined {
  return boards.find((b) => b.id === id)
}

export function buildCommands(env: string, uploadPort?: string): string[] {
  const upload = uploadPort
    ? `pio run -e ${env} --target upload --upload-port ${uploadPort}`
    : `pio run -e ${env} --target upload`
  return [`pio run -e ${env}`, upload, `pio device monitor`]
}
