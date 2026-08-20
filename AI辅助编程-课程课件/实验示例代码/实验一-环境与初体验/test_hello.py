# -*- coding: utf-8 -*-
"""实验一 · hello.py 的单元测试（标准库 unittest，无需安装任何依赖）。

运行方式（任选其一）：
    python3 -m unittest test_hello -v
    pytest test_hello.py          # 若已安装 pytest，也可直接运行
"""
import unittest
from hello import (
    celsius_to_fahrenheit, fahrenheit_to_celsius,
    is_prime, primes_upto, fizzbuzz,
)


class TestTemperature(unittest.TestCase):
    def test_c2f(self):
        self.assertEqual(celsius_to_fahrenheit(0), 32)
        self.assertEqual(celsius_to_fahrenheit(100), 212)

    def test_f2c(self):
        self.assertAlmostEqual(fahrenheit_to_celsius(32), 0)
        self.assertAlmostEqual(fahrenheit_to_celsius(212), 100)

    def test_roundtrip(self):
        self.assertAlmostEqual(fahrenheit_to_celsius(celsius_to_fahrenheit(37)), 37)


class TestPrime(unittest.TestCase):
    def test_small(self):
        self.assertFalse(is_prime(0))
        self.assertFalse(is_prime(1))
        self.assertTrue(is_prime(2))
        self.assertTrue(is_prime(13))
        self.assertFalse(is_prime(15))

    def test_negative(self):
        self.assertFalse(is_prime(-7))

    def test_primes_upto(self):
        self.assertEqual(primes_upto(20), [2, 3, 5, 7, 11, 13, 17, 19])


class TestFizzBuzz(unittest.TestCase):
    def test_sequence(self):
        self.assertEqual(
            fizzbuzz(15),
            ["1", "2", "Fizz", "4", "Buzz", "Fizz", "7", "8",
             "Fizz", "Buzz", "11", "Fizz", "13", "14", "FizzBuzz"],
        )


if __name__ == "__main__":
    unittest.main(verbosity=2)
