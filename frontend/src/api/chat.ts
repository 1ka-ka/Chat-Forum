import request from './request'
import type { ApiResponse, Conversation, Message, PaginatedResponse } from '@/types'

export const getConversations = () => {
  return request.get<ApiResponse<Conversation[]>>('/chat/conversations')
}

export const getMessages = (userId: number, params: { page?: number; pageSize?: number }) => {
  return request.get<ApiResponse<PaginatedResponse<Message>>>(`/chat/messages/${userId}`, { params })
}

export const markAsRead = (userId: number) => {
  return request.put<ApiResponse>(`/chat/read/${userId}`)
}
