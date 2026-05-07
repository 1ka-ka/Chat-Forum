<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { ElButton, ElPagination, ElEmpty, ElSpin } from 'element-plus'
import { postApi } from '@/api/post'
import type { Post } from '@/types'
import PostCard from '@/components/PostCard.vue'

const router = useRouter()

const posts = ref<Post[]>([])
const loading = ref(false)
const currentPage = ref(1)
const pageSize = ref(20)
const total = ref(0)
const totalPages = ref(0)

async function fetchPosts(page: number = 1) {
  loading.value = true
  try {
    const res = await postApi.getPostList(page, pageSize.value)
    posts.value = res.data.items
    total.value = res.data.total
    totalPages.value = res.data.total_pages
    currentPage.value = page
  } catch {
    // error handled by interceptor
  } finally {
    loading.value = false
  }
}

function handlePageChange(page: number) {
  fetchPosts(page)
}

function goToCreate() {
  router.push('/create')
}

onMounted(() => {
  fetchPosts()
})
</script>

<template>
  <div class="home-view">
    <div class="home-header">
      <h1>最新帖子</h1>
      <ElButton type="primary" @click="goToCreate">发帖</ElButton>
    </div>

    <ElSpin :spinning="loading">
      <div v-if="posts.length > 0" class="post-list">
        <PostCard v-for="post in posts" :key="post.id" :post="post" />
      </div>
      <ElEmpty v-else-if="!loading" description="暂无帖子，来说点什么吧！" />
    </ElSpin>

    <div v-if="totalPages > 1" class="pagination">
      <ElPagination
        v-model:current-page="currentPage"
        :page-size="pageSize"
        :total="total"
        layout="prev, pager, next"
        @current-change="handlePageChange"
      />
    </div>
  </div>
</template>

<style scoped>
.home-view {
  max-width: 800px;
  margin: 0 auto;
}

.home-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.home-header h1 {
  font-size: 24px;
  color: #333;
}

.post-list {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.pagination {
  display: flex;
  justify-content: center;
  margin-top: 30px;
}
</style>
