<template>
  <div class="upload-container">
    <div class="image-list" v-if="fileList.length > 0">
      <div v-for="(file, idx) in fileList" :key="idx" class="image-item">
        <img :src="file" alt="">
        <div class="image-cover" @click="removeImage(idx)">
          <el-icon><Close /></el-icon>
        </div>
      </div>
    </div>
    <el-upload
      v-show="fileList.length < maxCount"
      :show-file-list="false"
      :on-change="handleChange"
      :before-upload="beforeUpload"
      :auto-upload="false"
      accept="image/*"
      multiple
    >
      <div class="upload-btn">
        <el-icon><Plus /></el-icon>
      </div>
    </el-upload>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { ElMessage } from 'element-plus'
import { Plus, Close } from '@element-plus/icons-vue'

interface Props {
  maxCount?: number
  maxSize?: number
  modelValue?: string[]
}

interface Emits {
  (e: 'update:modelValue', value: string[]): void
}

const props = withDefaults(defineProps<Props>(), {
  maxCount: 9,
  maxSize: 5
})

const emit = defineEmits<Emits>()
const fileList = ref<string[]>(props.modelValue || [])

const beforeUpload = (file: File) => {
  const isImg = file.type.startsWith('image/')
  if (!isImg) {
    ElMessage.error('只能上传图片文件')
    return false
  }
  const isLt = file.size / 1024 / 1024 < props.maxSize
  if (!isLt) {
    ElMessage.error(`图片大小不能超过 ${props.maxSize}MB`)
    return false
  }
  return true
}

const handleChange = (file: any) => {
  if (file.raw && beforeUpload(file.raw)) {
    const reader = new FileReader()
    reader.onload = (e) => {
      if (e.target?.result) {
        fileList.value.push(e.target.result as string)
        emit('update:modelValue', [...fileList.value])
      }
    }
    reader.readAsDataURL(file.raw)
  }
}

const removeImage = (idx: number) => {
  fileList.value.splice(idx, 1)
  emit('update:modelValue', [...fileList.value])
}
</script>

<style scoped>
.upload-container {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
}

.image-list {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
}

.image-item {
  position: relative;
  width: 100px;
  height: 100px;
}

.image-item img {
  width: 100%;
  height: 100%;
  object-fit: cover;
  border-radius: 8px;
}

.image-cover {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: rgba(0, 0, 0, 0.5);
  display: flex;
  align-items: center;
  justify-content: center;
  opacity: 0;
  transition: opacity 0.3s;
  border-radius: 8px;
  cursor: pointer;
}

.image-item:hover .image-cover {
  opacity: 1;
}

.image-cover .el-icon {
  color: white;
  font-size: 24px;
}

.upload-btn {
  width: 100px;
  height: 100px;
  border: 2px dashed #d9d9d9;
  border-radius: 8px;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: border-color 0.3s;
}

.upload-btn:hover {
  border-color: #409eff;
}

.upload-btn .el-icon {
  font-size: 32px;
  color: #8c939d;
}
</style>
