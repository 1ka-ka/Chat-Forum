<template>
  <div class="home-page">
    <div class="page-header">
      <h2>最新帖子</h2>
    </div>
    <div class="post-list" v-loading="loading">
      <PostCard v-for="post in posts" :key="post.id" :post="post" />
      <el-empty v-if="!loading && posts.length === 0" description="暂无帖子" />
    </div>
    <div class="pagination" v-if="totalPages > 1">
      <el-pagination
        v-model:current-page="page"
        :page-size="pageSize"
        :total="total"
        @current-change="loadPosts"
        layout="prev, pager, next"
      />
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { getPostList } from '@/api/post'
import PostCard from '@/components/PostCard.vue'
import type { Post } from '@/types'

const posts = ref<Post[]>([])
const loading = ref(false)
const page = ref(1)
const pageSize = ref(20)
const total = ref(0)
const totalPages = ref(0)

const loadPosts = async () => {
  loading.value = true
  try {
    const res = await getPostList({ page: page.value, pageSize: pageSize.value })
    posts.value = res.data.items
    total.value = res.data.total
    totalPages.value = res.data.totalPages
  } catch (e) {
  } finally {
    loading.value = false
  }
}

onMounted(() => {
  loadPosts()
})
</script>

<style scoped>
.home-page {
  max-width: 800px;
  margin: 0 auto;
}

.page-header {
  margin-bottom: 24px;
}

.page-header h2 {
  color: #303133;
}

.post-list {
  min-height: 200px;
}

.pagination {
  margin-top: 24px;
  display: flex;
  justify-content: center;
}
</style>
