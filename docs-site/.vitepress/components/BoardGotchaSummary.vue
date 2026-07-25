<script setup lang="ts">
import { gotchasForBoard } from '../data/gotchas'

const props = defineProps<{ boardId: string }>()

const items = gotchasForBoard(props.boardId)
const critical = items.filter((g) => g.severity === 'critical')
</script>

<template>
  <div v-if="critical.length" class="board-gotcha-alert">
    <strong>Before you flash</strong>
    <ul>
      <li v-for="g in critical" :key="g.id">{{ g.title }}</li>
    </ul>
  </div>
  <BoardGotchas :board-id="boardId" compact />
</template>

<style scoped>
.board-gotcha-alert {
  background: rgba(229, 115, 115, 0.1);
  border: 1px solid rgba(229, 115, 115, 0.35);
  border-radius: 10px;
  padding: 0.75rem 1rem;
  margin-bottom: 1rem;
  font-size: 0.9rem;
}
.board-gotcha-alert ul {
  margin: 0.35rem 0 0;
  padding-left: 1.2rem;
}
</style>
