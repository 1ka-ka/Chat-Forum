export interface User {
  id: number
  username?: string
  nickname: string
  avatar_url: string
  created_at?: string
  post_count?: number
}

export interface Post {
  id: number
  user_id: number
  title: string
  content: string
  image_urls: string[]
  like_count: number
  comment_count: number
  is_liked: boolean
  nickname: string
  avatar_url: string
  created_at: string
}

export interface Comment {
  id: number
  post_id: number
  user_id: number
  content: string
  nickname: string
  avatar_url: string
  created_at: string
}

export interface Message {
  id: number
  sender_id: number
  receiver_id: number
  content: string
  is_read: number
  created_at: string
}

export interface Conversation {
  user_id: number
  nickname: string
  avatar_url: string
  last_message: string
  last_time: string
  unread_count: number
}

export interface ApiResponse<T = any> {
  code: number
  message: string
  data: T
}

export interface PageResult<T> {
  items: T[]
  total: number
  page: number
  page_size: number
  total_pages: number
}

export interface LoginData {
  id: number
  username: string
  nickname: string
  avatar_url: string
  token: string
}
