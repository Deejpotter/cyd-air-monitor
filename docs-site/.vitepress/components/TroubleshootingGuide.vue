<script setup lang="ts">
import { computed, ref } from 'vue'
import { boards, getBoard } from '../data/boards'

const props = withDefaults(
  defineProps<{ boardId?: string }>(),
  { boardId: '2432s028r' }
)

const selectedBoard = ref(props.boardId)
const selectedSymptom = ref('')
const copied = ref(false)

const board = computed(() => getBoard(selectedBoard.value) ?? boards[0])

const symptoms = computed(() => {
  const base = [
    { id: 'blank', label: 'Blank or garbled display', applies: () => true },
    { id: 'touch-dead', label: 'Touch not responding', applies: () => true },
    { id: 'touch-offset', label: 'Touch offset or inverted', applies: (b: typeof board.value) => true },
    { id: 'dht-nan', label: 'DHT11 reads NaN', applies: () => true },
    { id: 'wifi', label: 'WiFi will not connect', applies: () => true },
    { id: 'keyboard', label: 'On-screen keyboard stuck open', applies: () => true },
  ]
  return base
})

const fixes = computed(() => {
  const b = board.value
  switch (selectedSymptom.value) {
    case 'blank':
      return [
        `Confirm you are building env <code>${b.env}</code> — wrong env is the #1 cause.`,
        b.displayDriver.includes('Arduino_GFX')
          ? 'This board uses Arduino_GFX QSPI, not TFT_eSPI.'
          : 'Check build log for <code>[copy_template]</code> — User_Setup.h must copy into TFT_eSPI.',
        'Verify USB cable and power; try <code>pio device monitor</code> for boot messages.',
      ]
    case 'touch-dead':
      if (b.touch === 'capacitive') {
        return [
          `Use <code>${b.env}</code> — never flash the resistive env on capacitive hardware.`,
          'Check I²C: SDA=33, SCL=32, RST=25.',
          'Some JC2432W328C boards need CST820_INT=36 instead of 21.',
        ]
      }
      return [
        `Resistive board: env must be <code>${b.env}</code>.`,
        'Open Settings → Touch Test to verify raw ADC values change when pressing.',
        'Check TOUCH_CS and SPI wiring in platformio.ini.',
      ]
    case 'touch-offset':
      if (b.touch === 'capacitive') {
        return [
          'Capacitive: use Touch Test for diagnostics only.',
          'Try TOUCH_MIRROR_X or TOUCH_MIRROR_Y in platformio.ini.',
          'Confirm CAP_TOUCH_NATIVE_LANDSCAPE=1 for JC2432W328C.',
        ]
      }
      return [
        'Settings → Touch Test → adjust X/Y min/max → Save Cal (stored in NVS).',
        `Compile-time defaults for ${b.shortName}: see platformio.ini TOUCH_X/Y_MIN/MAX.`,
        `Touch rot ${b.build.touchRotation}, mirror X=${b.build.touchMirrorX ? 'yes' : 'no'}, Y=${b.build.touchMirrorY ? 'yes' : 'no'}.`,
        ...(b.troubleshooting.find((t) => t.symptom.toLowerCase().includes('touch'))?.fixes ?? []),
      ]
    case 'dht-nan':
      return [
        `Wire DHT11 data to GPIO <strong>${b.dhtPin}</strong> for this board.`,
        'VCC to 3.3 V, GND to GND; allow a few seconds after boot.',
        'If using long wires, add a 10 kΩ pull-up on the data line.',
      ]
    case 'wifi':
      return [
        'Settings → WiFi → Scan → pick SSID from dropdown → enter password → Connect.',
        'Credentials persist in NVS; use Forget WiFi to clear bad saves.',
        '2.4 GHz networks only — ESP32 does not support 5 GHz WiFi.',
      ]
    case 'keyboard':
      return [
        'Tap OK or Cancel on the keyboard, or tap outside the password field.',
        'Connect / Scan / Back buttons also dismiss the keyboard.',
      ]
    default:
      return ['Select a symptom above to see board-specific suggestions.']
  }
})

async function copyFixes() {
  const text = fixes.value.map((f) => f.replace(/<[^>]+>/g, '')).join('\n• ')
  await navigator.clipboard.writeText('• ' + text)
  copied.value = true
  setTimeout(() => (copied.value = false), 1600)
}
</script>

<template>
  <div class="troubleshooting-guide cyd-card">
    <div class="cyd-grid cyd-grid-2">
      <div>
        <label style="display: block; font-weight: 600; margin-bottom: 0.35rem">Your board</label>
        <select v-model="selectedBoard" class="cyd-btn" style="width: 100%; margin-bottom: 1rem">
          <option v-for="b in boards" :key="b.id" :value="b.id">{{ b.name }}</option>
        </select>

        <label style="display: block; font-weight: 600; margin-bottom: 0.35rem">What's wrong?</label>
        <div class="cyd-grid">
          <button
            v-for="s in symptoms"
            :key="s.id"
            class="cyd-btn"
            :class="{ 'is-active': selectedSymptom === s.id }"
            style="text-align: left"
            @click="selectedSymptom = s.id"
          >
            {{ s.label }}
          </button>
        </div>
      </div>

      <div>
        <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 0.5rem">
          <h3 style="margin: 0">Suggested fixes</h3>
          <button v-if="selectedSymptom" class="cyd-btn cyd-btn--copy" @click="copyFixes">
            {{ copied ? 'Copied!' : 'Copy' }}
          </button>
        </div>
        <div v-for="(fix, i) in fixes" :key="i" class="cyd-trouble-item">
          <p v-html="fix" />
        </div>

        <div v-if="board.troubleshooting.length && selectedSymptom.startsWith('touch')" style="margin-top: 1.25rem">
          <h4>Also documented for {{ board.shortName }}</h4>
          <div v-for="(item, i) in board.troubleshooting" :key="i" class="cyd-trouble-item">
            <h4>{{ item.symptom }}</h4>
            <ul style="margin: 0; padding-left: 1.1rem; color: var(--vp-c-text-2); font-size: 0.9rem">
              <li v-for="(f, j) in item.fixes" :key="j">{{ f }}</li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
select.cyd-btn {
  appearance: auto;
}
</style>
