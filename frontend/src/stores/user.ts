import { ref, computed } from 'vue'
import { defineStore } from 'pinia'
import { getToken, setToken, removeToken, getUser, setUser, removeUser } from '@/utils/auth'
import type { User, AuthResponse } from '@/types'

export const useUserStore = defineStore('user', () => {
  const token = ref<string | null>(getToken())
  const userInfo = ref<User | null>(getUser())

  const isLoggedIn = computed(() => !!token.value)
  const userId = computed(() => userInfo.value?.id)

  const setAuth = (data: AuthResponse) => {
    token.value = data.token
    userInfo.value = {
      id: data.id,
      username: data.username,
      nickname: data.nickname,
      avatarUrl: data.avatarUrl
    }
    setToken(data.token)
    setUser(userInfo.value)
  }

  const updateUserInfo = (info: Partial<User>) => {
    if (userInfo.value) {
      userInfo.value = { ...userInfo.value, ...info }
      setUser(userInfo.value)
    }
  }

  const logout = () => {
    token.value = null
    userInfo.value = null
    removeToken()
    removeUser()
  }

  return {
    token,
    userInfo,
    isLoggedIn,
    userId,
    setAuth,
    updateUserInfo,
    logout
  }
})
