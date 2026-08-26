import type {
  DrugSuggestionResult,
  Prescription,
  PrescriptionCreate,
  PrescriptionInterpretation,
  User,
} from './types'

/** 422 校验错误的字段名 → 中文标签（loc 里出现什么就翻译什么） */
const FIELD_LABELS: Record<string, string> = {
  doctor_id: '医生',
  patient_id: '患者',
  diagnosis: '诊断',
  follow_up_date: '复诊日期',
  notes: '医嘱备注',
  drugs: '药品明细',
  drug_name: '药品名称',
  spec: '规格',
  dosage: '用法用量',
  days: '天数',
  quantity: '数量',
}

/** 常见 Pydantic 英文报错 → 中文 */
function translateMsg(msg: string): string {
  if (msg.startsWith('Value error, ')) return msg.slice('Value error, '.length) // 自定义校验器本就是中文
  if (msg === 'Field required') return '此项必填'
  if (msg === 'String should have at least 1 character') return '至少填写 1 个字符'
  if (msg.startsWith('Input should be a valid date')) return '日期格式不正确'
  if (msg.startsWith('Input should be a valid integer')) return '请填写整数'
  if (msg.startsWith('Input should be less than or equal to')) return `数值过大（${msg.slice(msg.lastIndexOf(' ') + 1)} 以内）`
  if (msg.startsWith('Input should be greater than or equal to')) return `数值过小（至少 ${msg.slice(msg.lastIndexOf(' ') + 1)}）`
  return msg
}

/** 把一条 422 错误（含 loc 字段路径）转成"位置：原因"的中文描述 */
function format422(d: unknown): string {
  if (typeof d !== 'object' || d === null || !('msg' in d)) return JSON.stringify(d)
  const item = d as { msg?: unknown; loc?: unknown }
  const msg = translateMsg(typeof item.msg === 'string' ? item.msg : JSON.stringify(item.msg))
  const loc = Array.isArray(item.loc) ? item.loc.filter((p) => p !== 'body') : []
  if (!loc.length) return msg
  const where = loc
    .map((p) => (typeof p === 'number' ? `第${p + 1}行` : (FIELD_LABELS[String(p)] ?? String(p))))
    .join(' ')
  return `${where}：${msg}`
}

/**
 * 后端统一返回 {detail: string}（404/409）或 {detail: [{msg, loc}, ...]}（422），
 * 这里统一转成中文 Error.message 供页面红条展示。
 */
async function request<T>(path: string, init?: RequestInit): Promise<T> {
  const res = await fetch('/api' + path, {
    headers: { 'Content-Type': 'application/json' },
    ...init,
  })
  if (!res.ok) {
    let detail: unknown = `HTTP ${res.status}`
    try {
      detail = (await res.json()).detail ?? detail
    } catch {
      /* 无响应体 */
    }
    if (Array.isArray(detail)) {
      throw new Error(detail.map(format422).join('；'))
    }
    throw new Error(typeof detail === 'string' ? detail : JSON.stringify(detail))
  }
  return (await res.json()) as T
}

export const api = {
  getUsers: (role: 'doctor' | 'patient') =>
    request<User[]>(`/users?role=${role}`),

  createPrescription: (body: PrescriptionCreate) =>
    request<Prescription>('/prescriptions', {
      method: 'POST',
      body: JSON.stringify(body),
    }),

  publishPrescription: (id: number) =>
    request<Prescription>(`/prescriptions/${id}/publish`, { method: 'POST' }),

  doctorPrescriptions: (doctorId: number) =>
    request<Prescription[]>(`/prescriptions?doctor_id=${doctorId}`),

  patientPrescriptions: (patientId: number) =>
    request<Prescription[]>(`/patients/${patientId}/prescriptions`),

  getDrugSuggestions: (body: {
    doctor_id: number
    patient_id: number
    diagnosis: string
  }) =>
    request<DrugSuggestionResult>('/ai/drug-suggestions', {
      method: 'POST',
      body: JSON.stringify(body),
    }),

  getInterpretation: (prescriptionId: number, patientId: number) =>
    request<PrescriptionInterpretation>(
      `/ai/prescriptions/${prescriptionId}/interpretation`,
      { method: 'POST', body: JSON.stringify({ patient_id: patientId }) },
    ),
}
