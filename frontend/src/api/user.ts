import request from './request'
import type { ApiResponse, AuthResponse, User } from '@/types'

export const register = (data: { username: string; password: string; nickname: string }) => {
  return request.post<ApiResponse<AuthResponse>>('/auth/register', data)
}

export const login = (data: { username: string; password: string }) => {
  return request.post<ApiResponse<AuthResponse>>('/auth/login', data)
}

export const getProfile = () => {
  return request.get<ApiResponse<User>>('/user/profile')
}

export const updateProfile = (data: FormData) => {
  return request.put<ApiResponse<User>>('/user/profile', data, {
    headers: { 'Content-Type': 'multipart/form-data' }
  })
}

export const getUserById = (id: number) => {
  return request.get<ApiResponse<User>>(`/user/${id}`)
}
