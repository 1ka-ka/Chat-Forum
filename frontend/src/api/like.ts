import request from './request'
import type { ApiResponse } from '@/types'

export const toggleLike = (postId: number) => {
  return request.post<ApiResponse<{ isLiked: boolean; likeCount: number }>>(`/posts/${postId}/like`)
}
