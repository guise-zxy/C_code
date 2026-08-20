# -*- coding: utf-8 -*-
"""实验二 · 简易“功能正确性”自动评分脚本。

它对当前目录的 sort_students.sort_students 运行若干测试用例，
给出功能正确性得分（满分 5），并提示你手动评估“代码风格”和“可读性”。

把你用不同提示词生成的实现保存为 sort_students.py 后运行本脚本，即可横向比较。

运行：
    python3 evaluate.py
"""
from __future__ import annotations
import copy

try:
    from sort_students import sort_students
except Exception as e:  # noqa
    print("无法导入 sort_students：", e)
    raise SystemExit(1)

# 每个用例：(描述, 可调用断言)  断言返回 True 表示通过
CASES = []


def case(desc):
    def deco(fn):
        CASES.append((desc, fn))
        return fn
    return deco


@case("按分数降序排序")
def _():
    out = sort_students([{"name": "A", "score": 80}, {"name": "B", "score": 90}])
    return [s["name"] for s in out] == ["B", "A"]


@case("分数相同按姓名升序")
def _():
    out = sort_students([
        {"name": "Charlie", "score": 88},
        {"name": "Alice", "score": 95},
        {"name": "Bob", "score": 88},
    ])
    return [s["name"] for s in out] == ["Alice", "Bob", "Charlie"]


@case("不修改原列表")
def _():
    data = [{"name": "A", "score": 1}, {"name": "B", "score": 2}]
    snap = copy.deepcopy(data)
    sort_students(data)
    return data == snap


@case("空列表返回空列表")
def _():
    return sort_students([]) == []


@case("非法输入应抛出异常")
def _():
    try:
        sort_students([{"name": "x"}])
        return False
    except Exception:
        return True


def main() -> None:
    passed = 0
    print("=" * 44)
    print(" 功能正确性评测")
    print("=" * 44)
    for desc, fn in CASES:
        try:
            ok = bool(fn())
        except Exception as e:  # noqa
            ok = False
            desc += f"（异常：{e}）"
        print(f"[{'✔' if ok else '✗'}] {desc}")
        passed += int(ok)
    print("-" * 44)
    print(f"功能正确性：{passed}/{len(CASES)}")
    print("\n请再手动评估以下两个维度（各 1~5 分），填入提示词优化日志：")
    print("  · 代码风格：命名/结构是否规范，有无坏味道")
    print("  · 可读性　：注释是否到位，逻辑是否清晰")


if __name__ == "__main__":
    main()
