<template>
  <el-header class="navbar">
    <div class="nav-content">
      <div class="nav-left">
        <router-link to="/" class="logo">ChatForum</router-link>
      </div>
      <div class="nav-right">
        <template v-if="userStore.isLoggedIn">
          <router-link to="/create" class="nav-link">
            <el-button type="primary">发帖</el-button>
          </router-link>
          <router-link to="/chat" class="nav-link">
            <el-button text>消息</el-button>
          </router-link>
          <el-dropdown @command="handleCommand">
            <span class="nav-link user-link">
              <UserAvatar :user="userStore.userInfo" />
              {{ userStore.userInfo?.nickname }}
            </span>
            <template #dropdown>
              <el-dropdown-menu>
                <el-dropdown-item command="profile">个人中心</el-dropdown-item>
                <el-dropdown-item command="logout" divided>退出登录</el-dropdown-item>
              </el-dropdown-menu>
            </template>
          </el-dropdown>
        </template>
        <template v-else>
          <router-link to="/login" class="nav-link">
            <el-button>登录</el-button>
          </router-link>
          <router-link to="/register" class="nav-link">
            <el-button type="primary">注册</el-button>
          </router-link>
        </template>
      </div>
    </div>
  </el-header>
</template>

<script setup lang="ts">
import { useRouter } from 'vue-router'
import { useUserStore } from '@/stores/user'
import { ElMessageBox } from 'element-plus'
import UserAvatar from './UserAvatar.vue'

const router = useRouter()
const userStore = useUserStore()

const handleCommand = (command: string) => {
  if (command === 'profile') {
    router.push('/profile')
  } else if (command === 'logout') {
    ElMessageBox.confirm('确定要退出登录吗？', '提示', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning'
    }).then(() => {
      userStore.logout()
      router.push('/login')
    }).catch(() => {})
  }
}
</script>

<style scoped>
.navbar {
  background: #fff;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
  padding: 0;
  height: 64px;
}

.nav-content {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 20px;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.logo {
  font-size: 24px;
  font-weight: bold;
  color: #409eff;
  text-decoration: none;
}

.nav-right {
  display: flex;
  align-items: center;
  gap: 16px;
}

.nav-link {
  text-decoration: none;
  color: #606266;
  cursor: pointer;
}

.user-link {
  display: flex;
  align-items: center;
  gap: 8px;
}
</style>
