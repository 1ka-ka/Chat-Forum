<template>
  <div class="create-post-page">
    <el-card>
      <template #header>
        <h2>发布帖子</h2>
      </template>
      <el-form :model="form" :rules="rules" ref="formRef" label-width="80px">
        <el-form-item label="标题" prop="title">
          <el-input v-model="form.title" placeholder="请输入帖子标题" />
        </el-form-item>
        <el-form-item label="内容" prop="content">
          <el-input v-model="form.content" type="textarea" :rows="10" placeholder="请输入帖子内容" />
        </el-form-item>
        <el-form-item label="图片">
          <ImageUpload v-model="form.images" :max-count="9" />
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="handleSubmit" :loading="loading">发布</el-button>
          <el-button @click="$router.back()">取消</el-button>
        </el-form-item>
      </el-form>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive } from 'vue'
import { useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'
import type { FormInstance, FormRules, UploadFile } from 'element-plus'
import { createPost } from '@/api/post'
import ImageUpload from '@/components/ImageUpload.vue'

const router = useRouter()

const formRef = ref<FormInstance>()
const loading = ref(false)

const form = reactive({
  title: '',
  content: '',
  images: [] as string[]
})

const rules: FormRules = {
  title: [
    { required: true, message: '请输入标题', trigger: 'blur' }
  ],
  content: [
    { required: true, message: '请输入内容', trigger: 'blur' }
  ]
}

const handleSubmit = async () => {
  if (!formRef.value) return
  await formRef.value.validate(async (valid) => {
    if (valid) {
      loading.value = true
      try {
        const formData = new FormData()
        formData.append('title', form.title)
        formData.append('content', form.content)
        const res = await createPost(formData)
        ElMessage.success('发布成功')
        router.push(`/post/${res.data.id}`)
      } catch (e) {
      } finally {
        loading.value = false
      }
    }
  })
}
</script>

<style scoped>
.create-post-page {
  max-width: 800px;
  margin: 0 auto;
}

.create-post-page h2 {
  margin: 0;
  color: #303133;
}
</style>
