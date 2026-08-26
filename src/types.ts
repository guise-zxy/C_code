export type Role = 'doctor' | 'patient'
export type PrescriptionStatus = 'DRAFT' | 'PUBLISHED'

export interface User {
  id: number
  name: string
  role: Role
}

export interface Drug {
  id: number
  drug_name: string
  spec: string | null
  dosage: string
  days: number | null
  quantity: string | null
  sort_order: number
}

export interface Prescription {
  id: number
  doctor_id: number
  doctor_name: string
  patient_id: number
  patient_name: string
  diagnosis: string
  status: PrescriptionStatus
  follow_up_date: string
  notes: string | null
  created_at: string
  published_at: string | null
  drugs: Drug[]
}

export interface DrugInput {
  drug_name: string
  spec: string | null
  dosage: string
  days: number | null
  quantity: string | null
}

export interface PrescriptionCreate {
  doctor_id: number
  patient_id: number
  diagnosis: string
  follow_up_date: string
  notes: string | null
  drugs: DrugInput[]
}

// ---------- AI 辅助 ----------

export interface DrugSuggestion {
  drug_name: string
  spec: string | null
  dosage: string
  days: number | null
  quantity: string | null
  reason: string
}

export interface DrugSuggestionResult {
  suggestions: DrugSuggestion[]
  model: string
}

export interface PrescriptionInterpretation {
  how_to_take: string[]
  cautions: string[]
  follow_up_note: string
  model: string
}
