# -*- coding: utf-8 -*-
"""
实验一 · 工具基础功能体验 —— 示例函数
本文件演示“用 AI 行内补全完成简单函数，并由 AI 生成注释/文档字符串”的成果。
请逐行阅读，理解每个函数的实现（这是 AI 协作中“人把关”的关键习惯）。

运行：
    python3 hello.py            # 看演示输出
    python3 -m unittest test_hello -v   # 运行测试
"""
from __future__ import annotations


def celsius_to_fahrenheit(c: float) -> float:
    """将摄氏度转换为华氏度。

    公式：F = C * 9/5 + 32
    :param c: 摄氏温度
    :return: 对应的华氏温度
    """
    return c * 9 / 5 + 32


def fahrenheit_to_celsius(f: float) -> float:
    """将华氏度转换为摄氏度。

    公式：C = (F - 32) * 5/9
    """
    return (f - 32) * 5 / 9


def is_prime(n: int) -> bool:
    """判断 n 是否为质数（素数）。

    - n < 2 时返回 False；
    - 仅需试除到 sqrt(n)，提升效率。
    """
    if n < 2:
        return False
    i = 2
    while i * i <= n:
        if n % i == 0:
            return False
        i += 1
    return True


def primes_upto(limit: int) -> list[int]:
    """返回 [2, limit] 区间内的所有质数列表。"""
    return [x for x in range(2, limit + 1) if is_prime(x)]


def fizzbuzz(n: int) -> list[str]:
    """生成 1..n 的 FizzBuzz 序列。

    - 能被 3 整除 -> "Fizz"；能被 5 整除 -> "Buzz"；
    - 同时能被 3 和 5 整除 -> "FizzBuzz"；否则为数字本身。
    """
    result: list[str] = []
    for i in range(1, n + 1):
        if i % 15 == 0:
            result.append("FizzBuzz")
        elif i % 3 == 0:
            result.append("Fizz")
        elif i % 5 == 0:
            result.append("Buzz")
        else:
            result.append(str(i))
    return result


def main() -> None:
    print("0°C  =", celsius_to_fahrenheit(0), "°F")
    print("100°C =", celsius_to_fahrenheit(100), "°F")
    print("98.6°F =", round(fahrenheit_to_celsius(98.6), 1), "°C")
    print("20 以内的质数：", primes_upto(20))
    print("FizzBuzz(15)：", fizzbuzz(15))


if __name__ == "__main__":
    main()
