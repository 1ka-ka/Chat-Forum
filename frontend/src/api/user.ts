import request from './request'
import type { ApiResponse, LoginData, User } from '@/types'

export const userApi = {
  register(username: string, password: string, nickname: string) {
    return request.post<ApiResponse<LoginData>>('/auth/register', {
      username,
      password,
      nickname
    })
  },

  login(username: string, password: string) {
    return request.post<ApiResponse<LoginData>>('/auth/login', {
      username,
      password
    })
  },

  getProfile() {
    return request.get<ApiResponse<User>>('/user/profile')
  },

  getUserById(userId: number) {
    return request.get<ApiResponse<User>>(`/user/${userId}`)
  },

  updateProfile(data: { nickname?: string; avatar?: File }) {
    const formData = new FormData()
    if (data.nickname) {
      formData.append('nickname', data.nickname)
    }
    if (data.avatar) {
      formData.append('avatar', data.avatar)
    }
    return request.put<ApiResponse<User>>('/user/profile', formData, {
      headers: { 'Content-Type': 'multipart/form-data' }
    })
  }
}
