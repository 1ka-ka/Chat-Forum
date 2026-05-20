<script setup lang="ts">
import { computed, onMounted, ref } from 'vue'
import { RouterLink, useRouter } from 'vue-router'
import { useUserStore } from '@/stores/user'
import { ElDropdown, ElDropdownMenu, ElDropdownItem, ElAvatar, ElBadge, ElButton } from 'element-plus'
import { isLoggedIn } from '@/utils/auth'
import { notificationApi } from '@/api/notification'

const router = useRouter()
const userStore = useUserStore()

const isAuthenticated = computed(() => !!userStore.user && isLoggedIn())
const user = computed(() => userStore.user)

const unreadCount = ref(0)

async function fetchUnreadCount() {
  if (!isLoggedIn()) return
  try {
    const res = await notificationApi.getUnreadCount()
    unreadCount.value = res.data.count
  } catch {
  }
}

function handleCommand(command: string) {
  if (command === 'profile') {
    router.push('/profile')
  } else if (command === 'chat') {
    router.push('/chat')
  } else if (command === 'notifications') {
    router.push('/notifications')
  } else if (command === 'logout') {
    userStore.logout()
    router.push('/')
  }
}

onMounted(() => {
  fetchUnreadCount()
  setInterval(fetchUnreadCount, 30000)
})
</script>

<template>
  <nav class="navbar">
    <div class="navbar-container">
      <RouterLink to="/" class="navbar-brand">畅谈</RouterLink>

      <div class="navbar-menu">
        <template v-if="isAuthenticated && user">
          <ElBadge :value="unreadCount" :hidden="unreadCount === 0" :max="99">
            <ElButton text @click="router.push('/notifications')" style="font-size: 18px">
              🔔
            </ElButton>
          </ElBadge>
          <ElDropdown @command="handleCommand" trigger="click">
            <div class="user-info">
              <ElAvatar :size="32" :src="user.avatar_url || '/default-avatar.png'" />
              <span class="username">{{ user.nickname }}</span>
            </div>
            <template #dropdown>
              <ElDropdownMenu>
                <ElDropdownItem command="profile">个人中心</ElDropdownItem>
                <ElDropdownItem command="chat">私信</ElDropdownItem>
                <ElDropdownItem command="notifications">消息通知</ElDropdownItem>
                <ElDropdownItem command="logout" divided>退出登录</ElDropdownItem>
              </ElDropdownMenu>
            </template>
          </ElDropdown>
        </template>
        <template v-else>
          <RouterLink to="/login" class="nav-link">登录</RouterLink>
          <RouterLink to="/register" class="nav-link">注册</RouterLink>
        </template>
      </div>
    </div>
  </nav>
</template>

<style scoped>
.navbar {
  background: white;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
  position: sticky;
  top: 0;
  z-index: 100;
}

.navbar-container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 20px;
  height: 60px;
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.navbar-brand {
  font-size: 20px;
  font-weight: bold;
  color: #409eff;
}

.navbar-menu {
  display: flex;
  align-items: center;
  gap: 16px;
}

.nav-link {
  color: #666;
  font-size: 14px;
}

.nav-link:hover {
  color: #409eff;
}

.user-info {
  display: flex;
  align-items: center;
  gap: 8px;
  cursor: pointer;
}

.username {
  font-size: 14px;
  color: #333;
}
</style>
