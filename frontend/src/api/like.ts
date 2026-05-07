import request from './request'
import type { ApiResponse } from '@/types'

export interface LikeResult {
  is_liked: boolean
  like_count: number
}

export const likeApi = {
  toggleLike(postId: number) {
    return request.post<ApiResponse<LikeResult>>(`/posts/${postId}/like`)
  }
}
