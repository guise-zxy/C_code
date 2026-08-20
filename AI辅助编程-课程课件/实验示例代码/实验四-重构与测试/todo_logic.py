# -*- coding: utf-8 -*-
"""
实验四 · 待办系统“核心纯函数”——用于练习“AI 为核心函数自动生成单元测试”。

这些函数无副作用、不依赖数据库与框架，最适合编写单元测试。
（与实验三的业务逻辑对应，抽成纯函数版本便于讲解测试覆盖。）
"""
from __future__ import annotations

VALID_STATUS = ("all", "active", "completed")


def validate_title(title) -> str:
    """校验并规范化待办标题。

    规则：必须为字符串；去除首尾空白后非空；长度 <= 200。
    :raises ValueError: 不满足规则时
    """
    if not isinstance(title, str):
        raise ValueError("title 必须是字符串")
    title = title.strip()
    if not title:
        raise ValueError("title 不能为空")
    if len(title) > 200:
        raise ValueError("title 长度不能超过 200")
    return title


def filter_todos(todos: list[dict], status: str = "all") -> list[dict]:
    """按状态筛选待办。status ∈ {all, active, completed}。"""
    if status not in VALID_STATUS:
        raise ValueError(f"status 仅支持 {VALID_STATUS}")
    if status == "active":
        return [t for t in todos if not t["done"]]
    if status == "completed":
        return [t for t in todos if t["done"]]
    return list(todos)


def summarize(todos: list[dict]) -> dict:
    """统计 {total, active, completed}。"""
    total = len(todos)
    completed = sum(1 for t in todos if t["done"])
    return {"total": total, "active": total - completed, "completed": completed}


def toggle_done(todo: dict) -> dict:
    """返回一个 done 取反后的**新** todo（不修改入参）。"""
    new = dict(todo)
    new["done"] = not todo["done"]
    return new
