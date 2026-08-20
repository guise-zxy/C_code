# -*- coding: utf-8 -*-
"""
实验五 · 课程学习追踪系统 —— 业务逻辑层（sqlite3，与 Web 框架解耦）

实体：
  Course  课程   : id, name, target_hours(目标学时), created_at
  Record  学习记录: id, course_id, minutes(本次分钟数), note, study_date(YYYY-MM-DD)

主要能力：登记课程、记录学习、计算进度、统计报表、生成“需要学习”提醒。
仅使用 Python 标准库，零第三方依赖。
"""
from __future__ import annotations
import sqlite3
import threading
from datetime import date, datetime


class ValidationError(ValueError):
    """输入校验失败（HTTP 层据此返回 400）。"""


class TrackerStore:
    def __init__(self, db_path: str = "tracker.db") -> None:
        self._conn = sqlite3.connect(db_path, check_same_thread=False)
        self._conn.row_factory = sqlite3.Row
        self._conn.execute("PRAGMA foreign_keys = ON")
        self._lock = threading.Lock()
        self._init_schema()

    def _init_schema(self) -> None:
        with self._lock, self._conn:
            self._conn.execute(
                """CREATE TABLE IF NOT EXISTS courses(
                    id           INTEGER PRIMARY KEY AUTOINCREMENT,
                    name         TEXT    NOT NULL,
                    target_hours REAL    NOT NULL,
                    created_at   TEXT    NOT NULL
                )"""
            )
            self._conn.execute(
                """CREATE TABLE IF NOT EXISTS records(
                    id         INTEGER PRIMARY KEY AUTOINCREMENT,
                    course_id  INTEGER NOT NULL,
                    minutes    INTEGER NOT NULL,
                    note       TEXT    NOT NULL DEFAULT '',
                    study_date TEXT    NOT NULL,
                    FOREIGN KEY(course_id) REFERENCES courses(id) ON DELETE CASCADE
                )"""
            )

    # ---------------- 校验 ----------------
    @staticmethod
    def _clean_name(name) -> str:
        if not isinstance(name, str):
            raise ValidationError("课程名必须是字符串")
        name = name.strip()
        if not name:
            raise ValidationError("课程名不能为空")
        if len(name) > 100:
            raise ValidationError("课程名不能超过 100 个字符")
        return name

    @staticmethod
    def _check_target_hours(h) -> float:
        try:
            h = float(h)
        except (TypeError, ValueError):
            raise ValidationError("目标学时必须是数字")
        if h <= 0:
            raise ValidationError("目标学时必须为正数")
        return h

    @staticmethod
    def _check_minutes(m) -> int:
        if isinstance(m, bool) or not isinstance(m, int):
            raise ValidationError("学习时长(minutes)必须是整数")
        if m <= 0:
            raise ValidationError("学习时长必须为正整数")
        if m > 24 * 60:
            raise ValidationError("单次学习时长不能超过 1440 分钟")
        return m

    # ---------------- 课程 ----------------
    def add_course(self, name: str, target_hours: float) -> dict:
        name = self._clean_name(name)
        target_hours = self._check_target_hours(target_hours)
        created_at = datetime.now().isoformat(timespec="seconds")
        with self._lock, self._conn:
            cur = self._conn.execute(
                "INSERT INTO courses(name, target_hours, created_at) VALUES (?,?,?)",
                (name, target_hours, created_at),
            )
            cid = cur.lastrowid
        return self.get_course(cid)  # type: ignore[return-value]

    def get_course(self, course_id: int) -> dict | None:
        with self._lock:
            row = self._conn.execute(
                "SELECT * FROM courses WHERE id = ?", (course_id,)
            ).fetchone()
            if row is None:
                return None
            agg = self._conn.execute(
                "SELECT COALESCE(SUM(minutes),0) AS m, MAX(study_date) AS last "
                "FROM records WHERE course_id = ?", (course_id,)
            ).fetchone()
        return self._build_course(row, agg["m"], agg["last"])

    def list_courses(self) -> list[dict]:
        with self._lock:
            rows = self._conn.execute(
                """SELECT c.*, COALESCE(SUM(r.minutes),0) AS m, MAX(r.study_date) AS last
                   FROM courses c LEFT JOIN records r ON r.course_id = c.id
                   GROUP BY c.id ORDER BY c.id DESC"""
            ).fetchall()
        return [self._build_course(r, r["m"], r["last"]) for r in rows]

    def delete_course(self, course_id: int) -> bool:
        with self._lock, self._conn:
            cur = self._conn.execute("DELETE FROM courses WHERE id = ?", (course_id,))
        return cur.rowcount > 0

    @staticmethod
    def _build_course(row, studied_minutes: int, last_study: str | None) -> dict:
        studied_hours = round(studied_minutes / 60, 2)
        target = row["target_hours"]
        percent = min(100, round(studied_hours / target * 100, 1)) if target else 0
        days_since = None
        if last_study:
            try:
                d = datetime.strptime(last_study, "%Y-%m-%d").date()
                days_since = (date.today() - d).days
            except ValueError:
                days_since = None
        return {
            "id": row["id"],
            "name": row["name"],
            "target_hours": target,
            "studied_hours": studied_hours,
            "progress_percent": percent,
            "completed": studied_hours >= target,
            "last_study": last_study,
            "days_since_last": days_since,
            "created_at": row["created_at"],
        }

    # ---------------- 学习记录 ----------------
    def add_record(self, course_id: int, minutes: int, note: str = "",
                   study_date: str | None = None) -> dict:
        if self.get_course(course_id) is None:
            raise ValidationError("课程不存在")
        minutes = self._check_minutes(minutes)
        note = (note or "").strip()[:200]
        study_date = study_date or date.today().isoformat()
        with self._lock, self._conn:
            cur = self._conn.execute(
                "INSERT INTO records(course_id, minutes, note, study_date) VALUES (?,?,?,?)",
                (course_id, minutes, note, study_date),
            )
            rid = cur.lastrowid
            row = self._conn.execute("SELECT * FROM records WHERE id = ?", (rid,)).fetchone()
        return dict(row)

    def list_records(self, course_id: int) -> list[dict]:
        with self._lock:
            rows = self._conn.execute(
                "SELECT * FROM records WHERE course_id = ? ORDER BY id DESC", (course_id,)
            ).fetchall()
        return [dict(r) for r in rows]

    # ---------------- 统计与提醒 ----------------
    def overall_stats(self) -> dict:
        today = date.today().isoformat()
        with self._lock:
            course_count = self._conn.execute("SELECT COUNT(*) FROM courses").fetchone()[0]
            total_minutes = self._conn.execute(
                "SELECT COALESCE(SUM(minutes),0) FROM records").fetchone()[0]
            today_minutes = self._conn.execute(
                "SELECT COALESCE(SUM(minutes),0) FROM records WHERE study_date = ?",
                (today,)).fetchone()[0]
        completed = sum(1 for c in self.list_courses() if c["completed"])
        return {
            "course_count": course_count,
            "total_hours": round(total_minutes / 60, 2),
            "today_minutes": today_minutes,
            "completed_courses": completed,
        }

    def reminders(self, threshold_days: int = 3) -> list[dict]:
        """返回需要学习的课程：未完成，且“从未学习”或“距上次学习 >= threshold_days 天”。"""
        result = []
        for c in self.list_courses():
            if c["completed"]:
                continue
            ds = c["days_since_last"]
            if ds is None:
                c["reason"] = "尚未开始学习"
                result.append(c)
            elif ds >= threshold_days:
                c["reason"] = f"已 {ds} 天未学习"
                result.append(c)
        return result

    def close(self) -> None:
        self._conn.close()
