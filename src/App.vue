<script setup lang="ts">
import { RouterView, useRouter } from 'vue-router'
import { auth } from './auth'

const router = useRouter()
const user = auth.currentUser // 模板里自动解包；登录页（user 为空）不显示顶栏

function logout() {
  auth.logout()
  router.push('/login')
}
</script>

<template>
  <template v-if="user">
    <header class="topbar">
      <span class="brand">
        <svg width="22" height="22" viewBox="0 0 32 32" aria-hidden="true">
          <rect width="32" height="32" rx="6" fill="rgba(255,255,255,0.16)" />
          <path d="M13 6h6v7h7v6h-7v7h-6v-7H6v-6h7z" fill="#fff" />
        </svg>
        SZTU医院 · 处方同步与复诊提醒系统
      </span>
      <div class="user-area">
        <span class="user-name">
          {{ user.name }} · {{ user.role === 'doctor' ? '医生' : '患者' }}
        </span>
        <button class="logout" type="button" @click="logout">退出</button>
      </div>
    </header>
    <main class="page">
      <RouterView />
    </main>
  </template>
  <main v-else>
    <RouterView />
  </main>
</template>
