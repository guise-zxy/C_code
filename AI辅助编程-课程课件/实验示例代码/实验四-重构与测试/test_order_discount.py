# -*- coding: utf-8 -*-
"""实验四 · 验证“重构前后行为一致”的测试（标准库 unittest）。

核心思想：重构的底线是“行为不变”。我们用同一批输入分别喂给
重构前 calc() 与重构后 total_amount()，断言结果一致；并补充边界用例。

运行：
    python3 -m unittest test_order_discount -v
"""
import os
import sys
import random
import unittest

# 让 before/ 与 after/ 可被导入
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "before"))
sys.path.insert(0, os.path.join(HERE, "after"))

import order_discount as _after          # after/order_discount.py
sys.modules.pop("order_discount", None)
# 用文件路径精确加载 before 版本，避免命名冲突
import importlib.util
_spec = importlib.util.spec_from_file_location(
    "before_order_discount", os.path.join(HERE, "before", "order_discount.py"))
_before = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_before)

calc = _before.calc                 # 重构前
total_amount = _after.total_amount  # 重构后


class TestBehaviorPreserved(unittest.TestCase):
    """随机对拍：重构前后结果必须一致。"""

    def test_random_equivalence(self):
        random.seed(42)
        for _ in range(500):
            orders = [
                {"type": random.choice([1, 2, 3, 9]), "price": random.randint(0, 600)}
                for _ in range(random.randint(0, 8))
            ]
            self.assertAlmostEqual(calc(orders), total_amount(orders), places=6)


class TestSpecificCases(unittest.TestCase):
    """针对“后”版本的具体功能与边界用例（AI 生成 + 人工补边界）。"""

    def test_empty(self):
        self.assertEqual(total_amount([]), 0)

    def test_member_rates(self):
        self.assertAlmostEqual(total_amount([{"type": 1, "price": 100}]), 100)
        self.assertAlmostEqual(total_amount([{"type": 2, "price": 100}]), 90)
        self.assertAlmostEqual(total_amount([{"type": 3, "price": 100}]), 80)

    def test_unknown_type_defaults_to_full_price(self):
        # 边界：未知会员类型按原价（人工补充的边界用例）
        self.assertAlmostEqual(total_amount([{"type": 99, "price": 50}]), 50)

    def test_bulk_discount_applied_above_threshold(self):
        # 合计 1200 > 1000，整单 95 折 -> 1140
        self.assertAlmostEqual(total_amount([{"type": 1, "price": 1200}]), 1140)

    def test_bulk_discount_boundary_not_applied_at_threshold(self):
        # 边界：恰好 1000 不触发整单折扣
        self.assertAlmostEqual(total_amount([{"type": 1, "price": 1000}]), 1000)


if __name__ == "__main__":
    unittest.main(verbosity=2)
