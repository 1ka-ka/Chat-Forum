import request from './request'
import type { ApiResponse, Post, PaginatedResponse } from '@/types'

export const createPost = (data: FormData) => {
  return request.post<ApiResponse<Post>>('/posts', data, {
    headers: { 'Content-Type': 'multipart/form-data' }
  })
}

export const getPostList = (params: { page?: number; pageSize?: number }) => {
  return request.get<ApiResponse<PaginatedResponse<Post>>>('/posts', { params })
}

export const getPostById = (id: number) => {
  return request.get<ApiResponse<Post>>(`/posts/${id}`)
}
