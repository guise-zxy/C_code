# -*- coding: utf-8 -*-
"""实验三 · 业务逻辑层单元测试（标准库 unittest，使用内存数据库，互不污染）。

运行：
    python3 -m unittest test_todo_service -v
"""
import unittest
from todo_service import TodoStore, ValidationError


class TestTodoStore(unittest.TestCase):
    def setUp(self):
        # ":memory:" 数据库随连接存在，测试间隔离
        self.store = TodoStore(":memory:")

    def tearDown(self):
        self.store.close()

    def test_add_and_get(self):
        t = self.store.add("买牛奶")
        self.assertEqual(t["title"], "买牛奶")
        self.assertFalse(t["done"])
        self.assertEqual(self.store.get(t["id"])["title"], "买牛奶")

    def test_title_is_trimmed(self):
        t = self.store.add("  写作业  ")
        self.assertEqual(t["title"], "写作业")

    def test_empty_title_rejected(self):
        with self.assertRaises(ValidationError):
            self.store.add("   ")
        with self.assertRaises(ValidationError):
            self.store.add("")

    def test_non_string_title_rejected(self):
        with self.assertRaises(ValidationError):
            self.store.add(123)

    def test_list_filter(self):
        a = self.store.add("A")
        b = self.store.add("B")
        self.store.update(b["id"], done=True)
        self.assertEqual(len(self.store.list("all")), 2)
        self.assertEqual([t["title"] for t in self.store.list("active")], ["A"])
        self.assertEqual([t["title"] for t in self.store.list("completed")], ["B"])

    def test_invalid_status(self):
        with self.assertRaises(ValidationError):
            self.store.list("unknown")

    def test_update_title_and_done(self):
        t = self.store.add("旧标题")
        u = self.store.update(t["id"], title="新标题", done=True)
        self.assertEqual(u["title"], "新标题")
        self.assertTrue(u["done"])

    def test_update_missing_returns_none(self):
        self.assertIsNone(self.store.update(9999, title="x"))

    def test_toggle(self):
        t = self.store.add("切换")
        self.assertTrue(self.store.toggle(t["id"])["done"])
        self.assertFalse(self.store.toggle(t["id"])["done"])

    def test_delete(self):
        t = self.store.add("待删除")
        self.assertTrue(self.store.delete(t["id"]))
        self.assertFalse(self.store.delete(t["id"]))  # 再次删除应失败
        self.assertIsNone(self.store.get(t["id"]))

    def test_clear_completed_and_stats(self):
        self.store.add("A")
        b = self.store.add("B")
        self.store.update(b["id"], done=True)
        self.assertEqual(self.store.stats(), {"total": 2, "active": 1, "completed": 1})
        self.assertEqual(self.store.clear_completed(), 1)
        self.assertEqual(self.store.stats(), {"total": 1, "active": 1, "completed": 0})


if __name__ == "__main__":
    unittest.main(verbosity=2)
