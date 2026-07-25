export type ReferenceCategory =
  | 'community'
  | 'pinout'
  | 'board-repo'
  | 'library'
  | 'display-touch'
  | 'tutorial'
  | 'this-project'
  | 'forum'

export interface ReferenceLink {
  id: string
  title: string
  url: string
  description: string
  category: ReferenceCategory
  boardIds?: string[] | 'all'
  tags?: string[]
}

export const referenceCategoryLabels: Record<ReferenceCategory, string> = {
  community: 'Community hubs',
  pinout: 'Pinouts & hardware',
  'board-repo': 'Board example repos',
  library: 'Libraries & drivers',
  'display-touch': 'Display & touch deep dives',
  tutorial: 'Tutorials & guides',
  'this-project': 'This firmware project',
  forum: 'Forums & discussions',
}

export const references: ReferenceLink[] = [
  // This project
  {
    id: 'cyd-air-monitor',
    title: 'cyd-air-monitor (this repo)',
    url: 'https://github.com/Deejpotter/cyd-air-monitor',
    description: 'Multi-board air monitor firmware — DHT11, LVGL UI, WiFi settings, touch calibration.',
    category: 'this-project',
    boardIds: 'all',
    tags: ['firmware', 'platformio'],
  },
  {
    id: 'display-config-md',
    title: 'DISPLAY_CONFIG.md',
    url: 'https://github.com/Deejpotter/cyd-air-monitor/blob/main/docs/DISPLAY_CONFIG.md',
    description: 'Rotation matrix, MADCTL notes, and compile-time flag reference in the repo.',
    category: 'this-project',
    boardIds: 'all',
    tags: ['display', 'touch', 'madctl'],
  },
  {
    id: 'cyd-lvgl-template',
    title: 'cyd-lvgl-template (upstream)',
    url: 'https://github.com/Deejpotter/cyd-lvgl-template',
    description: 'Display/touch/LVGL foundation this project extends.',
    category: 'this-project',
    boardIds: 'all',
  },
  {
    id: 'platformio-ini',
    title: 'platformio.ini (env definitions)',
    url: 'https://github.com/Deejpotter/cyd-air-monitor/blob/main/platformio.ini',
    description: 'Authoritative build flags, pins, and lib deps per board env.',
    category: 'this-project',
    boardIds: 'all',
  },

  // Community
  {
    id: 'witnessmenow-cyd',
    title: 'ESP32-Cheap-Yellow-Display',
    url: 'https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display',
    description: 'Central community hub for CYD boards — examples, discussions, pinout notes.',
    category: 'community',
    boardIds: ['2432s028r', 'jc2432w328r', 'jc2432w328c'],
    tags: ['cyd', 'community'],
  },
  {
    id: 'witnessmenow-discussions',
    title: 'CYD GitHub Discussions',
    url: 'https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/discussions',
    description: 'Board identification, env choice, and vendor revision threads.',
    category: 'community',
    boardIds: ['2432s028r', 'jc2432w328r', 'jc2432w328c'],
  },
  {
    id: 'squareline-cyd',
    title: 'SquareLine CYD template discussion',
    url: 'https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/discussions/102',
    description: 'UI tooling and CYD-specific LVGL/SquareLine notes.',
    category: 'forum',
    boardIds: ['2432s028r', 'jc2432w328r', 'jc2432w328c'],
  },

  // 2432S028R pinouts & tutorials
  {
    id: 'rnt-pinout-028r',
    title: 'Random Nerd Tutorials — 2432S028R pinout',
    url: 'https://randomnerdtutorials.com/esp32-cheap-yellow-display-cyd-pinout-esp32-2432s028r/',
    description: 'GPIO map, dual USB ports, touch SPI pins, and starter wiring.',
    category: 'pinout',
    boardIds: ['2432s028r'],
  },
  {
    id: 'rnt-cyd-intro',
    title: 'Random Nerd Tutorials — CYD getting started',
    url: 'https://randomnerdtutorials.com/esp32-cheap-yellow-display-cyd-st7789/',
    description: 'ST7789 setup overview for the original Cheap Yellow Display.',
    category: 'tutorial',
    boardIds: ['2432s028r'],
  },
  {
    id: 'rnt-resistive-cal',
    title: 'Random Nerd Tutorials — resistive touch calibration',
    url: 'https://randomnerdtutorials.com/esp32-cheap-yellow-display-cyd-resistive-touchscreen-calibration/',
    description: 'XPT2046 calibration workflow and raw ADC tuning.',
    category: 'tutorial',
    boardIds: ['2432s028r', 'jc2432w328r', 'jc4827w543r'],
    tags: ['xpt2046', 'calibration'],
  },
  {
    id: 'rnt-cyd-hub',
    title: 'Random Nerd Tutorials — CYD hub page',
    url: 'https://randomnerdtutorials.com/projects-using-cyd-cheap-yellow-display/',
    description: 'Index of CYD projects and tutorials.',
    category: 'tutorial',
    boardIds: ['2432s028r', 'jc2432w328r', 'jc2432w328c'],
  },
  {
    id: 'boxwrench-cyd-ref',
    title: 'boxwrench ESP32 CYD reference',
    url: 'https://github.com/boxwrench/DIST_ESP32-CYD-Tester/blob/main/docs/ESP32_CYD_REFERENCE.md',
    description: 'Detailed CYD reference doc with revision notes.',
    category: 'pinout',
    boardIds: ['2432s028r', 'jc2432w328r', 'jc2432w328c'],
  },
  {
    id: 'cf20852-cal',
    title: 'CF20852 — 2432S028 touch calibration sketch',
    url: 'https://github.com/CF20852/ESP32-2432S028-Touchscreen-Calibration',
    description: 'Advanced alpha/beta/delta calibration for skewed resistive panels.',
    category: 'display-touch',
    boardIds: ['2432s028r'],
    tags: ['calibration', 'xpt2046'],
  },

  // JC2432
  {
    id: 'kmendelev-jc2432c',
    title: 'kmendelev — JC2432W328C examples',
    url: 'https://github.com/kmendelev/jc2432w328c-examples',
    description: 'Capacitive touch native portrait → landscape transform (validated mapping).',
    category: 'board-repo',
    boardIds: ['jc2432w328c'],
    tags: ['cst820', 'capacitive'],
  },

  // JC4827
  {
    id: 'profi-max-jc4827',
    title: 'profi-max — JC4827W543 4.3" ESP32-S3 board',
    url: 'https://github.com/profi-max/JC4827W543_4.3inch_ESP32S3_board',
    description: 'LVGL Widgets PlatformIO example, QSPI NV3041A init, touch wiring.',
    category: 'board-repo',
    boardIds: ['jc4827w543r'],
    tags: ['arduino_gfx', 'qspi', 'lvgl'],
  },
  {
    id: 'calint-jc4827',
    title: 'calint — JC4827W543R',
    url: 'https://github.com/calint/JC4827W543R',
    description: 'PlatformIO env, NV3041A bring-up, inverted X touch cal defaults.',
    category: 'board-repo',
    boardIds: ['jc4827w543r'],
  },
  {
    id: 'lovyan-jc4827-discussion',
    title: 'LovyanGFX — JC4827 QSPI porting discussion',
    url: 'https://github.com/lovyan03/LovyanGFX/discussions/604',
    description: 'Arduino_GFX QSPI config that works; notes on NV3041A vs SH8601Z.',
    category: 'forum',
    boardIds: ['jc4827w543r'],
    tags: ['qspi', 'nv3041a'],
  },

  // Libraries
  {
    id: 'tft-espi',
    title: 'TFT_eSPI (Bodmer)',
    url: 'https://github.com/bodmer/TFT_eSPI',
    description: 'ST7789 driver used by all 2.8" CYD envs. User_Setup.h per env.',
    category: 'library',
    boardIds: ['2432s028r', 'jc2432w328r', 'jc2432w328c'],
  },
  {
    id: 'tft-espi-docs',
    title: 'TFT_eSPI setRotation / MADCTL docs',
    url: 'https://doc-tft-espi.readthedocs.io/tft_espi/methods/setrotation/',
    description: 'How ST7789 rotation and MADCTL bits interact.',
    category: 'library',
    boardIds: ['2432s028r', 'jc2432w328r', 'jc2432w328c'],
    tags: ['madctl', 'rotation'],
  },
  {
    id: 'arduino-gfx',
    title: 'Arduino_GFX (moononournation)',
    url: 'https://github.com/moononournation/Arduino_GFX',
    description: 'NV3041A QSPI driver for JC4827. PlatformIO package: GFX Library for Arduino.',
    category: 'library',
    boardIds: ['jc4827w543r'],
  },
  {
    id: 'arduino-gfx-nv3041a',
    title: 'Arduino_NV3041A.cpp (rotation source)',
    url: 'https://github.com/moononournation/Arduino_GFX/blob/master/src/display/Arduino_NV3041A.cpp',
    description: 'MADCTL values for setRotation 0–3 on NV3041A panels.',
    category: 'library',
    boardIds: ['jc4827w543r'],
    tags: ['madctl', 'nv3041a'],
  },
  {
    id: 'pio-arduino-gfx',
    title: 'PlatformIO — GFX Library for Arduino registry',
    url: 'https://registry.platformio.org/libraries/moononournation/GFX%20Library%20for%20Arduino',
    description: 'Correct PlatformIO package name (not Arduino_GFX).',
    category: 'library',
    boardIds: ['jc4827w543r'],
  },
  {
    id: 'xpt2046-lib',
    title: 'XPT2046_Touchscreen (PaulStoffregen)',
    url: 'https://github.com/PaulStoffregen/XPT2046_Touchscreen',
    description: 'Resistive touch driver for 2432S028R, JC2432R, and JC4827.',
    category: 'library',
    boardIds: ['2432s028r', 'jc2432w328r', 'jc4827w543r'],
  },
  {
    id: 'bb-captouch',
    title: 'bb_captouch (bitbank2)',
    url: 'https://github.com/bitbank2/bb_captouch',
    description: 'CST820/CST816 capacitive driver for JC2432W328C.',
    category: 'library',
    boardIds: ['jc2432w328c'],
  },
  {
    id: 'lvgl-docs',
    title: 'LVGL documentation',
    url: 'https://docs.lvgl.io/',
    description: 'UI library used by all boards — widgets, keyboard, rotation concepts.',
    category: 'library',
    boardIds: 'all',
  },

  // Display / touch forums
  {
    id: 'lvgl-forum-rotation',
    title: 'LVGL forum — display vs touch rotation',
    url: 'https://forum.lvgl.io/t/inconsistency-between-display-rotation-and-touchscreen-coordinate-rotation/20242',
    description: 'Why display and touch rotation must be tuned independently.',
    category: 'forum',
    boardIds: 'all',
    tags: ['rotation', 'touch'],
  },
  {
    id: 'lvgl-forum-screen-rot',
    title: 'LVGL forum — screen rotation',
    url: 'https://forum.lvgl.io/t/screen-rotation/19316',
    description: 'Pointer input vs display rotation on ESP32 panels.',
    category: 'forum',
    boardIds: 'all',
  },
  {
    id: 'lovyan-711',
    title: 'LovyanGFX #711 — touch orientation vs panel',
    url: 'https://github.com/lovyan03/LovyanGFX/issues/711',
    description: 'Why CAP_TOUCH_NATIVE_LANDSCAPE map is preferred over setOrientation() on CST820.',
    category: 'display-touch',
    boardIds: ['jc2432w328c'],
  },
  {
    id: 'lvgl-micropython-cyd',
    title: 'lvgl_micropython CYD touch discussion',
    url: 'https://github.com/lvgl-micropython/lvgl_micropython/discussions/281',
    description: 'Mirror both axes common on CYD resistive after mapping.',
    category: 'forum',
    boardIds: ['2432s028r', 'jc2432w328r'],
  },
  {
    id: 'pio-gfx-esp32s3',
    title: 'PlatformIO community — GFX on ESP32-S3',
    url: 'https://community.platformio.org/t/esp32-s3-compilation-issue-with-gfx-library/45807',
    description: 'ESP32-S3 + Arduino_GFX version/platform compatibility notes.',
    category: 'forum',
    boardIds: ['jc4827w543r'],
  },
  {
    id: 'arduino-gfx-606',
    title: 'Arduino_GFX #606 — PlatformIO / ESP-IDF versions',
    url: 'https://github.com/moononournation/Arduino_GFX/issues/606',
    description: 'Library vs platform version matrix; pin to GFX 1.4.9 on espressif32 6.x.',
    category: 'forum',
    boardIds: ['jc4827w543r'],
  },

  // PlatformIO / Espressif
  {
    id: 'pio-espressif32',
    title: 'PlatformIO espressif32 platform',
    url: 'https://docs.platformio.org/en/latest/platforms/espressif32.html',
    description: 'Board definitions, upload ports, PSRAM, partition tables.',
    category: 'tutorial',
    boardIds: 'all',
  },
  {
    id: 'esp32-s3-devkitc',
    title: 'ESP32-S3-DevKitC-1 (PlatformIO board doc)',
    url: 'https://docs.platformio.org/en/latest/boards/espressif32/esp32-s3-devkitc-1.html',
    description: 'Reference board used by jc4827w543r env.',
    category: 'pinout',
    boardIds: ['jc4827w543r'],
  },
]

export function referencesForBoard(boardId: string): ReferenceLink[] {
  return references.filter((r) => r.boardIds === 'all' || r.boardIds?.includes(boardId))
}

export function referencesByCategory(
  boardId?: string
): Record<ReferenceCategory, ReferenceLink[]> {
  const list = boardId ? referencesForBoard(boardId) : references
  const out = {} as Record<ReferenceCategory, ReferenceLink[]>
  for (const cat of Object.keys(referenceCategoryLabels) as ReferenceCategory[]) {
    out[cat] = list.filter((r) => r.category === cat)
  }
  return out
}
