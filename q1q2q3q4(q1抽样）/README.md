# 2024 国赛 B 题赛前模拟：问题 1 与问题 2 基线

## 运行方式

在 `problem_b` 目录外或目录内均可运行：

```powershell
python problem_b/code/run_q1_q2.py
```

如果使用 Codex bundled Python：

```powershell
& 'C:/Users/LY/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/python.exe' 'C:/Users/LY/Desktop/2026数学建模/problem_b/code/run_q1_q2.py'
```

## 已实现内容

### 问题 1：固定样本精确二项分布

主方案：

```text
p0 = 0.10
p_bad = 0.15
n = 368
a = 29
r = 47
X <= 29: accept
30 <= X <= 46: inconclusive
X >= 47: reject
```

程序会独立搜索最小 `n,a,r`，并验证 `n=367` 时拒收功效不足 90%。

输出文件：

```text
results/q1/q1_fixed_summary.json
results/q1/q1_selected_probabilities.csv
results/q1/q1_probability_curve.csv
results/q1/q1_sensitivity_pbad.csv
```

### 问题 2：16 种静态策略枚举

策略变量：

```text
d1: 是否检测零配件1
d2: 是否检测零配件2
df: 是否检测成品
dr: 是否拆解不合格成品
```

当前口径：

```text
检测完全准确；
最终交付 1 件合格品为核算单位；
售价只算一次，换货不产生新收入；
换货仍计调换损失；
报废后重新购买并开始下一轮生产；
拆解回流零件保留质量后验状态；
已知合格回流零件不重复检测。
```

输出文件：

```text
results/q2/q2_static_all_strategies.csv
results/q2/q2_static_best_strategies.csv
results/q2/q2_static_summary.json
```

## 当前最优静态策略

| 情况 | d1 | d2 | df | dr | 期望成本 | 期望利润 |
|---:|---:|---:|---:|---:|---:|---:|
| 1 | 1 | 1 | 0 | 1 | 37.8888888889 | 18.1111111111 |
| 2 | 1 | 1 | 0 | 1 | 44.0000000000 | 12.0000000000 |
| 3 | 1 | 1 | 0 | 1 | 40.5555555556 | 15.4444444444 |
| 4 | 1 | 1 | 1 | 1 | 41.2500000000 | 14.7500000000 |
| 5 | 0 | 1 | 0 | 0 | 44.0123456790 | 11.9876543210 |
| 6 | 0 | 0 | 0 | 0 | 34.3213296399 | 21.6786703601 |

## 注意事项

`inconclusive` 在问题 1 固定样本基线中只作为未决结果输出。由于题目没有给批量规模和全检成本，当前不计算转全检费用，只保留后续扩展口径。

问题 2 当前是静态策略基线，不是后续的状态依赖动态最优策略。后续动态策略可以在这个状态转移框架上继续扩展。
