/** 前端伪登录的登录态单例：sessionStorage 持久化（刷新保持、关标签页自动登出）。
 *  仅用于路由守卫与界面展示；接口仍按传入 id 过滤，无真实鉴权。 */
import { computed, ref } from 'vue'
import type { Role, User } from './types'

const STORAGE_KEY = 'zxy.auth.user'

/** 从 sessionStorage 恢复登录态；数据缺失或格式异常时静默当作未登录 */
function loadFromStorage(): User | null {
  try {
    const raw = sessionStorage.getItem(STORAGE_KEY)
    if (!raw) return null
    const u = JSON.parse(raw) as User
    if (
      typeof u.id === 'number' &&
      typeof u.name === 'string' &&
      (u.role === 'doctor' || u.role === 'patient')
    ) {
      return u
    }
    return null
  } catch {
    return null
  }
}

// 模块顶层同步恢复：router.ts 一旦 import 本模块即完成，早于任何守卫执行
const currentUser = ref<User | null>(loadFromStorage())

export const auth = {
  currentUser,
  isLoggedIn: computed(() => currentUser.value !== null),
  role: computed<Role | null>(() => currentUser.value?.role ?? null),
  login(u: User): void {
    currentUser.value = u
    sessionStorage.setItem(STORAGE_KEY, JSON.stringify(u))
  },
  logout(): void {
    currentUser.value = null
    sessionStorage.removeItem(STORAGE_KEY)
  },
}

/** 登录后应进入的页面 */
export function homePath(role: Role): '/doctor' | '/patient' {
  return role === 'doctor' ? '/doctor' : '/patient'
}

/** 视图内取非空身份：路由守卫已保证登录，这里让 TS 把类型收窄为 User */
export function currentUserStrict(): User {
  const u = currentUser.value
  if (u === null) throw new Error('登录状态异常，请重新登录')
  return u
}
