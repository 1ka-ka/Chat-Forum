import request from './request'
import type { ApiResponse, Comment, PageResult } from '@/types'

export const commentApi = {
  createComment(postId: number, content: string) {
    return request.post<ApiResponse<Comment>>(`/posts/${postId}/comments`, {
      content
    })
  },

  getComments(postId: number, page: number = 1, pageSize: number = 20) {
    return request.get<ApiResponse<PageResult<Comment>>>(`/posts/${postId}/comments`, {
      params: { page, page_size: pageSize }
    })
  }
}
