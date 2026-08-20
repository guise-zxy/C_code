# 实验四 · 示例代码：AI辅助代码重构与测试生成

两部分内容：(1) 含坏味道的样例代码 → AI 辅助重构，并用测试证明“行为不变”；
(2) 为核心纯函数生成单元测试，覆盖正常 / 边界 / 异常。**零第三方依赖**。

## 目录结构
```
实验四-重构与测试/
├── before/order_discount.py   # 重构【前】：含魔法数字、重复代码、长方法
├── after/order_discount.py    # 重构【后】：表驱动 + 命名常量（行为一致）
├── test_order_discount.py     # 随机对拍 + 边界用例，证明重构前后行为一致
├── todo_logic.py              # 待办系统的核心纯函数
├── test_todo_logic.py         # 为核心函数生成的测试（正常/边界/异常）
└── README.md
```

## 运行测试
```bash
python3 -m unittest discover -v          # 运行全部测试
python3 -m unittest test_order_discount -v
python3 -m unittest test_todo_logic -v
```

## 任务一：识别坏味道并重构
1. 打开 `before/order_discount.py`，找出全部坏味道（魔法数字、重复代码、长方法、命名差）。
2. 让 AI 在“**保持行为不变**”的前提下重构（可参考 `after/order_discount.py`）。
3. 运行 `test_order_discount.py`：其中 `test_random_equivalence` 用 500 组随机输入
   对“重构前 `calc`”与“重构后 `total_amount`”做**对拍**，断言结果一致。
4. 体会：**没有测试护航的重构是危险的**。

## 任务二：为核心函数生成测试并补边界
1. 阅读 `todo_logic.py` 的纯函数。
2. 让 AI 为这些函数生成单元测试。
3. 对照 `test_todo_logic.py`，**手动补齐 AI 常遗漏的边界与异常**
   （如：恰好 200 字符、空列表、非字符串、非法 status）。

## 关于测试覆盖率（可选）
本项目测试用标准库 `unittest`，无需安装。若想查看覆盖率，可选装 `coverage`：
```bash
pip install coverage
coverage run -m unittest discover
coverage report -m          # 查看每个文件未覆盖的行号，针对性补测试
```

## 学习要点
- 重构三原则：**保持行为、小步前进、测试护航**。
- AI 重构必须**人工审查 + 测试验证**，警惕“好心办坏事”。
- AI 生成的测试常只覆盖主路径，**边界与异常需人工补充**。
