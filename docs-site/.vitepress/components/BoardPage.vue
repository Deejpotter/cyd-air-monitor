<script setup lang="ts">
import { computed, ref } from 'vue'
import { boards, getBoard, buildCommands } from '../data/boards'

const props = defineProps<{ boardId: string }>()

const board = computed(() => getBoard(props.boardId))
const copied = ref('')

const commands = computed(() =>
  board.value ? buildCommands(board.value.env, board.value.uploadPort) : []
)

const previewClass = computed(() =>
  board.value?.resolution.startsWith('480')
    ? 'board-hero__preview board-hero__preview--wide'
    : 'board-hero__preview'
)

const touchMirror = computed(() => {
  if (!board.value) return ''
  const b = board.value.build
  const parts: string[] = []
  if (b.touchMirrorX) parts.push('X')
  if (b.touchMirrorY) parts.push('Y')
  return parts.length ? parts.join('+') : 'none'
})

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
</script>

<template>
  <div v-if="board" class="board-page">
    <header class="board-hero">
      <div class="board-hero__content">
        <p class="board-hero__eyebrow">PlatformIO · <code>{{ board.env }}</code></p>
        <h1 class="board-hero__title">{{ board.name }}</h1>
        <p class="board-hero__tagline">{{ board.tagline }}</p>

        <div class="board-hero__tags">
          <span class="board-tag" :class="board.mcu.includes('S3') ? 'board-tag--s3' : 'board-tag--mcu'">{{ board.mcu }}</span>
          <span class="board-tag" :class="board.touch === 'resistive' ? 'board-tag--resistive' : 'board-tag--capacitive'">
            {{ board.touch }} · {{ board.touchChip }}
          </span>
          <span class="board-tag board-tag--display">{{ board.display }}</span>
          <span v-if="board.rgbLed" class="board-tag board-tag--accent">RGB LED</span>
          <span v-if="board.defaultEnv" class="board-tag board-tag--accent">Default env</span>
        </div>

        <ul class="board-hero__highlights">
          <li v-for="(item, i) in board.highlights" :key="i">{{ item }}</li>
        </ul>
      </div>

      <div class="board-hero__aside">
        <div :class="previewClass">
          <span class="board-hero__res">{{ board.resolution }}</span>
        </div>
        <div class="board-flash-box">
          <p class="board-flash-box__label">Flash this board</p>
          <div v-for="(cmd, i) in commands" :key="cmd" class="board-code">
            <code>{{ cmd }}</code>
            <button type="button" class="board-copy" @click="copyText(cmd, `cmd-${i}`)">
              {{ copied === `cmd-${i}` ? 'Copied' : 'Copy' }}
            </button>
          </div>
        </div>
      </div>
    </header>

    <section class="board-section">
      <div class="board-section__head">
        <h2>Hardware</h2>
        <p>What makes this board distinct from the others in the lineup.</p>
      </div>
      <div class="board-feature-grid">
        <article v-for="(feat, i) in board.hardwareFeatures" :key="`hw-${i}`" class="board-feature-card">
          <span class="board-feature-card__icon" aria-hidden="true">{{ feat.icon ?? '◆' }}</span>
          <h3>{{ feat.title }}</h3>
          <p>{{ feat.detail }}</p>
        </article>
      </div>
    </section>

    <section class="board-section board-section--soft">
      <div class="board-section__head">
        <h2>Firmware features</h2>
        <p>What you get after flashing the matching <code>{{ board.env }}</code> build.</p>
      </div>
      <div class="board-feature-grid board-feature-grid--four">
        <article v-for="(feat, i) in board.firmwareFeatures" :key="`fw-${i}`" class="board-feature-card board-feature-card--compact">
          <span class="board-feature-card__icon" aria-hidden="true">{{ feat.icon ?? '◆' }}</span>
          <div>
            <h3>{{ feat.title }}</h3>
            <p>{{ feat.detail }}</p>
          </div>
        </article>
      </div>
    </section>

    <section class="board-section">
      <div class="board-two-col">
        <div>
          <h2>Pin map</h2>
          <div v-for="(group, key) in board.pins" :key="key" class="board-pin-group">
            <h3>{{ key }}</h3>
            <table class="board-table">
              <tbody>
                <tr v-for="pin in group" :key="pin.label">
                  <th>{{ pin.label }}</th>
                  <td>
                    GPIO {{ pin.gpio }}
                    <span v-if="pin.notes" class="board-muted"> — {{ pin.notes }}</span>
                  </td>
                </tr>
              </tbody>
            </table>
          </div>
        </div>

        <div>
          <h2>Build defaults</h2>
          <table class="board-table board-table--spec">
            <tbody>
              <tr><th>Env</th><td><code>{{ board.env }}</code></td></tr>
              <tr><th>Display driver</th><td>{{ board.displayDriver }}</td></tr>
              <tr><th>Resolution</th><td>{{ board.resolution }}</td></tr>
              <tr><th>DHT11 GPIO</th><td>{{ board.dhtPin }}</td></tr>
              <tr><th>Display rotation</th><td>{{ board.build.displayRotation }}<span v-if="board.build.madctlOverride"> + MADCTL {{ board.build.madctlOverride }}</span></td></tr>
              <tr><th>Touch rotation</th><td>{{ board.build.touchRotation }}</td></tr>
              <tr><th>Touch mirror</th><td>{{ touchMirror }}</td></tr>
              <tr v-if="board.build.capNativeLandscape"><th>Cap landscape map</th><td>enabled</td></tr>
              <tr v-if="board.uploadPort"><th>Typical port</th><td><code>{{ board.uploadPort }}</code></td></tr>
            </tbody>
          </table>

          <h3 style="margin-top: 1.5rem">When things go wrong</h3>
          <div v-for="(t, i) in board.troubleshooting" :key="i" class="board-trouble">
            <strong>{{ t.symptom }}</strong>
            <ul>
              <li v-for="(fix, j) in t.fixes" :key="j">{{ fix }}</li>
            </ul>
          </div>
        </div>
      </div>
    </section>

    <section class="board-section board-section--soft">
      <details class="board-details">
        <summary>Gotchas & pitfalls</summary>
        <div class="board-details__body">
          <BoardGotchas :board-id="boardId" compact />
        </div>
      </details>

      <details class="board-details">
        <summary>References & community links</summary>
        <div class="board-details__body">
          <BoardReferences :board-id="boardId" />
          <p class="board-muted" style="margin-top: 1rem">
            <a :href="'/cyd-air-monitor/boards/references'">Full references index →</a>
          </p>
        </div>
      </details>
    </section>

    <nav class="board-nav-siblings">
      <span>Other boards</span>
      <div class="board-nav-siblings__links">
        <a
          v-for="b in boards"
          :key="b.id"
          :href="`/cyd-air-monitor/boards/${b.id}`"
          :class="{ 'is-current': b.id === boardId }"
        >
          {{ b.shortName }}
        </a>
      </div>
    </nav>
  </div>

  <p v-else>Board not found: {{ boardId }}</p>
</template>
