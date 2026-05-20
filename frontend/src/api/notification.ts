import request from './request'
import type { ApiResponse, Notification, PageResult } from '@/types'

export const notificationApi = {
  getNotifications(page: number = 1, pageSize: number = 20) {
    return request.get<ApiResponse<PageResult<Notification>>>('/notifications', {
      params: { page, page_size: pageSize }
    })
  },

  getUnreadCount() {
    return request.get<ApiResponse<{ count: number }>>('/notifications/unread_count')
  },

  markRead(notificationId: number) {
    return request.put<ApiResponse>(`/notifications/read/${notificationId}`)
  },

  markAllRead() {
    return request.put<ApiResponse>('/notifications/read_all')
  }
}
