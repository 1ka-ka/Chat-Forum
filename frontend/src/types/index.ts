export interface User {
  id: number;
  username?: string;
  nickname: string;
  avatarUrl: string;
  createdAt?: string;
  postCount?: number;
}

export interface AuthResponse {
  id: number;
  username: string;
  nickname: string;
  avatarUrl: string;
  token: string;
}

export interface Post {
  id: number;
  userId: number;
  title: string;
  content: string;
  imageUrls: string[];
  likeCount: number;
  commentCount: number;
  nickname: string;
  avatarUrl: string;
  isLiked: boolean;
  createdAt: string;
}

export interface Comment {
  id: number;
  postId: number;
  content: string;
  userId: number;
  nickname: string;
  avatarUrl: string;
  createdAt: string;
}

export interface Message {
  id: number;
  senderId: number;
  receiverId: number;
  content: string;
  isRead: number;
  createdAt: string;
}

export interface Conversation {
  userId: number;
  nickname: string;
  avatarUrl: string;
  lastMessage: string;
  lastTime: string;
  unreadCount: number;
}

export interface PaginatedResponse<T> {
  items: T[];
  total: number;
  page: number;
  pageSize: number;
  totalPages: number;
}

export interface ApiResponse<T = any> {
  code: number;
  message: string;
  data: T;
}

export interface WsMessage {
  type: 'message' | 'ping' | 'pong' | 'online_status' | 'unread';
  senderId?: number;
  senderNickname?: string;
  senderAvatar?: string;
  receiverId?: number;
  content?: string;
  userId?: number;
  online?: boolean;
  messageId?: number;
  createdAt?: string;
}
