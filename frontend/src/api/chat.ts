import request from './request'
import type { ApiResponse, Conversation, Message, PageResult } from '@/types'

export const chatApi = {
  getConversations() {
    return request.get<ApiResponse<Conversation[]>>('/chat/conversations')
  },

  getMessages(userId: number, page: number = 1, pageSize: number = 20) {
    return request.get<ApiResponse<PageResult<Message>>>(`/chat/messages/${userId}`, {
      params: { page, page_size: pageSize }
    })
  },

  markAsRead(userId: number) {
    return request.put<ApiResponse>(`/chat/read/${userId}`)
  }
}
