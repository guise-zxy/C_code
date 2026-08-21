"""数据库引擎与会话管理。

SQLite 路径由 __file__ 推导为绝对路径并转为正斜杠，
与启动目录无关，也规避中文工作目录下相对路径的坑。
"""

import os
from pathlib import Path

from sqlalchemy import create_engine
from sqlalchemy.orm import DeclarativeBase, sessionmaker

BASE_DIR = Path(__file__).resolve().parent.parent  # backend/
DATABASE_URL = os.environ.get(
    "DATABASE_URL", f"sqlite:///{(BASE_DIR / 'demo.db').as_posix()}"
)


class Base(DeclarativeBase):
    pass


engine = create_engine(
    DATABASE_URL,
    connect_args={"check_same_thread": False},  # FastAPI 多线程访问 SQLite 必加
)

# expire_on_commit=False：commit 后 Pydantic 序列化不再触发过期属性的重查询
SessionLocal = sessionmaker(bind=engine, autoflush=False, expire_on_commit=False)


def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()
