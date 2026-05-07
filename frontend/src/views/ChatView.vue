<script setup lang="ts">
import { ref, onMounted, onUnmounted, computed, watch } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { ElInput, ElButton, ElAvatar, ElEmpty, ElSpin } from 'element-plus'
import { useUserStore } from '@/stores/user'
import { useChatStore } from '@/stores/chat'
import { wsManager } from '@/utils/websocket'
import type { Message } from '@/types'
import ChatMessage from '@/components/ChatMessage.vue'

const route = useRoute()
const router = useRouter()
const userStore = useUserStore()
const chatStore = useChatStore()

const messageInput = ref('')
const messagesContainer = ref<HTMLElement | null>(null)

const currentUserId = computed(() => {
  const id = route.params.userId
  return id ? Number(id) : null
})

const currentConversation = computed(() => {
  if (!currentUserId.value) return null
  return chatStore.conversations.find((c) => c.user_id === currentUserId.value)
})

async function sendMessage() {
  if (!messageInput.value.trim() || !currentUserId.value) return

  wsManager.sendMessage(currentUserId.value, messageInput.value)
  messageInput.value = ''
}

function scrollToBottom() {
  if (messagesContainer.value) {
    messagesContainer.value.scrollTop = messagesContainer.value.scrollHeight
  }
}

function handleMessage(data: any) {
  if (data.type === 'message') {
    const message: Message = {
      id: data.message_id,
      sender_id: data.sender_id,
      receiver_id: userStore.user?.id || 0,
      content: data.content,
      is_read: 0,
      created_at: data.created_at
    }

    if (data.sender_id === currentUserId.value || data.receiver_id === currentUserId.value) {
      chatStore.addMessage(message)
      if (data.sender_id === currentUserId.value) {
        chatStore.markAsRead(currentUserId.value!)
      }
    }

    setTimeout(scrollToBottom, 100)
  } else if (data.type === 'online_status') {
    chatStore.handleOnlineStatus(data.user_id, data.online)
  }
}

async function selectConversation(userId: number) {
  router.push(`/chat/${userId}`)
}

async function loadChat() {
  if (currentUserId.value) {
    chatStore.setCurrentChat(currentUserId.value)
    await chatStore.fetchMessages(currentUserId.value)
    setTimeout(scrollToBottom, 100)
  }
}

watch(currentUserId, async (newId) => {
  if (newId) {
    await loadChat()
  }
})

onMounted(async () => {
  await chatStore.fetchConversations()
  wsManager.connect()
  wsManager.onMessage(handleMessage)

  if (currentUserId.value) {
    await loadChat()
  }
})

onUnmounted(() => {
  chatStore.setCurrentChat(null)
})
</script>

<template>
  <div class="chat-view">
    <div class="conversation-list">
      <h3>私信</h3>
      <div
        v-for="conv in chatStore.conversations"
        :key="conv.user_id"
        class="conversation-item"
        :class="{ active: conv.user_id === currentUserId }"
        @click="selectConversation(conv.user_id)"
      >
        <ElAvatar :size="44" :src="conv.avatar_url || '/default-avatar.png'" />
        <div class="conv-info">
          <span class="conv-name">{{ conv.nickname }}</span>
          <span class="conv-msg">{{ conv.last_message }}</span>
        </div>
        <span v-if="conv.unread_count > 0" class="unread-badge">{{ conv.unread_count }}</span>
      </div>
      <ElEmpty v-if="chatStore.conversations.length === 0" description="暂无会话" />
    </div>

    <div class="chat-area">
      <template v-if="currentUserId">
        <div class="chat-header">
          <span>{{ currentConversation?.nickname || '聊天' }}</span>
        </div>

        <div ref="messagesContainer" class="messages-container">
          <ChatMessage
            v-for="msg in chatStore.currentMessages"
            :key="msg.id"
            :message="msg"
          />
        </div>

        <div class="message-input">
          <ElInput
            v-model="messageInput"
            placeholder="输入消息..."
            @keyup.enter="sendMessage"
          />
          <ElButton type="primary" @click="sendMessage">发送</ElButton>
        </div>
      </template>
      <ElEmpty v-else description="选择一个会话开始聊天" />
    </div>
  </div>
</template>

<style scoped>
.chat-view {
  display: flex;
  height: calc(100vh - 100px);
  background: white;
  border-radius: 12px;
  overflow: hidden;
}

.conversation-list {
  width: 300px;
  border-right: 1px solid #f0f0f0;
  padding: 20px;
  overflow-y: auto;
}

.conversation-list h3 {
  margin-bottom: 16px;
  color: #333;
}

.conversation-item {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 12px;
  border-radius: 8px;
  cursor: pointer;
  transition: background 0.2s;
}

.conversation-item:hover,
.conversation-item.active {
  background: #f5f5f5;
}

.conv-info {
  flex: 1;
  overflow: hidden;
}

.conv-name {
  display: block;
  font-weight: 500;
  color: #333;
}

.conv-msg {
  display: block;
  font-size: 12px;
  color: #999;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.unread-badge {
  background: #f56c6c;
  color: white;
  font-size: 10px;
  padding: 2px 6px;
  border-radius: 10px;
}

.chat-area {
  flex: 1;
  display: flex;
  flex-direction: column;
}

.chat-header {
  padding: 16px 20px;
  border-bottom: 1px solid #f0f0f0;
  font-weight: 500;
}

.messages-container {
  flex: 1;
  padding: 20px;
  overflow-y: auto;
}

.message-input {
  display: flex;
  gap: 12px;
  padding: 16px 20px;
  border-top: 1px solid #f0f0f0;
}

.message-input .el-input {
  flex: 1;
}
</style>
