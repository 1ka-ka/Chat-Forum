import { defineStore } from 'pinia'
import { ref } from 'vue'
import type { User } from '@/types'
import { setToken, removeToken, isLoggedIn } from '@/utils/auth'
import { userApi } from '@/api/user'

export const useUserStore = defineStore('user', () => {
  const user = ref<User | null>(null)
  const token = ref<string | null>(null)

  function setUser(userData: User) {
    user.value = userData
  }

  function setUserToken(userToken: string) {
    token.value = userToken
    setToken(userToken)
  }

  function login(userData: User, userToken: string) {
    user.value = userData
    token.value = userToken
    setToken(userToken)
  }

  function logout() {
    user.value = null
    token.value = null
    removeToken()
  }

  async function fetchProfile() {
    if (!isLoggedIn()) return
    try {
      const res = await userApi.getProfile()
      user.value = res.data
    } catch {
      logout()
    }
  }

  async function updateProfile(data: { nickname?: string; avatar?: File }) {
    const res = await userApi.updateProfile(data)
    user.value = res.data
  }

  return {
    user,
    token,
    setUser,
    setUserToken,
    login,
    logout,
    fetchProfile,
    updateProfile
  }
})
