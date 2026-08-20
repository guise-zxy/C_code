# -*- coding: utf-8 -*-
"""
实验四 · 重构【后】—— 在“行为不变”前提下消除坏味道

重构手法：
  1) 用命名常量替代魔法数字（折扣率、阈值、整单折扣）；
  2) 用映射表（表驱动）替代重复的 if-elif 分支；
  3) 优化命名，拆分关注点，补充类型注解与文档字符串。

⚠ 关键：重构不改变对外可观察的行为。请用 test_order_discount.py 验证
   与重构前 calc() 的结果完全一致。
"""
from __future__ import annotations

# 会员类型 -> 计价折扣率（表驱动，替代魔法数字与重复分支）
MEMBER_DISCOUNT_RATE: dict[int, float] = {
    1: 1.0,   # 普通会员：原价
    2: 0.9,   # 会员：9 折
    3: 0.8,   # VIP：8 折
}
DEFAULT_RATE = 1.0          # 未知类型按原价
BULK_THRESHOLD = 1000       # 整单满减阈值
BULK_DISCOUNT_RATE = 0.95   # 超过阈值后整单折扣


def total_amount(orders: list[dict]) -> float:
    """计算订单合计金额（重构后版本，行为与重构前 calc 一致）。

    :param orders: 订单列表，每个含 type(int) 与 price(number)
    :return: 应付合计金额
    """
    subtotal = sum(
        order["price"] * MEMBER_DISCOUNT_RATE.get(order["type"], DEFAULT_RATE)
        for order in orders
    )
    if subtotal > BULK_THRESHOLD:
        subtotal *= BULK_DISCOUNT_RATE
    return subtotal
