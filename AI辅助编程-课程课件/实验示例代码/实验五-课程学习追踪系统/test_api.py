# -*- coding: utf-8 -*-
"""实验五 · HTTP API 冒烟测试（真实启动服务器 + urllib，标准库）。

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
from tracker_service import TrackerStore


def http(method, path, body=None, port=0):
    data = json.dumps(body).encode() if body is not None else None
    req = urllib.request.Request(f"http://127.0.0.1:{port}{path}", data=data, method=method)
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
        app.store = TrackerStore(":memory:")
        cls.server = ThreadingHTTPServer(("127.0.0.1", 0), app.Handler)
        cls.port = cls.server.server_address[1]
        cls.t = threading.Thread(target=cls.server.serve_forever, daemon=True)
        cls.t.start()

    @classmethod
    def tearDownClass(cls):
        cls.server.shutdown()
        app.store.close()

    def test_flow(self):
        p = self.port
        # 新增课程
        st, c = http("POST", "/api/courses", {"name": "数据结构", "target_hours": 1}, port=p)
        self.assertEqual(st, 201)
        cid = c["id"]
        # 非法课程
        st, _ = http("POST", "/api/courses", {"name": "", "target_hours": 1}, port=p)
        self.assertEqual(st, 400)
        # 记录学习
        st, r = http("POST", f"/api/courses/{cid}/records", {"minutes": 30, "note": "看书"}, port=p)
        self.assertEqual(st, 201)
        # 列表带进度
        st, courses = http("GET", "/api/courses", port=p)
        self.assertEqual(courses[0]["progress_percent"], 50.0)
        # 统计
        st, stats = http("GET", "/api/stats", port=p)
        self.assertEqual(stats["course_count"], 1)
        # 记录
        st, recs = http("GET", f"/api/courses/{cid}/records", port=p)
        self.assertEqual(len(recs), 1)
        # 删除
        st, _ = http("DELETE", f"/api/courses/{cid}", port=p)
        self.assertEqual(st, 204)
        st, _ = http("DELETE", f"/api/courses/{cid}", port=p)
        self.assertEqual(st, 404)


if __name__ == "__main__":
    unittest.main(verbosity=2)
