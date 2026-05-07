<template>
  <div class="comment-item">
    <UserAvatar :user="{ id: comment.userId, nickname: comment.nickname, avatarUrl: comment.avatarUrl }" />
    <div class="comment-body">
      <div class="comment-meta">
        <span class="nickname">{{ comment.nickname }}</span>
        <span class="time">{{ comment.createdAt }}</span>
      </div>
      <div class="comment-content">{{ comment.content }}</div>
    </div>
  </div>
</template>

<script setup lang="ts">
import UserAvatar from './UserAvatar.vue'
import type { Comment } from '@/types'

interface Props {
  comment: Comment
}

defineProps<Props>()
</script>

<style scoped>
.comment-item {
  display: flex;
  gap: 12px;
  padding: 16px 0;
  border-bottom: 1px solid #f0f0f0;
}

.comment-body {
  flex: 1;
}

.comment-meta {
  margin-bottom: 8px;
}

.nickname {
  font-weight: 500;
  color: #303133;
  margin-right: 12px;
}

.time {
  font-size: 12px;
  color: #909399;
}

.comment-content {
  color: #606266;
  line-height: 1.6;
}
</style>
