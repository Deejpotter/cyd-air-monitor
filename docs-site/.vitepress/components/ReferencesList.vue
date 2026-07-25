<script setup lang="ts">
import { computed, ref } from 'vue'
import { boards } from '../data/boards'
import {
  references,
  referenceCategoryLabels,
  type ReferenceCategory,
} from '../data/references'

const props = withDefaults(
  defineProps<{
    boardId?: string
    compact?: boolean
  }>(),
  { boardId: '', compact: false }
)

const selectedBoard = ref(props.boardId || 'all')
const selectedCategory = ref<ReferenceCategory | 'all'>('all')

const filtered = computed(() => {
  return references.filter((r) => {
    const boardOk =
      selectedBoard.value === 'all' ||
      r.boardIds === 'all' ||
      r.boardIds?.includes(selectedBoard.value)
    const catOk = selectedCategory.value === 'all' || r.category === selectedCategory.value
    return boardOk && catOk
  })
})

const grouped = computed(() => {
  const groups = new Map<ReferenceCategory, typeof references>()
  for (const ref of filtered.value) {
    if (!groups.has(ref.category)) groups.set(ref.category, [])
    groups.get(ref.category)!.push(ref)
  }
  return groups
})

const categories = Object.entries(referenceCategoryLabels) as [ReferenceCategory, string][]
</script>

<template>
  <div class="references-list">
    <div v-if="!compact" class="cyd-filter-row">
      <select v-model="selectedBoard" class="cyd-btn" style="min-width: 11rem">
        <option value="all">All boards</option>
        <option v-for="b in boards" :key="b.id" :value="b.id">{{ b.shortName }}</option>
      </select>
      <button
        class="cyd-btn"
        :class="{ 'is-active': selectedCategory === 'all' }"
        @click="selectedCategory = 'all'"
      >
        All topics
      </button>
      <button
        v-for="[cat, label] in categories"
        :key="cat"
        class="cyd-btn"
        :class="{ 'is-active': selectedCategory === cat }"
        @click="selectedCategory = cat"
      >
        {{ label }}
      </button>
    </div>

    <p v-if="filtered.length === 0" style="color: var(--vp-c-text-2)">No references match this filter.</p>

    <div v-for="[cat, items] in grouped" :key="cat" class="ref-group">
      <h3 v-if="!compact || grouped.size > 1" class="ref-group__title">{{ referenceCategoryLabels[cat] }}</h3>
      <ul class="ref-list">
        <li v-for="item in items" :key="item.id" class="ref-item">
          <a :href="item.url" target="_blank" rel="noopener noreferrer" class="ref-item__link">{{ item.title }}</a>
          <p class="ref-item__desc">{{ item.description }}</p>
          <span v-if="item.tags?.length" class="ref-tags">
            <span v-for="tag in item.tags" :key="tag" class="cyd-tag" style="font-size: 0.65rem">{{ tag }}</span>
          </span>
        </li>
      </ul>
    </div>
  </div>
</template>

<style scoped>
.ref-group {
  margin-bottom: 1.5rem;
}
.ref-group__title {
  margin: 0 0 0.5rem;
  font-size: 0.95rem;
  color: var(--vp-c-brand-1);
  border-bottom: 1px solid var(--vp-c-divider);
  padding-bottom: 0.35rem;
}
.ref-list {
  list-style: none;
  padding: 0;
  margin: 0;
}
.ref-item {
  margin-bottom: 0.85rem;
  padding-left: 0;
}
.ref-item__link {
  font-weight: 600;
  font-size: 0.95rem;
}
.ref-item__desc {
  margin: 0.15rem 0 0.25rem;
  font-size: 0.85rem;
  color: var(--vp-c-text-2);
  line-height: 1.45;
}
.ref-tags {
  display: flex;
  flex-wrap: wrap;
  gap: 0.25rem;
}
select.cyd-btn {
  appearance: auto;
}
</style>
