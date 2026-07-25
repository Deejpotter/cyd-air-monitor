<script setup lang="ts">
import { computed, ref } from 'vue'
import { boards } from '../data/boards'

const sortKey = ref<'name' | 'resolution' | 'touch'>('name')
const copied = ref('')

const sorted = computed(() => {
  const list = [...boards]
  list.sort((a, b) => {
    if (sortKey.value === 'resolution') return b.resolution.localeCompare(a.resolution)
    if (sortKey.value === 'touch') return a.touch.localeCompare(b.touch)
    return a.name.localeCompare(b.name)
  })
  return list
})

async function copyEnv(env: string) {
  await navigator.clipboard.writeText(`pio run -e ${env} --target upload`)
  copied.value = env
  setTimeout(() => (copied.value = ''), 1600)
}
</script>

<template>
  <div>
    <div class="cyd-filter-row">
      <span style="align-self: center; color: var(--vp-c-text-2); font-size: 0.875rem">Sort by:</span>
      <button class="cyd-btn" :class="{ 'is-active': sortKey === 'name' }" @click="sortKey = 'name'">Name</button>
      <button class="cyd-btn" :class="{ 'is-active': sortKey === 'resolution' }" @click="sortKey = 'resolution'">Resolution</button>
      <button class="cyd-btn" :class="{ 'is-active': sortKey === 'touch' }" @click="sortKey = 'touch'">Touch type</button>
    </div>

    <div style="overflow-x: auto">
      <table class="cyd-spec-table" style="min-width: 640px">
        <thead>
          <tr>
            <th>Board</th>
            <th>Env</th>
            <th>MCU</th>
            <th>Display</th>
            <th>Touch</th>
            <th>DHT</th>
            <th></th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="b in sorted" :key="b.id">
            <td><strong>{{ b.shortName }}</strong><br /><span style="font-size: 0.8rem; color: var(--vp-c-text-2)">{{ b.name }}</span></td>
            <td><code>{{ b.env }}</code></td>
            <td>{{ b.mcu }}</td>
            <td>{{ b.resolution }}<br /><span style="font-size: 0.8rem; color: var(--vp-c-text-2)">{{ b.displayDriver }}</span></td>
            <td>
              <span class="cyd-tag" :class="b.touch === 'resistive' ? 'cyd-tag--resistive' : 'cyd-tag--capacitive'">{{ b.touchChip }}</span>
            </td>
            <td>GPIO {{ b.dhtPin }}</td>
            <td>
              <button class="cyd-btn cyd-btn--copy" @click="copyEnv(b.env)">
                {{ copied === b.env ? 'Copied!' : 'Copy upload cmd' }}
              </button>
            </td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</template>

<style scoped>
.cyd-spec-table thead th {
  border-bottom: 2px solid var(--vp-c-divider);
  padding-bottom: 0.5rem;
}
</style>
