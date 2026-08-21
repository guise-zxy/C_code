"""处方接口：创建草稿、发布、医生端列表、患者端列表。"""

from datetime import datetime

from fastapi import APIRouter, Depends, HTTPException, Query, status
from sqlalchemy import select
from sqlalchemy.orm import Session

from ..database import get_db
from ..models import (
    Prescription,
    PrescriptionDrug,
    PrescriptionStatus,
    User,
    UserRole,
)
from ..schemas import PrescriptionCreate, PrescriptionOut

router = APIRouter(tags=["prescriptions"])


def _get_user_by_role(db: Session, user_id: int, role: UserRole) -> User:
    user = db.get(User, user_id)
    label = "医生" if role == UserRole.DOCTOR else "患者"
    if user is None or user.role != role:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=f"{label}不存在或不是{label}",
        )
    return user


@router.post(
    "/prescriptions",
    response_model=PrescriptionOut,
    status_code=status.HTTP_201_CREATED,
)
def create_prescription(
    payload: PrescriptionCreate, db: Session = Depends(get_db)
) -> Prescription:
    _get_user_by_role(db, payload.doctor_id, UserRole.DOCTOR)
    _get_user_by_role(db, payload.patient_id, UserRole.PATIENT)
    rx = Prescription(
        doctor_id=payload.doctor_id,
        patient_id=payload.patient_id,
        diagnosis=payload.diagnosis,
        follow_up_date=payload.follow_up_date,
        notes=payload.notes,
        drugs=[
            PrescriptionDrug(**d.model_dump(), sort_order=i)
            for i, d in enumerate(payload.drugs)
        ],
    )
    db.add(rx)
    db.commit()
    db.refresh(rx)
    return rx


@router.post(
    "/prescriptions/{prescription_id}/publish", response_model=PrescriptionOut
)
def publish_prescription(
    prescription_id: int, db: Session = Depends(get_db)
) -> Prescription:
    rx = db.get(Prescription, prescription_id)
    if rx is None:
        raise HTTPException(status_code=404, detail="处方不存在")
    if rx.status == PrescriptionStatus.PUBLISHED:
        raise HTTPException(status_code=409, detail="处方已发布，不能重复发布")
    if not rx.drugs:
        # 防御性：创建接口已保证至少 1 条明细
        raise HTTPException(status_code=409, detail="处方无药品明细，不能发布")
    rx.status = PrescriptionStatus.PUBLISHED
    rx.published_at = datetime.now()
    db.commit()
    db.refresh(rx)
    return rx


@router.get("/prescriptions", response_model=list[PrescriptionOut])
def list_doctor_prescriptions(
    doctor_id: int = Query(...), db: Session = Depends(get_db)
) -> list[Prescription]:
    _get_user_by_role(db, doctor_id, UserRole.DOCTOR)
    stmt = (
        select(Prescription)
        .where(Prescription.doctor_id == doctor_id)
        .order_by(Prescription.created_at.desc(), Prescription.id.desc())
    )
    return list(db.scalars(stmt))


@router.get(
    "/patients/{patient_id}/prescriptions", response_model=list[PrescriptionOut]
)
def list_patient_prescriptions(
    patient_id: int, db: Session = Depends(get_db)
) -> list[Prescription]:
    _get_user_by_role(db, patient_id, UserRole.PATIENT)
    # 仅返回已发布处方：草稿对患者不可见（安全属性，非 UI 逻辑）
    stmt = (
        select(Prescription)
        .where(
            Prescription.patient_id == patient_id,
            Prescription.status == PrescriptionStatus.PUBLISHED,
        )
        .order_by(Prescription.published_at.desc(), Prescription.id.desc())
    )
    return list(db.scalars(stmt))
