# -*- coding: utf-8 -*-
"""实验一 · 环境自检脚本（接入验证前的“体检”）。

它会检查：
  1) Python 版本是否满足要求（>= 3.10）；
  2) 是否已设置常见大模型 API Key 环境变量（只检查是否存在，不读取、不联网）；
  3) 关键标准库是否可用。

运行：
    python3 check_env.py
"""
import os
import sys
import platform

REQUIRED_PY = (3, 10)
# 仅检测“是否设置”，不会打印 Key 的内容
KEY_VARS = ["ANTHROPIC_API_KEY", "OPENAI_API_KEY"]


def check_python() -> bool:
    ok = sys.version_info >= REQUIRED_PY
    flag = "✔" if ok else "✗"
    print(f"[{flag}] Python 版本：{platform.python_version()}（要求 >= {REQUIRED_PY[0]}.{REQUIRED_PY[1]}）")
    return ok


def check_keys() -> bool:
    any_set = False
    for var in KEY_VARS:
        val = os.environ.get(var)
        if val:
            any_set = True
            print(f"[✔] 已检测到环境变量 {var}（长度 {len(val)}，内容已隐藏）")
        else:
            print(f"[·] 未设置环境变量 {var}")
    if not any_set:
        print("    提示：请按实验指导书用环境变量配置至少一个 API Key 后再做接入验证。")
    return any_set


def check_stdlib() -> bool:
    ok = True
    for mod in ["json", "sqlite3", "http.server", "urllib.request", "unittest"]:
        try:
            __import__(mod)
            print(f"[✔] 标准库可用：{mod}")
        except Exception as e:  # pragma: no cover
            ok = False
            print(f"[✗] 标准库不可用：{mod} -> {e}")
    return ok


def main() -> int:
    print("=" * 48)
    print(" AI 辅助编程 · 实验一 环境自检")
    print("=" * 48)
    py_ok = check_python()
    print("-" * 48)
    check_keys()
    print("-" * 48)
    lib_ok = check_stdlib()
    print("=" * 48)
    if py_ok and lib_ok:
        print("基础环境就绪。下一步：在工具中完成 API Key 接入验证。")
        return 0
    print("环境存在问题，请根据上面的提示修复后重试。")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
