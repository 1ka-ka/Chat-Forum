<template>
  <div class="profile-page">
    <el-card>
      <template #header>
        <h2>个人中心</h2>
      </template>
      <div class="profile-info">
        <div class="avatar-section">
          <UserAvatar :user="user" :size="100" />
          <el-upload
            :show-file-list="false"
            :before-upload="beforeUpload"
            :on-change="handleAvatarChange"
            :auto-upload="false"
            accept="image/*"
          >
            <el-button>更换头像</el-button>
          </el-upload>
        </div>
        <el-form :model="form" label-width="100px">
          <el-form-item label="用户名">
            <el-input v-model="form.username" disabled />
          </el-form-item>
          <el-form-item label="昵称">
            <el-input v-model="form.nickname" />
          </el-form-item>
          <el-form-item>
            <el-button type="primary" @click="handleSave" :loading="loading">保存</el-button>
          </el-form-item>
        </el-form>
      </div>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive, onMounted } from 'vue'
import { ElMessage } from 'element-plus'
import { useUserStore } from '@/stores/user'
import { getProfile, updateProfile } from '@/api/user'
import UserAvatar from '@/components/UserAvatar.vue'

const userStore = useUserStore()

const user = ref(userStore.userInfo)
const loading = ref(false)
const newAvatar = ref<File | null>(null)

const form = reactive({
  username: '',
  nickname: ''
})

const loadProfile = async () => {
  try {
    const res = await getProfile()
    form.username = res.data.username || ''
    form.nickname = res.data.nickname
    user.value = res.data
  } catch (e) {}
}

const beforeUpload = (file: File) => {
  const isImg = file.type.startsWith('image/')
  if (!isImg) {
    ElMessage.error('只能上传图片')
    return false
  }
  const isLt = file.size / 1024 / 1024 < 2
  if (!isLt) {
    ElMessage.error('图片大小不能超过2MB')
    return false
  }
  newAvatar.value = file
  return false
}

const handleAvatarChange = (file: any) => {
  if (file.raw) {
    newAvatar.value = file.raw
    const reader = new FileReader()
    reader.onload = (e) => {
      if (user.value) {
        user.value.avatarUrl = e.target?.result as string
      }
    }
    reader.readAsDataURL(file.raw)
  }
}

const handleSave = async () => {
  loading.value = true
  try {
    const formData = new FormData()
    formData.append('nickname', form.nickname)
    if (newAvatar.value) {
      formData.append('avatar', newAvatar.value)
    }
    const res = await updateProfile(formData)
    userStore.updateUserInfo(res.data)
    ElMessage.success('保存成功')
  } catch (e) {
  } finally {
    loading.value = false
  }
}

onMounted(() => {
  loadProfile()
})
</script>

<style scoped>
.profile-page {
  max-width: 600px;
  margin: 0 auto;
}

.profile-page h2 {
  margin: 0;
  color: #303133;
}

.profile-info {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 20px 0;
}

.avatar-section {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 16px;
  margin-bottom: 24px;
}
</style>
