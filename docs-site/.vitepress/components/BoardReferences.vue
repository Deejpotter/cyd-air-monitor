<script setup lang="ts">
import { referencesForBoard } from '../data/references'

const props = defineProps<{ boardId: string }>()
const links = referencesForBoard(props.boardId)
</script>

<template>
  <ReferencesList :board-id="boardId" compact />
  <p v-if="links.length === 0" style="color: var(--vp-c-text-2); font-size: 0.9rem">
    See the <a href="/boards/references">full references index</a>.
  </p>
</template>
