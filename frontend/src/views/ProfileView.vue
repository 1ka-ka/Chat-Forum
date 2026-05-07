<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { ElAvatar, ElButton, ElInput, ElMessage, ElSpin } from 'element-plus'
import { useUserStore } from '@/stores/user'

const userStore = useUserStore()

const nickname = ref('')
const avatarFile = ref<File | null>(null)
const avatarPreview = ref('')
const loading = ref(false)

function handleAvatarChange(e: Event) {
  const target = e.target as HTMLInputElement
  if (target.files && target.files[0]) {
    avatarFile.value = target.files[0]
    avatarPreview.value = URL.createObjectURL(target.files[0])
  }
}

async function handleSave() {
  if (!nickname.value.trim()) {
    ElMessage.warning('请输入昵称')
    return
  }

  loading.value = true
  try {
    await userStore.updateProfile({
      nickname: nickname.value,
      avatar: avatarFile.value || undefined
    })
    ElMessage.success('更新成功')
  } catch {
    // error handled by interceptor
  } finally {
    loading.value = false
  }
}

onMounted(() => {
  if (userStore.user) {
    nickname.value = userStore.user.nickname
    avatarPreview.value = userStore.user.avatar_url
  }
})
</script>

<template>
  <div class="profile-view">
    <div class="profile-card">
      <h1>个人中心</h1>

      <ElSpin :spinning="loading">
        <div class="avatar-section">
          <ElAvatar :size="100" :src="avatarPreview || '/default-avatar.png'" />
          <div class="avatar-upload">
            <input type="file" accept="image/*" @change="handleAvatarChange" />
            <span>点击更换头像</span>
          </div>
        </div>

        <div class="form-section">
          <div class="form-item">
            <label>用户名</label>
            <span class="username">{{ userStore.user?.username }}</span>
          </div>

          <div class="form-item">
            <label>昵称</label>
            <ElInput v-model="nickname" placeholder="请输入昵称" />
          </div>

          <div class="form-item">
            <label>注册时间</label>
            <span>{{ userStore.user?.created_at || '-' }}</span>
          </div>
        </div>

        <div class="actions">
          <ElButton type="primary" :loading="loading" @click="handleSave">
            保存修改
          </ElButton>
        </div>
      </ElSpin>
    </div>
  </div>
</template>

<style scoped>
.profile-view {
  max-width: 600px;
  margin: 0 auto;
}

.profile-card {
  background: white;
  border-radius: 12px;
  padding: 30px;
}

.profile-card h1 {
  text-align: center;
  margin-bottom: 30px;
  color: #333;
}

.avatar-section {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 16px;
  margin-bottom: 30px;
}

.avatar-upload {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 4px;
  cursor: pointer;
}

.avatar-upload input {
  width: 80px;
  opacity: 0;
  cursor: pointer;
}

.avatar-upload span {
  font-size: 12px;
  color: #409eff;
}

.form-section {
  display: flex;
  flex-direction: column;
  gap: 20px;
  margin-bottom: 30px;
}

.form-item {
  display: flex;
  align-items: center;
  gap: 12px;
}

.form-item label {
  width: 80px;
  color: #666;
}

.form-item .username {
  color: #333;
}

.actions {
  display: flex;
  justify-content: center;
}
</style>
