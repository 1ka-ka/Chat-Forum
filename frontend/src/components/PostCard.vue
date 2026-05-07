<template>
  <el-card class="post-card">
    <div class="post-header">
      <UserAvatar :user="post" />
      <div class="user-info">
        <span class="nickname">{{ post.nickname }}</span>
        <span class="time">{{ post.createdAt }}</span>
      </div>
    </div>
    <div class="post-content" @click="goToDetail">
      <h3 class="post-title">{{ post.title }}</h3>
      <p class="post-text">{{ post.content }}</p>
      <div class="post-images" v-if="post.imageUrls && post.imageUrls.length > 0">
        <img v-for="(img, idx) in post.imageUrls.slice(0, 3)" :key="idx" :src="img" alt="">
      </div>
    </div>
    <div class="post-footer">
      <div class="footer-item" @click.stop="handleLike">
        <el-icon :color="post.isLiked ? '#f56c6c' : '#909399'">
          <component :is="post.isLiked ? 'StarFilled' : 'Star'" />
        </el-icon>
        <span>{{ post.likeCount }}</span>
      </div>
      <div class="footer-item" @click="goToDetail">
        <el-icon><ChatDotRound /></el-icon>
        <span>{{ post.commentCount }}</span>
      </div>
    </div>
  </el-card>
</template>

<script setup lang="ts">
import { useRouter } from 'vue-router'
import { useUserStore } from '@/stores/user'
import { ElMessage } from 'element-plus'
import { Star, StarFilled, ChatDotRound } from '@element-plus/icons-vue'
import UserAvatar from './UserAvatar.vue'
import { toggleLike as toggleLikeApi } from '@/api/like'
import type { Post } from '@/types'

interface Props {
  post: Post
}

const props = defineProps<Props>()
const router = useRouter()
const userStore = useUserStore()

const goToDetail = () => {
  router.push(`/post/${props.post.id}`)
}

const handleLike = async () => {
  if (!userStore.isLoggedIn) {
    ElMessage.warning('请先登录')
    router.push('/login')
    return
  }
  try {
    const res = await toggleLikeApi(props.post.id)
    if (res.data) {
      props.post.isLiked = res.data.isLiked
      props.post.likeCount = res.data.likeCount
    }
  } catch (e) {}
}
</script>

<style scoped>
.post-card {
  margin-bottom: 20px;
  cursor: pointer;
}

.post-header {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 16px;
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

.post-content {
  margin-bottom: 16px;
}

.post-title {
  font-size: 18px;
  margin-bottom: 8px;
  color: #303133;
}

.post-text {
  color: #606266;
  line-height: 1.6;
  display: -webkit-box;
  -webkit-line-clamp: 3;
  -webkit-box-orient: vertical;
  overflow: hidden;
}

.post-images {
  display: flex;
  gap: 12px;
  margin-top: 12px;
}

.post-images img {
  width: 150px;
  height: 150px;
  object-fit: cover;
  border-radius: 8px;
}

.post-footer {
  display: flex;
  gap: 24px;
  padding-top: 12px;
  border-top: 1px solid #ebeef5;
}

.footer-item {
  display: flex;
  align-items: center;
  gap: 6px;
  color: #909399;
  cursor: pointer;
}
</style>
