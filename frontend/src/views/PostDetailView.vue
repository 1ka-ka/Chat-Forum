<template>
  <div class="post-detail-page" v-loading="loading">
    <el-card class="post-card" v-if="post">
      <div class="post-header">
        <UserAvatar :user="post" />
        <div class="user-info">
          <span class="nickname">{{ post.nickname }}</span>
          <span class="time">{{ post.createdAt }}</span>
        </div>
      </div>
      <h1 class="post-title">{{ post.title }}</h1>
      <div class="post-content">{{ post.content }}</div>
      <div class="post-images" v-if="post.imageUrls && post.imageUrls.length > 0">
        <el-image
          v-for="(img, idx) in post.imageUrls"
          :key="idx"
          :src="img"
          :preview-src-list="post.imageUrls"
          :initial-index="idx"
          fit="cover"
        />
      </div>
      <div class="post-footer">
        <div class="footer-item" @click="handleLike">
          <el-icon :color="post.isLiked ? '#f56c6c' : '#909399'">
            <component :is="post.isLiked ? 'StarFilled' : 'Star'" />
          </el-icon>
          <span>{{ post.likeCount }}</span>
        </div>
        <div class="footer-item">
          <el-icon><ChatDotRound /></el-icon>
          <span>{{ post.commentCount }}</span>
        </div>
      </div>
    </el-card>

    <el-card class="comment-card">
      <template #header>
        <h3>评论 ({{ comments.length }})</h3>
      </template>
      <template v-if="userStore.isLoggedIn">
        <div class="comment-input">
          <UserAvatar :user="userStore.userInfo" />
          <el-input
            v-model="newComment"
            type="textarea"
            :rows="3"
            placeholder="发表评论..."
            @keyup.ctrl.enter="submitComment"
          />
          <el-button type="primary" @click="submitComment" :loading="submitting">发送</el-button>
        </div>
      </template>
      <div class="comment-list">
        <CommentItem v-for="comment in comments" :key="comment.id" :comment="comment" />
        <el-empty v-if="comments.length === 0" description="暂无评论" />
      </div>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'
import { Star, StarFilled, ChatDotRound } from '@element-plus/icons-vue'
import { useUserStore } from '@/stores/user'
import { getPostById } from '@/api/post'
import { getComments, createComment } from '@/api/comment'
import { toggleLike as toggleLikeApi } from '@/api/like'
import PostCard from '@/components/PostCard.vue'
import CommentItem from '@/components/CommentItem.vue'
import UserAvatar from '@/components/UserAvatar.vue'
import type { Post, Comment } from '@/types'

const route = useRoute()
const router = useRouter()
const userStore = useUserStore()

const post = ref<Post | null>(null)
const comments = ref<Comment[]>([])
const loading = ref(false)
const submitting = ref(false)
const newComment = ref('')

const loadPost = async () => {
  loading.value = true
  try {
    const postId = parseInt(route.params.id as string)
    const [postRes, commentsRes] = await Promise.all([
      getPostById(postId),
      getComments(postId, {})
    ])
    post.value = postRes.data
    comments.value = commentsRes.data.items
  } catch (e) {
  } finally {
    loading.value = false
  }
}

const handleLike = async () => {
  if (!userStore.isLoggedIn) {
    ElMessage.warning('请先登录')
    router.push('/login')
    return
  }
  if (!post.value) return
  try {
    const res = await toggleLikeApi(post.value.id)
    if (res.data) {
      post.value.isLiked = res.data.isLiked
      post.value.likeCount = res.data.likeCount
    }
  } catch (e) {}
}

const submitComment = async () => {
  if (!userStore.isLoggedIn) {
    ElMessage.warning('请先登录')
    router.push('/login')
    return
  }
  if (!newComment.value.trim()) {
    ElMessage.warning('请输入评论内容')
    return
  }
  submitting.value = true
  try {
    const res = await createComment(post.value!.id, { content: newComment.value })
    comments.value.unshift(res.data)
    newComment.value = ''
    ElMessage.success('评论成功')
    if (post.value) {
      post.value.commentCount++
    }
  } catch (e) {
  } finally {
    submitting.value = false
  }
}

onMounted(() => {
  loadPost()
})
</script>

<style scoped>
.post-detail-page {
  max-width: 800px;
  margin: 0 auto;
}

.post-card {
  margin-bottom: 20px;
}

.post-header {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 16px;
  padding-bottom: 16px;
  border-bottom: 1px solid #ebeef5;
}

.user-info {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.nickname {
  font-weight: 500;
  color: #303133;
}

.time {
  font-size: 12px;
  color: #909399;
}

.post-title {
  font-size: 24px;
  margin-bottom: 16px;
  color: #303133;
}

.post-content {
  color: #606266;
  line-height: 1.8;
  margin-bottom: 16px;
  white-space: pre-wrap;
}

.post-images {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
  margin-bottom: 16px;
}

.post-images :deep(.el-image) {
  width: 200px;
  height: 200px;
  border-radius: 8px;
}

.post-footer {
  display: flex;
  gap: 24px;
  padding-top: 16px;
  border-top: 1px solid #ebeef5;
}

.footer-item {
  display: flex;
  align-items: center;
  gap: 6px;
  color: #909399;
  cursor: pointer;
}

.comment-card {
  margin-bottom: 20px;
}

.comment-card h3 {
  margin: 0;
  color: #303133;
}

.comment-input {
  display: flex;
  gap: 12px;
  margin-bottom: 24px;
  padding-bottom: 24px;
  border-bottom: 1px solid #f0f0f0;
}

.comment-input .el-input {
  flex: 1;
}

.comment-list {
  min-height: 100px;
}
</style>
