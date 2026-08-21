"""演示数据：库为空时写入 2 名医生 + 2 名患者（幂等）。"""

from sqlalchemy import func, select
from sqlalchemy.orm import Session

from .models import User, UserRole

DEMO_USERS = [
    ("李文", UserRole.DOCTOR),
    ("陈华", UserRole.DOCTOR),
    ("王芳", UserRole.PATIENT),
    ("赵磊", UserRole.PATIENT),
]


def seed_demo_data(db: Session) -> None:
    if db.scalar(select(func.count()).select_from(User)) > 0:
        return
    db.add_all([User(name=name, role=role) for name, role in DEMO_USERS])
    db.commit()
