<template>
  <div class="chat-page">
    <el-container>
      <el-aside width="300px">
        <div class="conversation-list">
          <div class="list-header">会话列表</div>
          <div class="list-content">
            <div
              v-for="conv in conversations"
              :key="conv.userId"
              class="conversation-item"
              :class="{ active: currentUserId === conv.userId }"
              @click="selectConversation(conv)"
            >
              <UserAvatar :user="{ id: conv.userId, nickname: conv.nickname, avatarUrl: conv.avatarUrl }" />
              <div class="conv-info">
                <div class="conv-top">
                  <span class="nickname">{{ conv.nickname }}</span>
                  <span class="time">{{ formatTime(conv.lastTime) }}</span>
                </div>
                <div class="conv-bottom">
                  <span class="text">{{ conv.lastMessage }}</span>
                  <el-badge :value="conv.unreadCount" class="unread-badge" v-if="conv.unreadCount > 0" />
                </div>
              </div>
            </div>
            <el-empty v-if="conversations.length === 0" description="暂无会话" />
          </div>
        </div>
      </el-aside>
      <el-main>
        <div class="chat-window" v-if="currentUserId">
          <div class="chat-header">
            <UserAvatar :user="{ id: currentUserId, nickname: currentNickname, avatarUrl: currentAvatarUrl }" />
            <span class="nickname">{{ currentNickname }}</span>
          </div>
          <div class="chat-messages" ref="messagesRef">
            <div v-for="(msg, idx) in currentMessages" :key="idx" class="message-item" :class="{ 'is-me': msg.senderId === userStore.userId }">
              <UserAvatar v-if="msg.senderId !== userStore.userId" :user="{ id: msg.senderId, nickname: '', avatarUrl: '' }" />
              <div class="message-bubble">
                {{ msg.content }}
                <span class="message-time">{{ formatTime(msg.createdAt) }}</span>
              </div>
              <UserAvatar v-if="msg.senderId === userStore.userId" :user="userStore.userInfo" />
            </div>
          </div>
          <div class="chat-input">
            <el-input
              v-model="message"
              type="textarea"
              :rows="3"
              placeholder="输入消息..."
              @keyup.ctrl.enter="sendMessage"
            />
            <el-button type="primary" @click="sendMessage" :loading="sending">发送</el-button>
          </div>
        </div>
        <el-empty v-else description="请选择会话" class="empty-state" />
      </el-main>
    </el-container>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted, nextTick, watch } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { useUserStore } from '@/stores/user'
import { useChatStore } from '@/stores/chat'
import { getConversations, getMessages, markAsRead } from '@/api/chat'
import UserAvatar from '@/components/UserAvatar.vue'
import type { Conversation, Message } from '@/types'

const route = useRoute()
const router = useRouter()
const userStore = useUserStore()
const chatStore = useChatStore()

const conversations = ref<Conversation[]>([])
const currentUserId = ref<number | null>(null)
const currentMessages = ref<Message[]>([])
const message = ref('')
const sending = ref(false)
const messagesRef = ref<HTMLElement>()

const currentNickname = computed(() => {
  const conv = conversations.value.find(c => c.userId === currentUserId.value)
  return conv?.nickname || ''
})

const currentAvatarUrl = computed(() => {
  const conv = conversations.value.find(c => c.userId === currentUserId.value)
  return conv?.avatarUrl || ''
})

const loadConversations = async () => {
  try {
    const res = await getConversations()
    conversations.value = res.data
    chatStore.setConversations(res.data)
  } catch (e) {}
}

const loadMessages = async (userId: number) => {
  try {
    const res = await getMessages(userId, {})
    currentMessages.value = res.data.items.reverse()
    chatStore.setMessages(userId, currentMessages.value)
    await markAsRead(userId)
  } catch (e) {}
}

const selectConversation = (conv: Conversation) => {
  currentUserId.value = conv.userId
  router.push(`/chat/${conv.userId}`)
  loadMessages(conv.userId)
}

