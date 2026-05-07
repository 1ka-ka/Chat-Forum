<script setup lang="ts">
import { ref, onMounted, computed } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { ElButton, ElInput, ElAvatar, ElEmpty, ElSpin, ElMessage } from 'element-plus'
import { postApi } from '@/api/post'
import { commentApi } from '@/api/comment'
import { likeApi } from '@/api/like'
import { userApi } from '@/api/user'
import type { Post, Comment, User } from '@/types'
import LikeButton from '@/components/LikeButton.vue'
import CommentItem from '@/components/CommentItem.vue'

const route = useRoute()
const router = useRouter()

const post = ref<Post | null>(null)
const comments = ref<Comment[]>([])
const postAuthor = ref<User | null>(null)
const loading = ref(false)
const commentLoading = ref(false)
const newComment = ref('')
const commentPage = ref(1)
const commentTotalPages = ref(1)
const likeCount = ref(0)
const isLiked = ref(false)

async function fetchPost() {
  loading.value = true
  try {
    const postId = Number(route.params.id)
    const res = await postApi.getPostById(postId)
    post.value = res.data
    likeCount.value = res.data.like_count
    isLiked.value = res.data.is_liked

    const userRes = await userApi.getUserById(res.data.user_id)
    postAuthor.value = userRes.data
  } catch {
    ElMessage.error('帖子不存在')
    router.push('/')
  } finally {
    loading.value = false
  }
}

async function fetchComments(page: number = 1) {
  commentLoading.value = true
  try {
    const postId = Number(route.params.id)
    const res = await commentApi.getComments(postId, page)
    comments.value = res.data.items
    commentTotalPages.value = res.data.total_pages
    commentPage.value = page
  } catch {
    // error handled by interceptor
  } finally {
    commentLoading.value = false
  }
}

async function submitComment() {
  if (!newComment.value.trim()) {
    ElMessage.warning('请输入评论内容')
    return
  }

  try {
    const postId = Number(route.params.id)
    await commentApi.createComment(postId, newComment.value)
    ElMessage.success('评论成功')
    newComment.value = ''
    fetchComments(1)
    if (post.value) {
      post.value.comment_count++
    }
  } catch {
    // error handled by interceptor
  }
}

function goToUser(userId: number) {
  router.push(`/user/${userId}`)
}

function goBack() {
  router.back()
}

onMounted(() => {
  fetchPost()
  fetchComments()
})
</script>

<template>
  <div class="post-detail-view">
    <ElButton text @click="goBack">← 返回</ElButton>

    <ElSpin :spinning="loading">
      <div v-if="post" class="post-content">
        <h1 class="post-title">{{ post.title }}</h1>

        <div class="post-author" @click="goToUser(post.user_id)">
          <ElAvatar :size="48" :src="post.avatar_url || '/default-avatar.png'" />
          <div class="author-info">
            <span class="nickname">{{ post.nickname }}</span>
            <span class="post-time">{{ post.created_at }}</span>
          </div>
        </div>

        <div class="post-body">
          <p>{{ post.content }}</p>
          <div v-if="post.image_urls && post.image_urls.length > 0" class="post-images">
            <img
              v-for="(url, index) in post.image_urls"
              :key="index"
              :src="url"
              alt="post image"
            />
          </div>
        </div>

        <div class="post-actions">
          <LikeButton
            :post-id="post.id"
            :initial-liked="isLiked"
            :initial-count="likeCount"
            @update="likeCount = $event"
          />
          <span class="comment-count">💬 {{ post.comment_count }}</span>
        </div>

        <div class="comment-section">
          <h3>评论 ({{ post.comment_count }})</h3>

          <div class="comment-form">
            <ElInput
              v-model="newComment"
              type="textarea"
              :rows="3"
              placeholder="说点什么..."
            />
            <ElButton type="primary" @click="submitComment" :loading="commentLoading">
              发布评论
            </ElButton>
          </div>

          <div class="comment-list">
            <CommentItem
              v-for="comment in comments"
              :key="comment.id"
              :comment="comment"
            />
            <ElEmpty v-if="comments.length === 0 && !commentLoading" description="暂无评论" />
          </div>
        </div>
      </div>
    </ElSpin>
  </div>
</template>

<style scoped>
.post-detail-view {
  max-width: 800px;
  margin: 0 auto;
}

.post-content {
  background: white;
  border-radius: 12px;
  padding: 30px;
  margin-top: 20px;
}

.post-title {
  font-size: 28px;
  color: #333;
  margin-bottom: 20px;
}

.post-author {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 20px;
  cursor: pointer;
}

.author-info {
  display: flex;
  flex-direction: column;
}

.nickname {
  font-weight: 500;
  color: #333;
}

.post-time {
  font-size: 12px;
  color: #999;
}

.post-body {
  line-height: 1.8;
  color: #333;
  margin-bottom: 20px;
}

.post-body p {
  white-space: pre-wrap;
}

.post-images {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
  margin-top: 16px;
}

.post-images img {
  max-width: 300px;
  border-radius: 8px;
}

.post-actions {
  display: flex;
  align-items: center;
  gap: 20px;
  padding: 16px 0;
  border-top: 1px solid #f0f0f0;
  border-bottom: 1px solid #f0f0f0;
}

.comment-count {
  color: #666;
}

.comment-section {
  margin-top: 30px;
}

.comment-section h3 {
  margin-bottom: 16px;
  color: #333;
}

.comment-form {
  display: flex;
  gap: 12px;
  margin-bottom: 20px;
}

.comment-form .el-input {
  flex: 1;
}

.comment-list {
  background: #fafafa;
  border-radius: 8px;
  padding: 16px;
}
</style>
