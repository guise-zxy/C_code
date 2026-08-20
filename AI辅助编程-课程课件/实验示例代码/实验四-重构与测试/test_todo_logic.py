# -*- coding: utf-8 -*-
"""实验四 · 为核心纯函数生成的单元测试（覆盖 正常 / 边界 / 异常）。

这套测试演示了 AI 生成测试后，人工补充“边界与异常”用例的成果。

运行：
    python3 -m unittest test_todo_logic -v
"""
import unittest
from todo_logic import validate_title, filter_todos, summarize, toggle_done


def make(todos):
    """便捷构造：传入 (title, done) 列表。"""
    return [{"id": i, "title": t, "done": d} for i, (t, d) in enumerate(todos)]


class TestValidateTitle(unittest.TestCase):
    def test_normal(self):
        self.assertEqual(validate_title("看书"), "看书")

    def test_trim(self):  # 边界：首尾空白
        self.assertEqual(validate_title("  跑步 "), "跑步")

    def test_empty_raises(self):  # 异常
        for bad in ["", "   ", "\t"]:
            with self.assertRaises(ValueError):
                validate_title(bad)

    def test_too_long_raises(self):  # 边界：超长
        with self.assertRaises(ValueError):
            validate_title("x" * 201)

    def test_boundary_200_ok(self):  # 边界：恰好 200
        self.assertEqual(len(validate_title("x" * 200)), 200)

    def test_non_string_raises(self):  # 异常：非字符串
        for bad in [None, 123, ["a"]]:
            with self.assertRaises(ValueError):
                validate_title(bad)


class TestFilterTodos(unittest.TestCase):
    def setUp(self):
        self.todos = make([("A", False), ("B", True), ("C", False)])

    def test_all(self):
        self.assertEqual(len(filter_todos(self.todos, "all")), 3)

    def test_active(self):
        self.assertEqual([t["title"] for t in filter_todos(self.todos, "active")], ["A", "C"])

    def test_completed(self):
        self.assertEqual([t["title"] for t in filter_todos(self.todos, "completed")], ["B"])

    def test_empty_list(self):  # 边界：空列表
        self.assertEqual(filter_todos([], "active"), [])

    def test_invalid_status(self):  # 异常
        with self.assertRaises(ValueError):
            filter_todos(self.todos, "bad")


class TestSummarize(unittest.TestCase):
    def test_counts(self):
        todos = make([("A", False), ("B", True), ("C", True)])
        self.assertEqual(summarize(todos), {"total": 3, "active": 1, "completed": 2})

    def test_empty(self):  # 边界
        self.assertEqual(summarize([]), {"total": 0, "active": 0, "completed": 0})


class TestToggleDone(unittest.TestCase):
    def test_toggle_and_immutability(self):
        t = {"id": 1, "title": "X", "done": False}
        out = toggle_done(t)
        self.assertTrue(out["done"])
        self.assertFalse(t["done"])  # 原对象不应被修改


if __name__ == "__main__":
    unittest.main(verbosity=2)
