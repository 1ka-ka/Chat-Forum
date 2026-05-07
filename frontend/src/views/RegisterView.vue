<script setup lang="ts">
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { ElForm, ElFormItem, ElInput, ElButton, ElMessage } from 'element-plus'
import { userApi } from '@/api/user'
import { useUserStore } from '@/stores/user'

const router = useRouter()
const userStore = useUserStore()

const form = ref({
  username: '',
  password: '',
  nickname: ''
})
const loading = ref(false)

async function handleRegister() {
  if (!form.value.username || !form.value.password || !form.value.nickname) {
    ElMessage.warning('请填写所有字段')
    return
  }

  loading.value = true
  try {
    const res = await userApi.register(
      form.value.username,
      form.value.password,
      form.value.nickname
    )
    userStore.login(
      {
        id: res.data.id,
        username: res.data.username,
        nickname: res.data.nickname,
        avatar_url: ''
      },
      res.data.token
    )
    ElMessage.success('注册成功')
    router.push('/')
  } catch {
    // error handled by interceptor
  } finally {
    loading.value = false
  }
}
</script>

<template>
  <div class="register-container">
    <div class="register-card">
      <h2 class="register-title">注册 ChatForum</h2>
      <ElForm @submit.prevent="handleRegister">
        <ElFormItem>
          <ElInput
            v-model="form.username"
            placeholder="用户名（3-50位字母数字下划线）"
            size="large"
          />
        </ElFormItem>
        <ElFormItem>
          <ElInput
            v-model="form.nickname"
            placeholder="昵称（1-50位字符）"
            size="large"
          />
        </ElFormItem>
        <ElFormItem>
          <ElInput
            v-model="form.password"
            type="password"
            placeholder="密码（6-50位）"
            size="large"
          />
        </ElFormItem>
        <ElFormItem>
          <ElButton
            type="primary"
            size="large"
            :loading="loading"
            native-type="submit"
            style="width: 100%"
          >
            注册
          </ElButton>
        </ElFormItem>
      </ElForm>
      <div class="register-footer">
        已有账号？<router-link to="/login">立即登录</router-link>
      </div>
    </div>
  </div>
</template>

<style scoped>
.register-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: calc(100vh - 60px);
}

.register-card {
  width: 400px;
  padding: 40px;
  background: white;
  border-radius: 12px;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.1);
}

.register-title {
  text-align: center;
  margin-bottom: 30px;
  color: #333;
}

.register-footer {
  text-align: center;
  margin-top: 20px;
  color: #666;
}

.register-footer a {
  color: #409eff;
}
</style>
