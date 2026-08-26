<script setup lang="ts">
import { onMounted, reactive, ref } from 'vue'
import { api } from '../api'
import { currentUserStrict } from '../auth'
import type { DrugSuggestion, Prescription, User } from '../types'

interface DrugRow {
  drug_name: string
  spec: string
  dosage: string
  days: string // 表单内统一用字符串，提交时再转数字
  quantity: string
}

// 当前登录医生（路由守卫已保证登录，这里取非空身份）
const me = currentUserStrict()

const patients = ref<User[]>([])
const patientId = ref<number | null>(null)

const prescriptions = ref<Prescription[]>([])
const error = ref('')
const message = ref('')
const saving = ref(false)
const publishingId = ref<number | null>(null)

// AI 药品建议（错误只显示在面板内部，不影响开方主流程）
const aiLoading = ref(false)
const aiSuggestions = ref<DrugSuggestion[]>([])
const aiModel = ref('')
const aiError = ref('')

// 本地日期串（不用 toISOString：UTC 会在晚间差一天）
function localDateStr(d: Date): string {
  return `${d.getFullYear()}-${String(d.getMonth() + 1).padStart(2, '0')}-${String(d.getDate()).padStart(2, '0')}`
}
const todayStr = () => localDateStr(new Date())
const in7Days = () => {
  const d = new Date()
  d.setDate(d.getDate() + 7)
  return localDateStr(d)
}

function emptyRow(): DrugRow {
  return { drug_name: '', spec: '', dosage: '', days: '', quantity: '' }
}

const form = reactive({
  diagnosis: '',
  followUpDate: in7Days(),
  notes: '',
  drugs: [emptyRow()] as DrugRow[],
})

function addRow() {
  form.drugs.push(emptyRow())
}

function removeRow(i: number) {
  form.drugs.splice(i, 1)
}

function showError(e: unknown) {
  error.value = e instanceof Error ? e.message : String(e)
}

async function loadPatients() {
  try {
    patients.value = await api.getUsers('patient')
    if (patientId.value === null && patients.value.length) {
      patientId.value = patients.value[0].id
    }
  } catch (e) {
    showError(e)
  }
}

async function loadPrescriptions() {
  try {
    prescriptions.value = await api.doctorPrescriptions(me.id)
  } catch (e) {
    showError(e)
  }
}

onMounted(async () => {
  await loadPatients()
  await loadPrescriptions()
})

function resetForm() {
  form.diagnosis = ''
  form.followUpDate = in7Days()
  form.notes = ''
  form.drugs = [emptyRow()]
}

/** AI 建议 → 药品行（null 转 ''，天数转字符串，与表单数据形态对齐） */
function suggestionToRow(s: DrugSuggestion) {
  return {
    drug_name: s.drug_name,
    spec: s.spec ?? '',
    dosage: s.dosage,
    days: s.days == null ? '' : String(s.days),
    quantity: s.quantity ?? '',
  }
}

async function fetchSuggestions() {
  aiError.value = ''
  if (patientId.value === null) {
    aiError.value = '请先选择患者'
    return
  }
  if (!form.diagnosis.trim()) {
    aiError.value = '请先填写诊断再获取 AI 建议'
    return
  }
  try {
    aiLoading.value = true
    const res = await api.getDrugSuggestions({
      doctor_id: me.id,
      patient_id: patientId.value,
      diagnosis: form.diagnosis.trim(),
    })
    aiSuggestions.value = res.suggestions
    aiModel.value = res.model
  } catch (e) {
    aiError.value =
      (e instanceof Error ? e.message : String(e)) + '；可继续手动录入药品，不影响开方'
  } finally {
    aiLoading.value = false
  }
}

function adoptSuggestion(i: number) {
  form.drugs.push(suggestionToRow(aiSuggestions.value[i]))
  aiSuggestions.value.splice(i, 1)
}

function adoptAllSuggestions() {
  for (const s of aiSuggestions.value) form.drugs.push(suggestionToRow(s))
  aiSuggestions.value = []
}

function clearSuggestions() {
  aiSuggestions.value = []
  aiError.value = ''
}

/** 提交前的本地校验：给出中文提示，避免把请求打到后端才报错 */
function validateForm(): string | null {
  if (patientId.value === null) return '请先选择患者'
  if (!form.diagnosis.trim()) return '请填写诊断'
  if (!form.followUpDate) return '请选择建议复诊日期'
  for (let i = 0; i < form.drugs.length; i++) {
    const d = form.drugs[i]
    const no = `第${i + 1}行药品`
    if (!d.drug_name.trim()) return `${no}未填写「药品名称」`
    if (!d.dosage.trim()) return `${no}未填写「用法用量」`
  }
  return null
}

async function saveDraft() {
  error.value = ''
  message.value = ''
  const invalid = validateForm()
  if (invalid) {
    error.value = invalid
    return
  }
  // validateForm 已保证非空，这里再拦一次是为了让 TS 收窄类型
  if (patientId.value === null) return
  try {
    saving.value = true
    await api.createPrescription({
      doctor_id: me.id,
      patient_id: patientId.value,
      diagnosis: form.diagnosis,
      follow_up_date: form.followUpDate,
      notes: form.notes || null,
      drugs: form.drugs.map((d) => ({
        drug_name: d.drug_name,
        spec: d.spec || null,
        dosage: d.dosage,
        days: d.days ? Number(d.days) : null,
        quantity: d.quantity || null,
      })),
    })
    message.value = '草稿已保存，可在下方列表发布'
    resetForm()
    await loadPrescriptions()
  } catch (e) {
    showError(e)
  } finally {
    saving.value = false
  }
}

