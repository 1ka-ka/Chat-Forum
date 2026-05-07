import { ref, onMounted, onUnmounted } from 'vue'
import { defineStore } from 'pinia'
import { getToken } from '@/utils/auth'
import type { WsMessage, Message, Conversation } from '@/types'

export const useChatStore = defineStore('chat', () => {
  const ws = ref<WebSocket | null>(null)
  const isConnected = ref(false)
  const reconnectAttempts = ref(0)
  const maxReconnectAttempts = 5
  const reconnectDelay = ref(1000)
  const pingInterval = ref<any>(null)
  const conversations = ref<Conversation[]>([])
  const messages = ref<Record<number, Message[]>>({})
  const onlineUsers = ref<number[]>([])

  const connect = () => {
    const token = getToken()
    if (!token) return

    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:'
    const url = `${protocol}//${window.location.host}/ws/chat?token=${token}`

    try {
      ws.value = new WebSocket(url)

      ws.value.onopen = () => {
        console.log('WebSocket connected')
        isConnected.value = true
        reconnectAttempts.value = 0
        startPing()
      }

      ws.value.onmessage = (event) => {
        try {
          const data: WsMessage = JSON.parse(event.data)
          handleMessage(data)
        } catch (e) {
          console.error('Failed to parse message:', e)
        }
      }

      ws.value.onclose = () => {
        console.log('WebSocket disconnected')
        isConnected.value = false
        stopPing()
        scheduleReconnect()
      }

      ws.value.onerror = (error) => {
        console.error('WebSocket error:', error)
      }
    } catch (e) {
      console.error('Failed to connect:', e)
      scheduleReconnect()
    }
  }

  const disconnect = () => {
    if (ws.value) {
      ws.value.close()
      ws.value = null
    }
    isConnected.value = false
    stopPing()
  }

  const startPing = () => {
    pingInterval.value = setInterval(() => {
      send({ type: 'ping' })
    }, 30000)
  }

  const stopPing = () => {
    if (pingInterval.value) {
      clearInterval(pingInterval.value)
      pingInterval.value = null
    }
  }

  const scheduleReconnect = () => {
    if (reconnectAttempts.value < maxReconnectAttempts) {
      const delay = reconnectDelay.value * Math.pow(2, reconnectAttempts.value)
      setTimeout(() => {
        reconnectAttempts.value++
        connect()
      }, delay)
    }
  }

  const handleMessage = (data: WsMessage) => {
    switch (data.type) {
      case 'pong':
        break
      case 'message':
        if (data.senderId) {
          if (!messages.value[data.senderId]) {
            messages.value[data.senderId] = []
          }
          messages.value[data.senderId].push({
            id: data.messageId || Date.now(),
            senderId: data.senderId,
            receiverId: 0,
            content: data.content || '',
            isRead: 1,
            createdAt: data.createdAt || new Date().toISOString()
          } as Message)
        }
        break
      case 'online_status':
        if (data.userId) {
          if (data.online) {
            if (!onlineUsers.value.includes(data.userId)) {
              onlineUsers.value.push(data.userId)
            }
          } else {
            const idx = onlineUsers.value.indexOf(data.userId)
            if (idx > -1) {
              onlineUsers.value.splice(idx, 1)
            }
          }
        }
        break
    }
  }

  const send = (data: WsMessage) => {
    if (ws.value && isConnected.value) {
      ws.value.send(JSON.stringify(data))
    }
  }

  const sendMessage = (receiverId: number, content: string) => {
    send({
      type: 'message',
      receiverId,
      content
    })
  }

  const addMessage = (userId: number, message: Message) => {
    if (!messages.value[userId]) {
      messages.value[userId] = []
    }
    messages.value[userId].push(message)
  }

  const setConversations = (list: Conversation[]) => {
    conversations.value = list
  }

  const setMessages = (userId: number, list: Message[]) => {
    messages.value[userId] = list
  }

  const isUserOnline = (userId: number) => {
    return onlineUsers.value.includes(userId)
  }

  onMounted(() => {
    if (getToken()) {
      connect()
    }
  })

  onUnmounted(() => {
    disconnect()
  })

  return {
    isConnected,
    conversations,
    messages,
    onlineUsers,
    connect,
    disconnect,
    sendMessage,
    addMessage,
    setConversations,
    setMessages,
    isUserOnline
  }
})
