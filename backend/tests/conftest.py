"""pytest 基建：内存 SQLite + dependency_overrides，测试与生产库完全隔离。

- StaticPool：所有线程复用同一条连接，内存库才不会"分裂"成多个空库；
- TestClient 不作上下文管理器使用 → lifespan 不触发 → 不会碰 backend/demo.db；
- autouse fixture 每个测试前建表 + seed、测后 drop，测试间完全隔离。
"""

import pytest
from fastapi.testclient import TestClient
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from sqlalchemy.pool import StaticPool

from app.database import Base, get_db
from app.main import app
from app.seed import seed_demo_data

engine = create_engine(
    "sqlite://",
    connect_args={"check_same_thread": False},
    poolclass=StaticPool,
)
TestingSessionLocal = sessionmaker(
    bind=engine, autoflush=False, expire_on_commit=False
)


def override_get_db():
    db = TestingSessionLocal()
    try:
        yield db
    finally:
        db.close()


app.dependency_overrides[get_db] = override_get_db


@pytest.fixture(autouse=True)
def fresh_db():
    Base.metadata.create_all(bind=engine)
    with TestingSessionLocal() as db:
        seed_demo_data(db)  # 每个测试从 2 医生(id 1,2) + 2 患者(id 3,4) 开始
    yield
    Base.metadata.drop_all(bind=engine)


@pytest.fixture()
def client():
    return TestClient(app)
