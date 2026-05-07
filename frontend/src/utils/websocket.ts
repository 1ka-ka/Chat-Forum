import { getToken } from './auth'
import type { WsMessage } from '@/types'

type MessageHandler = (msg: WsMessage) => void

class WebSocketManager {
  private ws: WebSocket | null = null
  private url: string = ''
  private handlers: Map<string, MessageHandler[]> = new Map()
  private reconnectAttempts = 0
  private maxReconnectAttempts = 5
  private reconnectTimer: ReturnType<typeof setTimeout> | null = null
  private heartbeatTimer: ReturnType<typeof setInterval> | null = null
  private _isConnected = false

  get isConnected(): boolean {
    return this._isConnected
  }

  connect(): void {
    if (this.ws && this.ws.readyState === WebSocket.OPEN) return

    const token = getToken()
    if (!token) return

    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:'
    this.url = `${protocol}//${window.location.hostname}:8080/ws/chat?token=${token}`

    this.ws = new WebSocket(this.url)

    this.ws.onopen = () => {
      this._isConnected = true
      this.reconnectAttempts = 0
      this.startHeartbeat()
    }

    this.ws.onmessage = (event) => {
      try {
        const msg: WsMessage = JSON.parse(event.data)
        this.dispatch(msg)
      } catch (e) {
        console.error('WebSocket message parse error:', e)
      }
    }

    this.ws.onclose = () => {
      this._isConnected = false
      this.stopHeartbeat()
      this.tryReconnect()
    }

    this.ws.onerror = () => {
      this._isConnected = false
    }
  }

  disconnect(): void {
    this.stopHeartbeat()
    this.stopReconnect()
    if (this.ws) {
      this.ws.close()
      this.ws = null
    }
    this._isConnected = false
  }

  send(msg: WsMessage): void {
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(JSON.stringify(msg))
    }
  }

  on(type: string, handler: MessageHandler): void {
    if (!this.handlers.has(type)) {
      this.handlers.set(type, [])
    }
    this.handlers.get(type)!.push(handler)
  }

  off(type: string, handler: MessageHandler): void {
    const handlers = this.handlers.get(type)
    if (handlers) {
      const index = handlers.indexOf(handler)
      if (index > -1) handlers.splice(index, 1)
    }
  }

  private dispatch(msg: WsMessage): void {
    const handlers = this.handlers.get(msg.type)
    if (handlers) {
      handlers.forEach(h => h(msg))
    }
    const allHandlers = this.handlers.get('*')
    if (allHandlers) {
      allHandlers.forEach(h => h(msg))
    }
  }

  private startHeartbeat(): void {
    this.heartbeatTimer = setInterval(() => {
      this.send({ type: 'ping' })
    }, 30000)
  }

  private stopHeartbeat(): void {
    if (this.heartbeatTimer) {
      clearInterval(this.heartbeatTimer)
      this.heartbeatTimer = null
    }
  }

  private tryReconnect(): void {
    if (this.reconnectAttempts >= this.maxReconnectAttempts) return
    const delay = Math.pow(2, this.reconnectAttempts) * 1000
    this.reconnectTimer = setTimeout(() => {
      this.reconnectAttempts++
      this.connect()
    }, delay)
  }

  private stopReconnect(): void {
    if (this.reconnectTimer) {
      clearTimeout(this.reconnectTimer)
      this.reconnectTimer = null
    }
    this.reconnectAttempts = 0
  }
}

export const wsManager = new WebSocketManager()
