import { createRouter, createWebHistory } from 'vue-router'
import { auth, homePath } from './auth'
import type { Role } from './types'
import DoctorView from './views/DoctorView.vue'
import LoginView from './views/LoginView.vue'
import PatientView from './views/PatientView.vue'

// 路由 meta 可声明所需身份，守卫据此拦截越界访问
declare module 'vue-router' {
  interface RouteMeta {
    role?: Role
  }
}

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: '/',
      redirect: () => {
        const u = auth.currentUser.value
        return u ? homePath(u.role) : '/login'
      },
    },
    { path: '/login', name: 'login', component: LoginView },
    { path: '/doctor', name: 'doctor', component: DoctorView, meta: { role: 'doctor' } },
    { path: '/patient', name: 'patient', component: PatientView, meta: { role: 'patient' } },
  ],
})

// 登录守卫：未登录一律去登录页；医生/患者只能进各自页面。
// 全部用返回值式守卫（不用 next），避免 next 双调用。
router.beforeEach((to) => {
  const u = auth.currentUser.value
  if (to.name === 'login') return u ? homePath(u.role) : true
  if (!u) return '/login'
  if (to.meta.role && to.meta.role !== u.role) return homePath(u.role)
  return true
})

export default router
