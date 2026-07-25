<script setup lang="ts">
import { boards } from '../data/boards'

withDefaults(
  defineProps<{
    highlight?: string
    home?: boolean
  }>(),
  { highlight: '', home: false }
)
</script>

<template>
  <div class="board-card-grid" :class="{ 'board-card-grid--home': home }">
    <a
      v-for="board in boards"
      :key="board.id"
      :href="`/cyd-air-monitor/boards/${board.id}`"
      class="board-card"
      :class="{
        'board-card--highlight': highlight === board.id || (highlight === '' && board.defaultEnv && !home),
        'board-card--home': home,
      }"
    >
      <div v-if="home" class="board-card__visual" :class="{ 'board-card__visual--wide': board.resolution.startsWith('480') }">
        <span class="board-card__res">{{ board.resolution }}</span>
      </div>
      <div class="board-card__body">
        <div class="board-card__top">
          <span class="board-card__env">{{ board.env }}</span>
          <span
            class="board-tag board-tag--sm"
            :class="board.touch === 'resistive' ? 'board-tag--resistive' : 'board-tag--capacitive'"
          >
            {{ board.touch }}
          </span>
        </div>
        <h3 class="board-card__title">{{ board.shortName }}</h3>
        <p class="board-card__name">{{ board.name }}</p>
        <p v-if="!home" class="board-card__tagline">{{ board.tagline }}</p>
        <ul class="board-card__specs">
          <li>{{ board.display }}</li>
          <li>{{ board.mcu }}</li>
          <li>DHT GPIO {{ board.dhtPin }}</li>
        </ul>
        <span class="board-card__cta">Open board page →</span>
      </div>
    </a>
  </div>
</template>
