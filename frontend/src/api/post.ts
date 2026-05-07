import request from './request'
import type { ApiResponse, Post, PageResult } from '@/types'

export const postApi = {
  createPost(data: { title: string; content: string; images?: File[] }) {
    const formData = new FormData()
    formData.append('title', data.title)
    formData.append('content', data.content)
    if (data.images) {
      data.images.forEach((img) => {
        formData.append('images', img)
      })
    }
    return request.post<ApiResponse<Post>>('/posts', formData, {
      headers: { 'Content-Type': 'multipart/form-data' }
    })
  },

  getPostList(page: number = 1, pageSize: number = 20) {
    return request.get<ApiResponse<PageResult<Post>>>('/posts', {
      params: { page, page_size: pageSize }
    })
  },

  getPostById(postId: number) {
    return request.get<ApiResponse<Post>>(`/posts/${postId}`)
  }
}
