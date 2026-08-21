"""演示账号接口：前端身份切换下拉框的数据源。"""

from fastapi import APIRouter, Depends, Query
from sqlalchemy import select
from sqlalchemy.orm import Session

from ..database import get_db
from ..models import User, UserRole
from ..schemas import UserOut

router = APIRouter(tags=["users"])


@router.get("/users", response_model=list[UserOut])
def list_users(
    role: UserRole = Query(..., description="doctor 或 patient"),
    db: Session = Depends(get_db),
) -> list[User]:
    stmt = select(User).where(User.role == role).order_by(User.id)
    return list(db.scalars(stmt))
