<script setup lang="ts">
import { computed, ref, watch } from 'vue'
import { boards, buildCommands, type BoardSpec } from '../data/boards'

const props = withDefaults(
  defineProps<{
    initialBoard?: string
    compact?: boolean
  }>(),
  { initialBoard: '2432s028r', compact: false }
)

const emit = defineEmits<{ (e: 'select', board: BoardSpec): void }>()

const selectedId = ref(props.initialBoard)
const touchFilter = ref<'all' | 'resistive' | 'capacitive'>('all')
const copied = ref('')

const filteredBoards = computed(() => {
  if (touchFilter.value === 'all') return boards
  return boards.filter((b) => b.touch === touchFilter.value)
})

const selected = computed(() => boards.find((b) => b.id === selectedId.value) ?? boards[0])

watch(selected, (b) => emit('select', b), { immediate: true })

function selectBoard(id: string) {
  selectedId.value = id
}

async function copyText(text: string, key: string) {
  try {
    await navigator.clipboard.writeText(text)
    copied.value = key
    setTimeout(() => {
      if (copied.value === key) copied.value = ''
    }, 1600)
  } catch {
    copied.value = ''
  }
}

const commands = computed(() => buildCommands(selected.value.env, selected.value.uploadPort))

const previewClass = computed(() =>
  selected.value.resolution.startsWith('480') ? 'cyd-panel-preview cyd-panel-preview--wide' : 'cyd-panel-preview'
)
</script>

<template>
  <div class="board-explorer">
    <div v-if="!compact" class="cyd-filter-row">
      <span style="align-self: center; color: var(--vp-c-text-2); font-size: 0.875rem; margin-right: 0.25rem">Filter:</span>
      <button class="cyd-btn" :class="{ 'is-active': touchFilter === 'all' }" @click="touchFilter = 'all'">All</button>
      <button class="cyd-btn" :class="{ 'is-active': touchFilter === 'resistive' }" @click="touchFilter = 'resistive'">Resistive</button>
      <button class="cyd-btn" :class="{ 'is-active': touchFilter === 'capacitive' }" @click="touchFilter = 'capacitive'">Capacitive</button>
    </div>

    <div class="cyd-grid cyd-grid-2" style="align-items: start">
      <div>
        <div class="cyd-grid" style="margin-bottom: 1rem">
          <button
            v-for="board in filteredBoards"
            :key="board.id"
            class="cyd-btn"
            :class="{ 'is-active': selectedId === board.id }"
            style="text-align: left; width: 100%"
            @click="selectBoard(board.id)"
          >
            <strong>{{ board.shortName }}</strong>
            <span style="display: block; font-size: 0.8rem; color: var(--vp-c-text-2); margin-top: 0.15rem">
              {{ board.resolution }} · {{ board.touchChip }}
            </span>
          </button>
        </div>

        <div :class="previewClass">
          {{ selected.resolution }}
        </div>
      </div>

      <div class="cyd-card">
        <h3 style="margin-top: 0">{{ selected.name }}</h3>
        <p style="color: var(--vp-c-text-2); margin-top: -0.25rem">
          PlatformIO env: <code>{{ selected.env }}</code>
        </p>

        <div style="margin: 0.75rem 0">
          <span class="cyd-tag cyd-tag--esp32" v-if="selected.mcu.startsWith('ESP32') && !selected.mcu.includes('S3')">{{ selected.mcu }}</span>
          <span class="cyd-tag cyd-tag--s3" v-if="selected.mcu.includes('S3')">{{ selected.mcu }}</span>
          <span class="cyd-tag" :class="selected.touch === 'resistive' ? 'cyd-tag--resistive' : 'cyd-tag--capacitive'">
            {{ selected.touch }} · {{ selected.touchChip }}
          </span>
          <span class="cyd-tag" v-if="selected.rgbLed">RGB LED</span>
        </div>

        <table class="cyd-spec-table">
          <tbody>
            <tr><th>Display</th><td>{{ selected.display }} ({{ selected.resolution }})</td></tr>
            <tr><th>Driver</th><td>{{ selected.displayDriver }}</td></tr>
            <tr><th>DHT11 pin</th><td>GPIO {{ selected.dhtPin }}</td></tr>
            <tr><th>Display rot</th><td>{{ selected.build.displayRotation }}<span v-if="selected.build.madctlOverride"> + MADCTL {{ selected.build.madctlOverride }}</span></td></tr>
            <tr><th>Touch setup</th><td>rot {{ selected.build.touchRotation }}<span v-if="selected.build.touchMirrorX || selected.build.touchMirrorY">, mirror {{ selected.build.touchMirrorX ? 'X' : '' }}{{ selected.build.touchMirrorX && selected.build.touchMirrorY ? '+' : '' }}{{ selected.build.touchMirrorY ? 'Y' : '' }}</span></td></tr>
          </tbody>
        </table>

        <h4 style="margin-bottom: 0.5rem">Build &amp; flash</h4>
        <div v-for="(cmd, i) in commands" :key="cmd" class="cyd-code-block" style="margin-bottom: 0.5rem">
          {{ cmd }}
          <button class="cyd-btn cyd-btn--copy" @click="copyText(cmd, `cmd-${i}`)">
            {{ copied === `cmd-${i}` ? 'Copied!' : 'Copy' }}
          </button>
        </div>

        <details style="margin-top: 1rem" open>
          <summary><strong>Gotchas for this board</strong></summary>
          <div style="margin-top: 0.75rem">
            <BoardGotchas :board-id="selectedId" compact />
          </div>
        </details>

        <details style="margin-top: 0.75rem">
          <summary><strong>Pin map</strong></summary>
          <div v-for="(group, key) in selected.pins" :key="key" style="margin-top: 0.75rem">
            <h4 style="margin: 0 0 0.35rem; text-transform: capitalize">{{ key }}</h4>
            <table class="cyd-spec-table">
              <tbody>
                <tr v-for="pin in group" :key="pin.label">
                  <th>{{ pin.label }}</th>
                  <td>GPIO {{ pin.gpio }}<span v-if="pin.notes"> — {{ pin.notes }}</span></td>
                </tr>
              </tbody>
            </table>
          </div>
        </details>

        <details style="margin-top: 0.75rem">
          <summary><strong>References</strong></summary>
          <div style="margin-top: 0.75rem">
            <BoardReferences :board-id="selectedId" />
          </div>
        </details>

        <details style="margin-top: 0.75rem">
          <summary><strong>Board notes</strong></summary>
          <ul style="margin: 0.5rem 0 0; padding-left: 1.2rem">
            <li v-for="(note, i) in selected.notes" :key="i" style="margin-bottom: 0.35rem">{{ note }}</li>
          </ul>
        </details>
      </div>
    </div>
  </div>
</template>
