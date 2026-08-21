"""SQLAlchemy 数据模型：演示账号、处方、处方药品明细。"""

import enum
from datetime import date, datetime

from sqlalchemy import Date, DateTime, Enum, ForeignKey, Integer, String
from sqlalchemy.orm import Mapped, mapped_column, relationship

from .database import Base


class UserRole(str, enum.Enum):
    DOCTOR = "doctor"
    PATIENT = "patient"


class PrescriptionStatus(str, enum.Enum):
    DRAFT = "DRAFT"
    PUBLISHED = "PUBLISHED"


class User(Base):
    __tablename__ = "users"

    id: Mapped[int] = mapped_column(primary_key=True, autoincrement=True)
    name: Mapped[str] = mapped_column(String(50), nullable=False)
    # native_enum=False：SQLite 上生成 VARCHAR + CHECK，库内存可读字符串
    role: Mapped[UserRole] = mapped_column(
        Enum(UserRole, native_enum=False, length=10), nullable=False
    )


class Prescription(Base):
    __tablename__ = "prescriptions"

    id: Mapped[int] = mapped_column(primary_key=True)
    doctor_id: Mapped[int] = mapped_column(
        ForeignKey("users.id"), nullable=False, index=True
    )
    patient_id: Mapped[int] = mapped_column(
        ForeignKey("users.id"), nullable=False, index=True
    )
    diagnosis: Mapped[str] = mapped_column(String(200), nullable=False)
    status: Mapped[PrescriptionStatus] = mapped_column(
        Enum(PrescriptionStatus, native_enum=False, length=10),
        nullable=False,
        default=PrescriptionStatus.DRAFT,
    )
    follow_up_date: Mapped[date] = mapped_column(Date, nullable=False)
    notes: Mapped[str | None] = mapped_column(String(500))
    # 单机演示用本地 naive 时间；多时区部署需迁移到 aware UTC（本轮不做）
    created_at: Mapped[datetime] = mapped_column(
        DateTime, nullable=False, default=datetime.now
    )
    published_at: Mapped[datetime | None] = mapped_column(DateTime)

    # 两个外键指向同一张 users 表，必须显式指定 foreign_keys
    doctor: Mapped["User"] = relationship(foreign_keys=[doctor_id])
    patient: Mapped["User"] = relationship(foreign_keys=[patient_id])
    drugs: Mapped[list["PrescriptionDrug"]] = relationship(
        back_populates="prescription",
        order_by="PrescriptionDrug.sort_order",
        cascade="all, delete-orphan",
    )

    @property
    def doctor_name(self) -> str:
        return self.doctor.name

    @property
    def patient_name(self) -> str:
        return self.patient.name


class PrescriptionDrug(Base):
    __tablename__ = "prescription_drugs"

    id: Mapped[int] = mapped_column(primary_key=True)
    prescription_id: Mapped[int] = mapped_column(
        ForeignKey("prescriptions.id"), nullable=False, index=True
    )
    drug_name: Mapped[str] = mapped_column(String(100), nullable=False)
    spec: Mapped[str | None] = mapped_column(String(100))  # 规格，如 0.25g×24粒
    dosage: Mapped[str] = mapped_column(String(200), nullable=False)  # 用法用量
    days: Mapped[int | None] = mapped_column(Integer)  # 用药天数 1-365
    quantity: Mapped[str | None] = mapped_column(String(20))  # 数量含单位，如 2盒
    sort_order: Mapped[int] = mapped_column(Integer, nullable=False, default=0)

    prescription: Mapped["Prescription"] = relationship(back_populates="drugs")