const sendMessage = () => {
  if (!message.value.trim() || !currentUserId.value) return
  chatStore.sendMessage(currentUserId.value, message.value)
  const newMsg: Message = {
    id: Date.now(),
    senderId: userStore.userId!,
    receiverId: currentUserId.value,
    content: message.value,
    isRead: 1,
    createdAt: new Date().toISOString()
  }
  currentMessages.value.push(newMsg)
  message.value = ''
  scrollToBottom()
}

const formatTime = (time: string) => {
  const date = new Date(time)
  const now = new Date()
  const diff = now.getTime() - date.getTime()
  if (diff < 60000) return '刚刚'
  if (diff < 3600000) return `${Math.floor(diff / 60000)}分钟前`
  if (diff < 86400000) return `${Math.floor(diff / 3600000)}小时前`
  return date.toLocaleDateString() + ' ' + date.toLocaleTimeString().slice(0, 5)
}

const scrollToBottom = () => {
  nextTick(() => {
    if (messagesRef.value) {
      messagesRef.value.scrollTop = messagesRef.value.scrollHeight
    }
  })
}

onMounted(() => {
  loadConversations()
  if (route.params.userId) {
    const userId = parseInt(route.params.userId as string)
    currentUserId.value = userId
    loadMessages(userId)
  }
})

watch(() => route.params.userId, (newVal) => {
  if (newVal) {
    const userId = parseInt(newVal as string)
    currentUserId.value = userId
    loadMessages(userId)
  }
})
</script>

<style scoped>
.chat-page {
  height: calc(100vh - 104px);
}

.el-container {
  height: 100%;
  background: #fff;
  border-radius: 8px;
  overflow: hidden;
}

.el-aside {
  border-right: 1px solid #ebeef5;
  height: 100%;
}

.el-main {
  padding: 0;
  display: flex;
  flex-direction: column;
  height: 100%;
}

.conversation-list {
  height: 100%;
  display: flex;
  flex-direction: column;
}

.list-header {
  padding: 16px;
  font-weight: 600;
  border-bottom: 1px solid #ebeef5;
  color: #303133;
}

.list-content {
  flex: 1;
  overflow-y: auto;
}

.conversation-item {
  padding: 12px 16px;
  display: flex;
  gap: 12px;
  cursor: pointer;
  transition: background 0.2s;
}

.conversation-item:hover {
  background: #f5f7fa;
}

.conversation-item.active {
  background: #ecf5ff;
}

.conv-info {
  flex: 1;
  display: flex;
  flex-direction: column;
  justify-content: center;
  gap: 4px;
  min-width: 0;
}

.conv-top {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.conv-top .nickname {
  font-weight: 500;
  color: #303133;
}

.conv-top .time {
  font-size: 12px;
  color: #909399;
}

.conv-bottom {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.conv-bottom .text {
  font-size: 13px;
  color: #909399;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.chat-window {
  height: 100%;
  display: flex;
  flex-direction: column;
}

.chat-header {
  padding: 16px;
  display: flex;
  align-items: center;
  gap: 12px;
  border-bottom: 1px solid #ebeef5;
}

.chat-header .nickname {
  font-weight: 500;
  color: #303133;
}

.chat-messages {
  flex: 1;
  padding: 16px;
  overflow-y: auto;
}

.message-item {
  display: flex;
  gap: 12px;
  margin-bottom: 16px;
}

.message-item.is-me {
  flex-direction: row-reverse;
}

.message-bubble {
  max-width: 60%;
  padding: 12px 16px;
  border-radius: 12px;
  background: #f5f7fa;
  color: #303133;
  position: relative;
}

.message-item.is-me .message-bubble {
  background: #409eff;
  color: white;
}

.message-time {
  display: block;
  font-size: 11px;
  margin-top: 8px;
  opacity: 0.7;
}

.chat-input {
  padding: 16px;
  border-top: 1px solid #ebeef5;
  display: flex;
  gap: 12px;
}

.chat-input .el-input {
  flex: 1;
}

.empty-state {
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
}
</style>
