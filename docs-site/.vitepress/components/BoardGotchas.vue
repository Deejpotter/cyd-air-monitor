<script setup lang="ts">
import { computed, ref } from 'vue'
import { boards } from '../data/boards'
import { gotchas, categoryLabels, type GotchaCategory } from '../data/gotchas'

const props = withDefaults(
  defineProps<{
    boardId?: string
    compact?: boolean
  }>(),
  { boardId: '', compact: false }
)

const selectedBoard = ref(props.boardId || 'all')
const selectedCategory = ref<GotchaCategory | 'all'>('all')

const filtered = computed(() => {
  return gotchas.filter((g) => {
    const boardOk =
      selectedBoard.value === 'all' ||
      g.boardIds === 'all' ||
      g.boardIds.includes(selectedBoard.value)
    const catOk = selectedCategory.value === 'all' || g.category === selectedCategory.value
    return boardOk && catOk
  })
})

const severityOrder = { critical: 0, common: 1, tip: 2 }

const sorted = computed(() =>
  [...filtered.value].sort((a, b) => severityOrder[a.severity] - severityOrder[b.severity])
)

const categories = Object.entries(categoryLabels) as [GotchaCategory, string][]

function boardLabel(ids: string[] | 'all'): string {
  if (ids === 'all') return 'All boards'
  return ids.map((id) => boards.find((b) => b.id === id)?.shortName ?? id).join(', ')
}
</script>

<template>
  <div class="gotchas-guide">
    <div v-if="!compact" class="cyd-filter-row">
      <select v-model="selectedBoard" class="cyd-btn" style="min-width: 10rem">
        <option value="all">All boards</option>
        <option v-for="b in boards" :key="b.id" :value="b.id">{{ b.name }}</option>
      </select>
      <button
        v-for="[cat, label] in [['all', 'All topics'], ...categories]"
        :key="cat"
        class="cyd-btn"
        :class="{ 'is-active': selectedCategory === cat }"
        @click="selectedCategory = cat as GotchaCategory | 'all'"
      >
        {{ label }}
      </button>
    </div>

    <p v-if="sorted.length === 0" style="color: var(--vp-c-text-2)">No gotchas match this filter.</p>

    <div v-for="g in sorted" :key="g.id" class="gotcha-card" :class="`gotcha-card--${g.severity}`">
      <div class="gotcha-card__head">
        <span class="gotcha-badge" :class="`gotcha-badge--${g.severity}`">{{ g.severity }}</span>
        <span class="gotcha-badge gotcha-badge--cat">{{ categoryLabels[g.category] }}</span>
        <span class="gotcha-boards">{{ boardLabel(g.boardIds) }}</span>
      </div>
      <h4 class="gotcha-card__title">{{ g.title }}</h4>
      <p class="gotcha-card__body">{{ g.body }}</p>
      <p v-if="g.fix" class="gotcha-card__fix"><strong>Fix:</strong> {{ g.fix }}</p>
    </div>
  </div>
</template>

<style scoped>
.gotcha-card {
  border: 1px solid var(--vp-c-divider);
  border-radius: 10px;
  padding: 0.85rem 1rem;
  margin-bottom: 0.75rem;
  background: var(--vp-c-bg-soft);
}
.gotcha-card--critical {
  border-left: 3px solid #e57373;
}
.gotcha-card--common {
  border-left: 3px solid var(--vp-c-brand-1);
}
.gotcha-card--tip {
  border-left: 3px solid #81c784;
}
.gotcha-card__head {
  display: flex;
  flex-wrap: wrap;
  gap: 0.35rem;
  align-items: center;
  margin-bottom: 0.35rem;
}
.gotcha-card__title {
  margin: 0 0 0.35rem;
  font-size: 1rem;
}
.gotcha-card__body {
  margin: 0 0 0.35rem;
  color: var(--vp-c-text-2);
  font-size: 0.9rem;
  line-height: 1.5;
}
.gotcha-card__fix {
  margin: 0;
  font-size: 0.875rem;
  padding: 0.45rem 0.6rem;
  background: var(--vp-c-brand-soft);
  border-radius: 6px;
}
.gotcha-badge {
  font-size: 0.7rem;
  font-weight: 700;
  text-transform: uppercase;
  letter-spacing: 0.04em;
  padding: 0.1rem 0.45rem;
  border-radius: 4px;
}
.gotcha-badge--critical {
  background: rgba(229, 115, 115, 0.2);
  color: #e57373;
}
.gotcha-badge--common {
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand-1);
}
.gotcha-badge--tip {
  background: rgba(129, 199, 132, 0.15);
  color: #81c784;
}
.gotcha-badge--cat {
  background: rgba(255, 255, 255, 0.06);
  color: var(--vp-c-text-2);
  text-transform: none;
  font-weight: 600;
}
.gotcha-boards {
  font-size: 0.75rem;
  color: var(--vp-c-text-3);
  margin-left: auto;
}
select.cyd-btn {
  appearance: auto;
}
</style>
