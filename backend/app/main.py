"""FastAPI 应用入口：建表 + 演示数据 seed + 路由注册。"""

from contextlib import asynccontextmanager

from fastapi import FastAPI

from . import models  # noqa: F401  # 导入以注册模型，create_all 才能看到表
from .database import Base, SessionLocal, engine
from .routers import prescriptions, users
from .seed import seed_demo_data


@asynccontextmanager
async def lifespan(app: FastAPI):
    Base.metadata.create_all(bind=engine)
    with SessionLocal() as db:
        seed_demo_data(db)
    yield


app = FastAPI(
    title="处方同步与复诊提醒系统 Demo",
    version="0.1.0",
    lifespan=lifespan,
)

app.include_router(users.router, prefix="/api")
app.include_router(prescriptions.router, prefix="/api")
