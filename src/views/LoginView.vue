<script setup lang="ts">
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { api } from '../api'
import { auth, homePath } from '../auth'
import type { Role, User } from '../types'

const router = useRouter()

const role = ref<Role | ''>('')
const users = ref<User[]>([])
const userId = ref<number | null>(null)
const loading = ref(false)
const error = ref('')

function selectRole(r: Role) {
  if (role.value === r) return
  role.value = r
  userId.value = null
  users.value = []
  loadUsers()
}

async function loadUsers() {
  if (role.value === '') return
  error.value = ''
  loading.value = true
  try {
    users.value = await api.getUsers(role.value)
  } catch (e) {
    error.value = e instanceof Error ? e.message : String(e)
  } finally {
    loading.value = false
  }
}

function login() {
  const u = users.value.find((x) => x.id === userId.value)
  if (!u) return
  auth.login(u)
  router.push(homePath(u.role))
}
</script>

<template>
  <div class="login-wrap">
    <div class="login-card">
      <div class="login-brand">
        <svg width="40" height="40" viewBox="0 0 32 32" aria-hidden="true">
          <rect width="32" height="32" rx="6" fill="#14406e" />
          <path d="M13 6h6v7h7v6h-7v7h-6v-7H6v-6h7z" fill="#fff" />
        </svg>
        <span class="title">SZTU医院</span>
        <span class="subtitle">处方同步与复诊提醒系统</span>
      </div>

      <div class="banner error" v-if="error">
        {{ error }}
        <button class="secondary small" type="button" style="margin-left: 8px" @click="loadUsers">
          重试
        </button>
      </div>

      <div class="login-roles">
        <div :class="['role-card', { active: role === 'doctor' }]" @click="selectRole('doctor')">
          <span class="role-icon">👨‍⚕️</span>
          <span>医生工作站</span>
        </div>
        <div :class="['role-card', { active: role === 'patient' }]" @click="selectRole('patient')">
          <span class="role-icon">🧑</span>
          <span>患者门户</span>
        </div>
      </div>

      <template v-if="role !== ''">
        <p class="login-hint">{{ loading ? '正在加载人员名单…' : '请选择登录人员' }}</p>
        <div class="login-users" v-if="!loading && users.length">
          <button
            v-for="u in users"
            :key="u.id"
            :class="['user-chip', { active: userId === u.id }]"
            type="button"
            @click="userId = u.id"
          >
            {{ u.name }}
          </button>
        </div>
        <p class="login-hint" v-else-if="!loading">该身份下暂无人员</p>
      </template>

      <button class="login-btn" type="button" :disabled="userId === null" @click="login">
        登 录
      </button>
    </div>
  </div>
</template>
