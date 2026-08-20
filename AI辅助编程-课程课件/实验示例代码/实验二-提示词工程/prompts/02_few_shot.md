# 提示词 B · 少样本（Few-shot）

> 给出 1~2 个“输入→输出”示例，让模型模仿期望的行为与格式。

```text
请用 Python 实现学生排序函数 sort_students，规则见示例。

示例1：
输入：[{"name":"A","score":80}, {"name":"B","score":90}]
输出：[{"name":"B","score":90}, {"name":"A","score":80}]

示例2（分数相同时按姓名升序）：
输入：[{"name":"Charlie","score":88}, {"name":"Alice","score":95}, {"name":"Bob","score":88}]
输出：[{"name":"Alice","score":95}, {"name":"Bob","score":88}, {"name":"Charlie","score":88}]

现在请实现该函数，输入为任意学生列表，返回排序后的新列表。
```

## 效果
- 通过示例隐式传达了“分数相同按姓名升序”这一难以一句话讲清的规则。
- 输出格式更可控，更接近期望结果。
- 仍建议补充“不修改原列表 / 类型注解 / 边界处理”等约束（见 CoT 版本）。
