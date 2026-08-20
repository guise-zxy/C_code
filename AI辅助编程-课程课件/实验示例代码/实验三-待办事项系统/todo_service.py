# -*- coding: utf-8 -*-
"""
实验三 · 待办事项系统 —— 业务逻辑层（与 Web 框架解耦，便于单元测试）

设计要点（这是实验三/实验四共同强调的“可测试架构”）：
  - 业务逻辑与数据访问集中在 TodoStore，不依赖任何 Web 框架；
  - 仅使用 Python 标准库 sqlite3，零第三方依赖；
  - HTTP 层（app.py）只负责解析请求、调用本模块、返回 JSON。

一个待办事项（todo）的结构：
    {"id": int, "title": str, "done": bool, "created_at": str}
"""
from __future__ import annotations
import sqlite3
import threading
from datetime import datetime


class ValidationError(ValueError):
    """输入校验失败时抛出（HTTP 层据此返回 400）。"""


class TodoStore:
    def __init__(self, db_path: str = "todo.db") -> None:
        # check_same_thread=False + 锁：支持多线程 HTTP 服务器
        self._conn = sqlite3.connect(db_path, check_same_thread=False)
        self._conn.row_factory = sqlite3.Row
        self._lock = threading.Lock()
        self._init_schema()

    def _init_schema(self) -> None:
        with self._lock, self._conn:
            self._conn.execute(
                """
                CREATE TABLE IF NOT EXISTS todos (
                    id         INTEGER PRIMARY KEY AUTOINCREMENT,
                    title      TEXT    NOT NULL,
                    done       INTEGER NOT NULL DEFAULT 0,
                    created_at TEXT    NOT NULL
                )
                """
            )

    @staticmethod
    def _row_to_dict(row: sqlite3.Row) -> dict:
        return {
            "id": row["id"],
            "title": row["title"],
            "done": bool(row["done"]),
            "created_at": row["created_at"],
        }

    @staticmethod
    def _clean_title(title) -> str:
        if not isinstance(title, str):
            raise ValidationError("title 必须是字符串")
        title = title.strip()
        if not title:
            raise ValidationError("title 不能为空")
        if len(title) > 200:
            raise ValidationError("title 长度不能超过 200 个字符")
        return title

    # ---------------- CRUD ----------------
    def add(self, title: str) -> dict:
        """新增待办，返回创建的记录。"""
        title = self._clean_title(title)
        created_at = datetime.now().isoformat(timespec="seconds")
        with self._lock, self._conn:
            cur = self._conn.execute(
                "INSERT INTO todos(title, done, created_at) VALUES (?, 0, ?)",
                (title, created_at),
            )
            new_id = cur.lastrowid
        return self.get(new_id)  # type: ignore[return-value]

    def list(self, status: str = "all") -> list[dict]:
        """列出待办。status ∈ {all, active, completed}。"""
        if status not in ("all", "active", "completed"):
            raise ValidationError("status 仅支持 all / active / completed")
        sql = "SELECT * FROM todos"
        if status == "active":
            sql += " WHERE done = 0"
        elif status == "completed":
            sql += " WHERE done = 1"
        sql += " ORDER BY done ASC, id DESC"
        with self._lock:
            rows = self._conn.execute(sql).fetchall()
        return [self._row_to_dict(r) for r in rows]

    def get(self, todo_id: int) -> dict | None:
        with self._lock:
            row = self._conn.execute(
                "SELECT * FROM todos WHERE id = ?", (todo_id,)
            ).fetchone()
        return self._row_to_dict(row) if row else None

    def update(self, todo_id: int, title=None, done=None) -> dict | None:
        """更新标题或完成状态；记录不存在返回 None。"""
        current = self.get(todo_id)
        if current is None:
            return None
        new_title = current["title"] if title is None else self._clean_title(title)
        new_done = current["done"] if done is None else bool(done)
        with self._lock, self._conn:
            self._conn.execute(
                "UPDATE todos SET title = ?, done = ? WHERE id = ?",
                (new_title, 1 if new_done else 0, todo_id),
            )
        return self.get(todo_id)

    def toggle(self, todo_id: int) -> dict | None:
        """切换完成状态的便捷方法。"""
        current = self.get(todo_id)
        if current is None:
            return None
        return self.update(todo_id, done=not current["done"])

    def delete(self, todo_id: int) -> bool:
        """删除待办；返回是否删除成功。"""
        with self._lock, self._conn:
            cur = self._conn.execute("DELETE FROM todos WHERE id = ?", (todo_id,))
        return cur.rowcount > 0

    def clear_completed(self) -> int:
        """删除所有已完成项，返回删除数量。"""
        with self._lock, self._conn:
            cur = self._conn.execute("DELETE FROM todos WHERE done = 1")
        return cur.rowcount

    def stats(self) -> dict:
        """返回 {total, active, completed} 统计。"""
        with self._lock:
            total = self._conn.execute("SELECT COUNT(*) FROM todos").fetchone()[0]
            done = self._conn.execute(
                "SELECT COUNT(*) FROM todos WHERE done = 1"
            ).fetchone()[0]
        return {"total": total, "active": total - done, "completed": done}

    def close(self) -> None:
        self._conn.close()
