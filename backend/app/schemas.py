"""Pydantic v2 请求/响应模型（API 契约）。"""

from datetime import date, datetime

from pydantic import BaseModel, ConfigDict, Field, field_validator

from .models import PrescriptionStatus, UserRole


class UserOut(BaseModel):
    model_config = ConfigDict(from_attributes=True)

    id: int
    name: str
    role: UserRole


class DrugIn(BaseModel):
    drug_name: str = Field(min_length=1, max_length=100)
    spec: str | None = Field(default=None, max_length=100)
    dosage: str = Field(min_length=1, max_length=200)
    days: int | None = Field(default=None, ge=1, le=365)
    quantity: str | None = Field(default=None, max_length=20)


class PrescriptionCreate(BaseModel):
    doctor_id: int
    patient_id: int
    diagnosis: str = Field(min_length=1, max_length=200)
    follow_up_date: date
    notes: str | None = Field(default=None, max_length=500)
    drugs: list[DrugIn] = Field(min_length=1)

    @field_validator("follow_up_date")
    @classmethod
    def _not_in_past(cls, v: date) -> date:
        if v < date.today():
            raise ValueError("复诊日期不能早于今天")
        return v


class DrugOut(DrugIn):
    model_config = ConfigDict(from_attributes=True)

    id: int
    sort_order: int


class PrescriptionOut(BaseModel):
    model_config = ConfigDict(from_attributes=True)

    id: int
    doctor_id: int
    doctor_name: str  # 模型上的 @property
    patient_id: int
    patient_name: str
    diagnosis: str
    status: PrescriptionStatus
    follow_up_date: date
    notes: str | None
    created_at: datetime
    published_at: datetime | None
    drugs: list[DrugOut]