async function publish(id: number) {
  error.value = ''
  message.value = ''
  try {
    publishingId.value = id
    await api.publishPrescription(id)
    message.value = `处方 ${id} 已发布，患者端即可查看`
    await loadPrescriptions()
  } catch (e) {
    showError(e)
  } finally {
    publishingId.value = null
  }
}

function formatTime(s: string | null): string {
  if (!s) return '—'
  return new Date(s).toLocaleString('zh-CN', { hour12: false })
}
</script>

<template>
  <div class="banner error" v-if="error">{{ error }}</div>
  <div class="banner success" v-if="message">{{ message }}</div>

  <div class="card">
    <h2>开具处方</h2>
    <div class="row" style="margin-bottom: 12px">
      <div class="field" style="margin: 0">
        <label>患者</label>
        <select v-model.number="patientId">
          <option v-for="p in patients" :key="p.id" :value="p.id">{{ p.name }}</option>
        </select>
      </div>
    </div>

    <div class="field">
      <label>诊断</label>
      <input v-model="form.diagnosis" maxlength="200" placeholder="如：急性上呼吸道感染" />
      <div style="margin-top: 8px">
        <button
          class="secondary small"
          type="button"
          @click="fetchSuggestions"
          :disabled="aiLoading"
        >
          {{ aiLoading ? 'AI 生成中，约需数秒…' : 'AI 推荐药品' }}
        </button>
      </div>
      <div class="ai-panel" v-if="aiError || aiSuggestions.length" style="margin-top: 10px">
        <h3>AI 药品建议</h3>
        <div class="ai-error" v-if="aiError">{{ aiError }}</div>
        <template v-if="aiSuggestions.length">
          <div class="ai-actions">
            <button class="secondary small" type="button" @click="adoptAllSuggestions">
              全部采纳
            </button>
            <button class="secondary small" type="button" @click="clearSuggestions">
              清空建议
            </button>
          </div>
          <div class="ai-suggestion" v-for="(s, i) in aiSuggestions" :key="i">
            <span class="name">{{ s.drug_name }}</span>
            <span>{{ s.spec ?? '—' }}</span>
            <span>{{ s.dosage }}</span>
            <span>{{ s.days != null ? s.days + ' 天' : '—' }}</span>
            <span>{{ s.quantity ?? '—' }}</span>
            <button class="small" type="button" @click="adoptSuggestion(i)">采纳</button>
            <span class="ai-reason">{{ s.reason }}</span>
          </div>
          <div class="ai-note">
            AI 建议由 {{ aiModel }} 生成，仅供参考，最终处方以医生判断为准
          </div>
        </template>
      </div>
    </div>

    <div class="field">
      <label>建议复诊日期</label>
      <input type="date" v-model="form.followUpDate" :min="todayStr()" />
    </div>

    <div class="field">
      <label>药品明细（至少一条）</label>
      <div class="drug-row" v-for="(d, i) in form.drugs" :key="i">
        <input v-model="d.drug_name" maxlength="100" placeholder="药品名称（必填）" />
        <input v-model="d.spec" maxlength="100" placeholder="规格（可选）" />
        <input v-model="d.dosage" maxlength="200" placeholder="用法用量（必填），如：每次1片，每日3次" />
        <input v-model="d.days" type="number" min="1" max="365" placeholder="天数" />
        <input v-model="d.quantity" maxlength="20" placeholder="数量，如：1盒" />
        <button
          class="danger"
          type="button"
          @click="removeRow(i)"
          :disabled="form.drugs.length === 1"
          title="删除此行"
        >
          ✕
        </button>
      </div>
      <div>
        <button class="secondary" type="button" @click="addRow">+ 添加药品</button>
      </div>
    </div>

    <div class="field">
      <label>医嘱备注（可选）</label>
      <textarea v-model="form.notes" maxlength="500" rows="2" placeholder="如：多喝水，注意休息"></textarea>
    </div>

    <button @click="saveDraft" :disabled="saving">
      {{ saving ? '保存中…' : '保存草稿' }}
    </button>
  </div>

  <div class="card">
    <h2>我的处方</h2>
    <p class="empty" v-if="prescriptions.length === 0">暂无处方</p>
    <div v-for="rx in prescriptions" :key="rx.id" style="border-top: 1px solid var(--c-divider); padding: 12px 0">
      <div class="rx-head">
        <span class="rx-title">{{ rx.id }} {{ rx.diagnosis }} — {{ rx.patient_name }}</span>
        <span :class="['badge', rx.status === 'DRAFT' ? 'draft' : 'published']">
          {{ rx.status === 'DRAFT' ? '草稿' : '已发布' }}
        </span>
      </div>
      <div class="rx-meta">
        复诊日期 {{ rx.follow_up_date }} · 创建于 {{ formatTime(rx.created_at) }}
        <template v-if="rx.published_at"> · 发布于 {{ formatTime(rx.published_at) }}</template>
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
      <button v-if="rx.status === 'DRAFT'" @click="publish(rx.id)" :disabled="publishingId === rx.id">
        {{ publishingId === rx.id ? '发布中…' : '发布' }}
      </button>
    </div>
  </div>
</template>
