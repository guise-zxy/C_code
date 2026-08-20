# -*- coding: utf-8 -*-
"""
实验五 · 课程学习追踪系统 —— HTTP 层（仅用 Python 标准库，零依赖）

启动：
    python3 app.py          # http://localhost:8000
    python3 app.py 8080

REST API：
    GET    /api/courses                列出课程（含进度）
    POST   /api/courses                {name, target_hours} 新增课程
    DELETE /api/courses/<id>           删除课程（级联删除记录）
    GET    /api/courses/<id>/records   该课程的学习记录
    POST   /api/courses/<id>/records   {minutes, note?} 记录一次学习
    GET    /api/stats                  总体统计
    GET    /api/reminders              需要学习的提醒
"""
from __future__ import annotations
import json
import os
import re
import sys
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

from tracker_service import TrackerStore, ValidationError

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
STATIC_DIR = os.path.join(BASE_DIR, "static")
# 若位于 OneDrive/网盘导致 sqlite "disk I/O error"，可用 TRACKER_DB 指向本地路径
DB_PATH = os.environ.get("TRACKER_DB", os.path.join(BASE_DIR, "tracker.db"))
store = TrackerStore(DB_PATH)

_COURSE_ID = re.compile(r"^/api/courses/(\d+)$")
_RECORDS = re.compile(r"^/api/courses/(\d+)/records$")
_CT = {".html": "text/html; charset=utf-8", ".css": "text/css; charset=utf-8",
       ".js": "application/javascript; charset=utf-8"}


class Handler(BaseHTTPRequestHandler):
    server_version = "Tracker/1.0"

    def _json(self, obj, status=200):
        body = json.dumps(obj, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _body(self) -> dict:
        n = int(self.headers.get("Content-Length", 0) or 0)
        if not n:
            return {}
        try:
            data = json.loads(self.rfile.read(n).decode("utf-8"))
        except json.JSONDecodeError:
            raise ValidationError("请求体不是合法 JSON")
        if not isinstance(data, dict):
            raise ValidationError("请求体必须是 JSON 对象")
        return data

    def _static(self, path):
        if path in ("/", ""):
            path = "/index.html"
        safe = os.path.normpath(path).lstrip("/\\")
        full = os.path.join(STATIC_DIR, safe)
        if not full.startswith(STATIC_DIR) or not os.path.isfile(full):
            return self._json({"error": "Not Found"}, 404)
        with open(full, "rb") as f:
            body = f.read()
        self.send_response(200)
        self.send_header("Content-Type", _CT.get(os.path.splitext(full)[1], "application/octet-stream"))
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, *a):
        pass

    def do_GET(self):
        path = self.path.split("?", 1)[0]
        try:
            if path == "/api/courses":
                return self._json(store.list_courses())
            if path == "/api/stats":
                return self._json(store.overall_stats())
            if path == "/api/reminders":
                return self._json(store.reminders())
            m = _RECORDS.match(path)
            if m:
                return self._json(store.list_records(int(m.group(1))))
            if path.startswith("/api/"):
                return self._json({"error": "Not Found"}, 404)
            return self._static(path)
        except ValidationError as e:
            return self._json({"error": str(e)}, 400)

    def do_POST(self):
        try:
            data = self._body()
            if self.path == "/api/courses":
                c = store.add_course(data.get("name"), data.get("target_hours"))
                return self._json(c, 201)
            m = _RECORDS.match(self.path)
            if m:
                r = store.add_record(int(m.group(1)), data.get("minutes"), data.get("note", ""))
                return self._json(r, 201)
            return self._json({"error": "Not Found"}, 404)
        except ValidationError as e:
            return self._json({"error": str(e)}, 400)

    def do_DELETE(self):
        m = _COURSE_ID.match(self.path)
        if not m:
            return self._json({"error": "Not Found"}, 404)
        if not store.delete_course(int(m.group(1))):
            return self._json({"error": "课程不存在"}, 404)
        self.send_response(204)
        self.end_headers()


def run(port: int = 8000) -> None:
    server = ThreadingHTTPServer(("0.0.0.0", port), Handler)
    print(f"✅ 课程学习追踪系统已启动： http://localhost:{port}（Ctrl+C 停止）")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n已停止。")
        server.shutdown()


if __name__ == "__main__":
    run(int(sys.argv[1]) if len(sys.argv) > 1 else 8000)
