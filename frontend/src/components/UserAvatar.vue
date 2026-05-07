<script setup lang="ts">
import { ElAvatar } from 'element-plus'
import { computed } from 'vue'

const props = defineProps<{
  src?: string
  size?: number | string
  nickname?: string
}>()

const avatarSrc = computed(() => {
  if (props.src) {
    return props.src.startsWith('http') ? props.src : props.src
  }
  return '/default-avatar.png'
})

const initials = computed(() => {
  if (props.nickname) {
    return props.nickname.charAt(0).toUpperCase()
  }
  return 'U'
})
</script>

<template>
  <ElAvatar
    :size="size || 40"
    :src="avatarSrc"
  >
    {{ initials }}
  </ElAvatar>
</template>
