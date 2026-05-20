<script setup lang="ts">
import { onMounted } from 'vue'
import { RouterView } from 'vue-router'
import NavBar from './components/NavBar.vue'
import { useUserStore } from '@/stores/user'
import { isLoggedIn } from '@/utils/auth'

const userStore = useUserStore()

onMounted(async () => {
  if (isLoggedIn() && !userStore.user) {
    await userStore.fetchProfile()
  }
})
</script>

<template>
  <div class="app-container">
    <NavBar />
    <main class="main-content">
      <RouterView />
    </main>
  </div>
</template>

<style scoped>
.app-container {
  min-height: 100vh;
  background-color: #f5f5f5;
}

.main-content {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}
</style>
