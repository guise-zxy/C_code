# -*- coding: utf-8 -*-
"""
实验二 · 基准任务参考实现
任务：对学生列表按 score 降序排序；score 相同时按 name 升序；保持稳定、不修改原列表。

这是经过“四要素 + 链式推理”提示词迭代后得到的高质量参考实现。
请把你自己用三种提示技术生成的结果与本实现对比、评分。

运行：
    python3 sort_students.py
    python3 -m unittest test_sort_students -v
"""
from __future__ import annotations
from typing import TypedDict


class Student(TypedDict):
    name: str
    score: int


def sort_students(students: list[Student]) -> list[Student]:
    """按 score 降序、score 相同按 name 升序，返回排序后的**新列表**。

    实现说明：
      - Python 的 sorted 是稳定排序；
      - 利用元组键 (-score, name) 一次性表达“分数降序、姓名升序”；
      - 不修改入参 students（返回新列表）。

    :param students: 学生列表，每个元素含 name(str) 与 score(int)
    :return: 排序后的新列表
    :raises ValueError: 当某个元素缺少 name/score 字段时
    """
    for s in students:
        if "name" not in s or "score" not in s:
            raise ValueError(f"每个学生必须包含 name 与 score 字段：{s}")
    return sorted(students, key=lambda s: (-s["score"], s["name"]))


def main() -> None:
    data: list[Student] = [
        {"name": "Charlie", "score": 88},
        {"name": "Alice", "score": 95},
        {"name": "Bob", "score": 88},
        {"name": "David", "score": 72},
    ]
    print("排序前：", data)
    print("排序后：", sort_students(data))
    print("原列表未被修改：", data)


if __name__ == "__main__":
    main()
