<template>
  <el-avatar :size="size" :src="user?.avatarUrl" v-if="user">
    {{ user.nickname?.charAt(0)?.toUpperCase() }}
  </el-avatar>
</template>

<script setup lang="ts">
import type { User } from '@/types'

interface Props {
  user?: User | null
  size?: 'large' | 'default' | 'small' | number
}

withDefaults(defineProps<Props>(), {
  size: 'default'
})
</script>
