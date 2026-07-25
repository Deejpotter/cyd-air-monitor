<script setup lang="ts">
import DefaultTheme from 'vitepress/theme'
import BoardCardGrid from '../components/BoardCardGrid.vue'

const { Layout } = DefaultTheme
</script>

<template>
  <Layout>
    <template #home-hero-after>
      <div class="home-tiles-wrap">
        <BoardCardGrid home />
      </div>
    </template>
  </Layout>
</template>
