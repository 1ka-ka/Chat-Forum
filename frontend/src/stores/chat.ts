import { defineStore } from 'pinia'
import { ref } from 'vue'
import type { Message, Conversation } from '@/types'
import { chatApi } from '@/api/chat'
import { wsManager } from '@/utils/websocket'

export const useChatStore = defineStore('chat', () => {
  const conversations = ref<Conversation[]>([])
  const currentMessages = ref<Message[]>([])
  const currentChatUserId = ref<number | null>(null)
  const onlineUsers = ref<Set<number>>(new Set())

  async function fetchConversations() {
    const res = await chatApi.getConversations()
    conversations.value = res.data || []
  }

  async function fetchMessages(userId: number, page: number = 1) {
    const res = await chatApi.getMessages(userId, page)
    if (page === 1) {
      currentMessages.value = res.data.items.reverse()
    } else {
      currentMessages.value = [...res.data.items.reverse(), ...currentMessages.value]
    }
    return res.data
  }

  async function markAsRead(userId: number) {
    await chatApi.markAsRead(userId)
    const conv = conversations.value.find((c) => c.user_id === userId)
    if (conv) {
      conv.unread_count = 0
    }
  }

  function setCurrentChat(userId: number | null) {
    currentChatUserId.value = userId
    if (userId) {
      markAsRead(userId)
    }
  }

  function addMessage(message: Message) {
    currentMessages.value.push(message)
    const otherUserId = message.sender_id === currentChatUserId.value
      ? message.receiver_id
      : message.sender_id
    const conv = conversations.value.find((c) => c.user_id === otherUserId)
    if (conv) {
      conv.last_message = message.content
      conv.last_time = message.created_at
    }
  }

  function handleOnlineStatus(userId: number, online: boolean) {
    if (online) {
      onlineUsers.value.add(userId)
    } else {
      onlineUsers.value.delete(userId)
    }
  }

  function isOnline(userId: number): boolean {
    return onlineUsers.value.has(userId)
  }

  return {
    conversations,
    currentMessages,
    currentChatUserId,
    onlineUsers,
    fetchConversations,
    fetchMessages,
    markAsRead,
    setCurrentChat,
    addMessage,
    handleOnlineStatus,
    isOnline
  }
})
