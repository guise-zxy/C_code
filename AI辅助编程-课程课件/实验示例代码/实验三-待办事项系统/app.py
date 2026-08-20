# -*- coding: utf-8 -*-
"""
实验三 · 待办事项系统 —— HTTP 层（仅用 Python 标准库，零依赖）

启动：
    python3 app.py          # 默认 http://localhost:8000
    python3 app.py 8080     # 指定端口

REST API：
    GET    /api/todos?status=all|active|completed   列出待办
    POST   /api/todos        body: {"title": "..."}  新增
    PUT    /api/todos/<id>    body: {"title"?, "done"?} 更新（也支持 {"toggle": true}）
    DELETE /api/todos/<id>                            删除
    GET    /api/stats                                 统计

静态页面：GET / 返回 static/index.html
"""
from __future__ import annotations
import json
import os
import re
import sys
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

from todo_service import TodoStore, ValidationError

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
STATIC_DIR = os.path.join(BASE_DIR, "static")
# 数据库路径可用环境变量 TODO_DB 覆盖。
# 若项目位于 OneDrive/网盘等同步目录导致 sqlite "disk I/O error"，
# 可设为本地路径，例如：TODO_DB=/tmp/todo.db python3 app.py
DB_PATH = os.environ.get("TODO_DB", os.path.join(BASE_DIR, "todo.db"))
store = TodoStore(DB_PATH)

_ID_RE = re.compile(r"^/api/todos/(\d+)$")
_CONTENT_TYPES = {
    ".html": "text/html; charset=utf-8",
    ".css": "text/css; charset=utf-8",
    ".js": "application/javascript; charset=utf-8",
}


class Handler(BaseHTTPRequestHandler):
    server_version = "TodoApp/1.0"

    # ---------- 工具方法 ----------
    def _send_json(self, obj, status=200):
        body = json.dumps(obj, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _read_json(self) -> dict:
        length = int(self.headers.get("Content-Length", 0) or 0)
        if length == 0:
            return {}
        raw = self.rfile.read(length)
        try:
            data = json.loads(raw.decode("utf-8"))
        except json.JSONDecodeError:
            raise ValidationError("请求体不是合法的 JSON")
        if not isinstance(data, dict):
            raise ValidationError("请求体必须是 JSON 对象")
        return data

    def _serve_static(self, path):
        if path == "/" or path == "":
            path = "/index.html"
        # 防止路径穿越
        safe = os.path.normpath(path).lstrip("/\\")
        full = os.path.join(STATIC_DIR, safe)
        if not full.startswith(STATIC_DIR) or not os.path.isfile(full):
            self._send_json({"error": "Not Found"}, 404)
            return
        ext = os.path.splitext(full)[1]
        with open(full, "rb") as f:
            body = f.read()
        self.send_response(200)
        self.send_header("Content-Type", _CONTENT_TYPES.get(ext, "application/octet-stream"))
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, fmt, *args):  # 静默默认访问日志，保持输出整洁
        pass

    # ---------- 路由 ----------
    def do_GET(self):
        path = self.path.split("?", 1)[0]
        if path == "/api/stats":
            return self._send_json(store.stats())
        if path == "/api/todos":
            status = "all"
            if "?" in self.path:
                query = self.path.split("?", 1)[1]
                for kv in query.split("&"):
                    if kv.startswith("status="):
                        status = kv.split("=", 1)[1]
            try:
                return self._send_json(store.list(status))
            except ValidationError as e:
                return self._send_json({"error": str(e)}, 400)
        if path.startswith("/api/"):
            return self._send_json({"error": "Not Found"}, 404)
        return self._serve_static(path)

    def do_POST(self):
        if self.path != "/api/todos":
            return self._send_json({"error": "Not Found"}, 404)
        try:
            data = self._read_json()
            todo = store.add(data.get("title"))
            return self._send_json(todo, 201)
        except ValidationError as e:
            return self._send_json({"error": str(e)}, 400)

    def do_PUT(self):
        m = _ID_RE.match(self.path)
        if not m:
            return self._send_json({"error": "Not Found"}, 404)
        todo_id = int(m.group(1))
        try:
            data = self._read_json()
            if data.get("toggle"):
                todo = store.toggle(todo_id)
            else:
                todo = store.update(
                    todo_id,
                    title=data.get("title"),
                    done=data.get("done"),
                )
            if todo is None:
                return self._send_json({"error": "待办不存在"}, 404)
            return self._send_json(todo)
        except ValidationError as e:
            return self._send_json({"error": str(e)}, 400)

    def do_DELETE(self):
        m = _ID_RE.match(self.path)
        if not m:
            return self._send_json({"error": "Not Found"}, 404)
        ok = store.delete(int(m.group(1)))
        if not ok:
            return self._send_json({"error": "待办不存在"}, 404)
        self.send_response(204)
        self.end_headers()


def run(port: int = 8000) -> None:
    server = ThreadingHTTPServer(("0.0.0.0", port), Handler)
    print(f"✅ 待办事项系统已启动： http://localhost:{port}")
    print("   按 Ctrl+C 停止。")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n已停止。")
        server.shutdown()


if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8000
    run(port)
