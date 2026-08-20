# -*- coding: utf-8 -*-
"""实验二 · 参考实现的单元测试，也是“功能正确性”维度的评分依据。

把你用不同提示词生成的函数命名为 sort_students 并替换本目录的实现，
即可用这套测试客观比较“功能正确性”。

运行：
    python3 -m unittest test_sort_students -v
"""
import copy
import unittest
from sort_students import sort_students


class TestSortStudents(unittest.TestCase):
    def test_basic_desc(self):
        data = [{"name": "A", "score": 80}, {"name": "B", "score": 90}]
        self.assertEqual([s["name"] for s in sort_students(data)], ["B", "A"])

    def test_tie_break_by_name(self):
        data = [
            {"name": "Charlie", "score": 88},
            {"name": "Alice", "score": 95},
            {"name": "Bob", "score": 88},
        ]
        self.assertEqual(
            [s["name"] for s in sort_students(data)],
            ["Alice", "Bob", "Charlie"],
        )

    def test_does_not_mutate_input(self):
        data = [{"name": "A", "score": 1}, {"name": "B", "score": 2}]
        snapshot = copy.deepcopy(data)
        sort_students(data)
        self.assertEqual(data, snapshot)  # 原列表不应被修改

    def test_empty(self):
        self.assertEqual(sort_students([]), [])

    def test_missing_field_raises(self):
        with self.assertRaises(ValueError):
            sort_students([{"name": "NoScore"}])


if __name__ == "__main__":
    unittest.main(verbosity=2)
