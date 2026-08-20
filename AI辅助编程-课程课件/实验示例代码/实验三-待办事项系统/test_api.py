# -*- coding: utf-8 -*-
"""实验三 · HTTP API 冒烟测试：真实启动服务器并用 urllib 访问（标准库）。

运行：
    python3 -m unittest test_api -v
"""
import json
import threading
import unittest
import urllib.request
import urllib.error
from http.server import ThreadingHTTPServer

import app
from todo_service import TodoStore


def http(method, path, body=None, port=0):
    url = f"http://127.0.0.1:{port}{path}"
    data = json.dumps(body).encode() if body is not None else None
    req = urllib.request.Request(url, data=data, method=method)
    if data is not None:
        req.add_header("Content-Type", "application/json")
    try:
        with urllib.request.urlopen(req) as resp:
            raw = resp.read()
            return resp.status, (json.loads(raw) if raw else None)
    except urllib.error.HTTPError as e:
        raw = e.read()
        return e.code, (json.loads(raw) if raw else None)


class TestApi(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # 用内存数据库替换默认 store，避免污染真实 todo.db
        app.store = TodoStore(":memory:")
        cls.server = ThreadingHTTPServer(("127.0.0.1", 0), app.Handler)
        cls.port = cls.server.server_address[1]
        cls.thread = threading.Thread(target=cls.server.serve_forever, daemon=True)
        cls.thread.start()

    @classmethod
    def tearDownClass(cls):
        cls.server.shutdown()
        app.store.close()

    def test_crud_flow(self):
        p = self.port
        # 初始为空
        status, todos = http("GET", "/api/todos", port=p)
        self.assertEqual(status, 200)
        self.assertEqual(todos, [])

        # 新增
        status, t = http("POST", "/api/todos", {"title": "学习AI编程"}, port=p)
        self.assertEqual(status, 201)
        self.assertEqual(t["title"], "学习AI编程")
        tid = t["id"]

        # 空标题 -> 400
        status, err = http("POST", "/api/todos", {"title": "  "}, port=p)
        self.assertEqual(status, 400)
        self.assertIn("error", err)

        # 标记完成
        status, t = http("PUT", f"/api/todos/{tid}", {"toggle": True}, port=p)
        self.assertEqual(status, 200)
        self.assertTrue(t["done"])

        # 按状态筛选
        _, active = http("GET", "/api/todos?status=active", port=p)
        self.assertEqual(active, [])
        _, completed = http("GET", "/api/todos?status=completed", port=p)
        self.assertEqual(len(completed), 1)

        # 统计
        _, stats = http("GET", "/api/stats", port=p)
        self.assertEqual(stats, {"total": 1, "active": 0, "completed": 1})

        # 更新不存在 -> 404
        status, _ = http("PUT", "/api/todos/9999", {"title": "x"}, port=p)
        self.assertEqual(status, 404)

        # 删除
        status, _ = http("DELETE", f"/api/todos/{tid}", port=p)
        self.assertEqual(status, 204)
        status, _ = http("DELETE", f"/api/todos/{tid}", port=p)
        self.assertEqual(status, 404)


if __name__ == "__main__":
    unittest.main(verbosity=2)
