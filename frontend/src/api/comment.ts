import request from './request'
import type { ApiResponse, Comment, PaginatedResponse } from '@/types'

export const createComment = (postId: number, data: { content: string }) => {
  return request.post<ApiResponse<Comment>>(`/posts/${postId}/comments`, data)
}

export const getComments = (postId: number, params: { page?: number; pageSize?: number }) => {
  return request.get<ApiResponse<PaginatedResponse<Comment>>>(`/posts/${postId}/comments`, { params })
}
