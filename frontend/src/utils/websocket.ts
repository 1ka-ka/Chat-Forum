import { ref, onUnmounted } from 'vue'
import { getToken } from './auth'
import { ElMessage } from 'element-plus'
import type { Message, Conversation } from '@/types'

type MessageHandler = (data: any) => void

class WebSocketManager {
  private ws: WebSocket | null = null
  private reconnectAttempts = 0
  private maxReconnectAttempts = 5
  private reconnectDelay = 1000
  private pingInterval: number | null = null
  private messageHandlers: Set<MessageHandler> = new Set()
  private isConnected = ref(false)

  connect() {
    const token = getToken()
    if (!token) {
      ElMessage.error('请先登录')
      return
    }

    if (this.ws?.readyState === WebSocket.OPEN) {
      return
    }

    const wsUrl = `${window.location.protocol === 'https:' ? 'wss:' : 'ws:'}//${window.location.host}/ws/chat?token=${token}`
    this.ws = new WebSocket(wsUrl)

    this.ws.onopen = () => {
      this.isConnected.value = true
      this.reconnectAttempts = 0
      this.startPing()
    }

    this.ws.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data)
        if (data.type === 'pong') {
          return
        }
        this.messageHandlers.forEach((handler) => handler(data))
      } catch {
        console.error('Failed to parse WebSocket message')
      }
    }

    this.ws.onclose = () => {
      this.isConnected.value = false
      this.stopPing()
      this.attemptReconnect()
    }

    this.ws.onerror = () => {
      this.isConnected.value = false
    }
  }

  private attemptReconnect() {
    if (this.reconnectAttempts < this.maxReconnectAttempts) {
      this.reconnectAttempts++
      const delay = this.reconnectDelay * Math.pow(2, this.reconnectAttempts - 1)
      setTimeout(() => {
        this.connect()
      }, delay)
    }
  }

  private startPing() {
    this.pingInterval = window.setInterval(() => {
      this.send({ type: 'ping' })
    }, 30000)
  }

  private stopPing() {
    if (this.pingInterval) {
      clearInterval(this.pingInterval)
      this.pingInterval = null
    }
  }

  send(data: { type: string; receiver_id?: number; content?: string }) {
    if (this.ws?.readyState === WebSocket.OPEN) {
      this.ws.send(JSON.stringify(data))
    }
  }

  sendMessage(receiverId: number, content: string) {
    this.send({
      type: 'message',
      receiver_id: receiverId,
      content
    })
  }

  onMessage(handler: MessageHandler) {
    this.messageHandlers.add(handler)
    return () => {
      this.messageHandlers.delete(handler)
    }
  }

  disconnect() {
    this.stopPing()
    if (this.ws) {
      this.ws.close()
      this.ws = null
    }
    this.isConnected.value = false
  }

  get connectionState() {
    return this.isConnected
  }
}

export const wsManager = new WebSocketManager()
