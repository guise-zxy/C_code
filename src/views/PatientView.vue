<script setup lang="ts">
import { onMounted, ref } from 'vue'
import { api } from '../api'
import { currentUserStrict } from '../auth'
import type { Prescription, PrescriptionInterpretation } from '../types'

// 当前登录患者（路由守卫已保证登录，这里取非空身份）
const me = currentUserStrict()

const prescriptions = ref<Prescription[]>([])
const error = ref('')

// AI 解读（按处方独立记录；错误显示在卡片内，不影响查看处方）
const interpretingId = ref<number | null>(null)
const interpretations = ref<Record<number, PrescriptionInterpretation>>({})
const interpErrors = ref<Record<number, string>>({})

async function loadPrescriptions() {
  try {
    // 后端只返回已发布处方：草稿对患者不可见
    prescriptions.value = await api.patientPrescriptions(me.id)
  } catch (e) {
    showError(e)
  }
}

onMounted(loadPrescriptions)

function showError(e: unknown) {
  error.value = e instanceof Error ? e.message : String(e)
}

function formatTime(s: string | null): string {
  if (!s) return '—'
  return new Date(s).toLocaleString('zh-CN', { hour12: false })
}

async function interpret(rxId: number) {
  interpErrors.value = { ...interpErrors.value, [rxId]: '' }
  try {
    interpretingId.value = rxId
    interpretations.value = {
      ...interpretations.value,
      [rxId]: await api.getInterpretation(rxId, me.id),
    }
  } catch (e) {
    interpErrors.value = {
      ...interpErrors.value,
      [rxId]: (e instanceof Error ? e.message : String(e)) + '；不影响查看处方，可稍后重试',
    }
  } finally {
    interpretingId.value = null
  }
}
</script>

<template>
  <div class="banner error" v-if="error">{{ error }}</div>

  <p class="empty" v-if="!error && prescriptions.length === 0">暂无已发布处方</p>

  <div class="card" v-for="rx in prescriptions" :key="rx.id">
    <div class="rx-head">
      <span class="rx-title">{{ rx.diagnosis }}</span>
      <span class="badge published">已发布</span>
    </div>
    <div class="rx-meta">
      开方医生：{{ rx.doctor_name }} · 发布于 {{ formatTime(rx.published_at) }}
    </div>
    <table>
      <thead>
        <tr>
          <th>药品</th><th>规格</th><th>用法用量</th><th>天数</th><th>数量</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="d in rx.drugs" :key="d.id">
          <td>{{ d.drug_name }}</td>
          <td>{{ d.spec ?? '—' }}</td>
          <td>{{ d.dosage }}</td>
          <td>{{ d.days ?? '—' }}</td>
          <td>{{ d.quantity ?? '—' }}</td>
        </tr>
      </tbody>
    </table>
    <div class="follow-up">📅 建议复诊日期：<b>{{ rx.follow_up_date }}</b></div>
    <p class="rx-meta" v-if="rx.notes" style="margin-top: 8px">医嘱：{{ rx.notes }}</p>

    <div v-if="!interpretations[rx.id]" style="margin-top: 10px">
      <div class="ai-error" v-if="interpErrors[rx.id]">{{ interpErrors[rx.id] }}</div>
      <button
        class="secondary small"
        type="button"
        @click="interpret(rx.id)"
        :disabled="interpretingId === rx.id"
      >
        {{ interpretingId === rx.id ? '解读中，约需数秒…' : 'AI 解读这份处方' }}
      </button>
    </div>
    <div class="ai-panel" v-else>
      <h3>AI 解读</h3>
      <b>怎么吃</b>
      <ul class="ai-list">
        <li v-for="(t, i) in interpretations[rx.id].how_to_take" :key="i">{{ t }}</li>
      </ul>
      <b>注意事项</b>
      <ul class="ai-list">
        <li v-for="(c, i) in interpretations[rx.id].cautions" :key="i">{{ c }}</li>
      </ul>
      <div class="ai-follow">📅 {{ interpretations[rx.id].follow_up_note }}</div>
      <div class="ai-note">
        AI 解读由 {{ interpretations[rx.id].model }}
        生成，仅供参考，具体请遵医嘱与药品说明书
      </div>
    </div>
  </div>
</template>
