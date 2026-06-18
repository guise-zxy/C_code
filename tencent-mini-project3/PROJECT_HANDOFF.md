# 腾讯 mini 项目三交接文档：性能工程 Skill 评估专项

> 交接目标：把当前 ChatGPT 侧已经厘清的项目理解、硬性要求、导师补充说明、原始材料、推荐交付结构、Codex 开发路线一次性迁移到 Codex / VS Code / 桌面 App 中继续推进。

---

## 0. 项目一句话定位

本项目不是做一个普通 AI 应用，也不是单纯做一个漂亮 Demo，而是：

> **基于新版 `db_report_skill`，设计一套高质量 Skill 评估资产包：用理想态规范定义“好输出”，用 Rubrics 定义评分规则，用 Testcases 系统检验 Skill 表现，用 Meta-Testcase 检查测试用例本身质量，最终打包成 zip 提交。**

Demo / Python 脚本可以做，但定位是**附加加分项**：用于证明评估资产可校验、可复现、可自动生成测试报告。

---

## 1. 当前原材料清单

本交接包已经附带原始材料，路径如下：

```text
materials_raw/
├── 项目三.pdf
├── 2026深技大腾讯mini项目考题.pdf
└── db_report_skill(1).zip

materials_extracted/
└── db_report_skill/
    ├── SKILL.md
    ├── references/
    ├── scripts/
    └── templates/
```

其中：

1. `项目三.pdf`：项目三背景说明。包含项目目的、意义、用户群体、先导知识、testcase/rubric/meta-testcase 的基本要求、组织要求和验收标准。
2. `2026深技大腾讯mini项目考题.pdf`：正式考题汇总文档。项目三页面明确写出考题目标与 a/b/c/d 四项要求。
3. `db_report_skill(1).zip`：导师重新更新后的主 Skill 包。后续一律以这个版本为准，不要用旧包。
4. `materials_extracted/db_report_skill/`：为了方便 Codex 直接阅读，已经从 zip 解压并清理 `.git` 和 `__pycache__` 后放在这里。

---

## 2. 已确认的硬性要求

### 2.1 正式考题要求

项目三题目是：

> **为目标 Skill 构建评估用例集。**

目标是：

> 给定目标 Skill，为其构建一套完整的评估用例集，使该用例集能够系统性地检验 Skill 的质量。

要求拆解为：

1. 熟悉该 Skill，并定义该 Skill 的理想态规范；
2. 设计评分规则 Rubrics，定义评分维度及各维度权重；
3. 编写测试用例集：每个 Skill 不少于 5 条用例，不设上限，鼓励用更少用例覆盖更多场景，输出测试报告；
4. 设计编写 Meta-Testcase 自检方案。

### 2.2 交付格式要求

导师在微信答疑中明确说明：最终不是把所有内容塞进一个 YAML，而是每个模块单独成文档，然后整体 zip 打包。

推荐格式：

| 模块 | 推荐格式 | 是否核心 |
|---|---|---|
| 理想态规范 | `ideal_state.md` 或 `ideal_state.yaml` | 核心 |
| Rubrics | `rubrics.yaml` | 核心 |
| 测试用例 | `testcases.yaml` | 核心 |
| Meta-Testcase 自检方案 | `meta_testcase.md` 或 `meta_testcase.html` | 核心 |
| 测试报告 | `test_report.md` / `test_report.html` | 核心/强建议 |
| Python 校验脚本 | `.py` | 附加加分 |
| HTML 可视化报告 | `.html` | 附加加分 |
| 其他想法 | `appendix/` | 可附加，导师说会看 |

最终提交建议：

```text
姓名-意向方向-项目三.zip
├── ideal_state.md
├── rubrics.yaml
├── testcases.yaml
├── meta_testcase.md
├── test_report.md
├── README.md
├── tools/                  # 可选加分：校验/报告生成脚本
├── sample_skill_outputs/    # 可选：示例 Skill 输出
└── appendix/                # 可选：额外设计说明
```

---

## 3. 导师微信答疑信息的结论

### 3.1 关于 YAML 与 MD

导师原意：

- 理想态可以是 MD，也可以是 YAML；
- Rubrics 应该是 YAML；
- 用例最好是 YAML；
- Meta-Testcase 设计可以是 MD，也可以是 HTML 展示；
- 整体打包为 zip。

结论：**核心结构应是“YAML + MD + zip”，不是单文件 YAML。**

### 3.2 关于 references 文件数量

有人只看到 2 个 references 文件，是解压问题。导师确认应有 6 个规范文件。

当前新版包中确实有 6 个：

```text
references/
├── 分析方法论.md
├── 报告类型模板规范.md
├── 最小交付标准.md
├── 三格式输出规范.md
├── 数据源接入规范.md
└── 意图解析规范.md
```

结论：**必须围绕这 6 个规范设计理想态、Rubrics 和 Testcases。**

### 3.3 关于乱码

`分析方法论.md` 有同学打开乱码，但导师本地不乱码。当前材料中该文件可正常读取。若本地乱码，优先用：

- Bandizip / 7-Zip 重新解压；
- VS Code 以 UTF-8 打开。

### 3.4 关于新版 Skill 包

导师反馈旧版 `db_report_skill` 存在内部副文件引用没有合并的问题，已重新打包了新的 Skill 文件。

结论：**后续一律基于 `db_report_skill(1).zip`。**

### 3.5 关于附加产物

导师允许把超出考题范围的附加产物随主交付一起提交。

结论：**可以附加 Python 自动校验脚本、HTML 报告、覆盖矩阵、评测系统 Demo，但主交付物必须先完成。**

---

## 4. 被测 Skill：db_report_skill 摘要

### 4.1 Skill 名称与功能

`SKILL.md` 中给出的 Skill 信息：

```yaml
name: db-report-skill
description: 数据库性能测试报告生成技能。
```

核心能力：

> 根据本地性能测试数据或粘贴 JSON 数据，生成数据库性能测试报告。

支持输入：

- `.log`
- `.xlsx`
- `.json`
- `.csv`
- 用户直接粘贴 JSON 性能数据

不支持：

- 依赖外部服务；
- 调用外部 API；
- 没有数据时凭空生成报告；
- AI 推断、估算、硬编码性能数据。

支持 4 种报告类型：

| report_type | 中文类型 | 触发关键词 | 基本要求 |
|---|---|---|---|
| `single` | 单次测试报告 | 默认，无特殊关键词 | 1 份测试数据 + 场景指标分析 |
| `comparison` | 性能对比报告 | 对比、比较、vs、差异、哪个更好 | 至少 2 份测试数据横向对比 |
| `iteration` | 项目迭代报告 | 迭代、版本演进、历史趋势、变化趋势 | 至少 3 个版本/时间点纵向趋势 |
| `custom` | 客制化/专项报告 | 客制化、专项、定制、深度分析 | 按用户指定维度深度分析 |

输出要求：

```text
report.md + report.docx + report.html
```

且三格式需要独立生成，不允许只生成一个格式再批量转换。

### 4.2 Skill 五阶段流程

```text
阶段1：输入解析
  -> 识别数据源类型 + report_type + dimension_filters

阶段2：数据接入
  -> local_file / local_data / keyword_only
  -> 统一转成 StandardRecords

阶段3：分析
  -> 按 report_type 分流
  -> single / comparison / iteration / custom

阶段4：图表生成
  -> 按报告类型生成对应图表组合

阶段5：报告组装
  -> 输出 md/docx/html 三格式
```

---

## 5. 6 个 references 的作用

### 5.1 `意图解析规范.md`

用于约束阶段 1：自然语言输入如何变成 `intent.json`。

重点：

- `data_source_type`：`local_file | local_data | keyword_only`
- `report_type`：`single | comparison | iteration | custom`
- 中文场景关键词到英文关键词映射；
- AND / OR 逻辑区分；
- `test_name_keywords` 与 `test_name_keywords_or` 互斥。

评测关注：

- 能不能识别“对比/迭代/客制化”等意图；
- 能不能把“只读、点查、读写、更新索引”等中文术语映射正确；
- 能不能正确处理“和/或/以及”导致的 OR 逻辑。

### 5.2 `数据源接入规范.md`

用于约束阶段 2：各种输入转为统一的 `StandardRecords`。

重点：

- 支持 `.log/.xlsx/.json/.csv`；
- 支持粘贴 JSON；
- `keyword_only` 没有数据时应提示用户提供文件，不能编造数据；
- 质量门控：records 非空，TPS/QPS/P95 空值率 0%，并发档完整。

评测关注：

- 是否识别不存在文件；
- 是否拒绝无数据生成；
- 是否保持原始数据真实性；
- 是否输出统一字段结构。

### 5.3 `分析方法论.md`

用于约束阶段 3：按 4 类 `report_type` 分流分析。

重点：

- 5 个标准 sysbench 场景：
  - `oltp_point_select`
  - `oltp_read_only`
  - `oltp_write_only`
  - `oltp_read_write`
  - `oltp_update_index`
- 洞察分级：
  - L1：Python 计算的客观事实；
  - L2：跨维度统计发现；
  - L3：数据模式推测，只能出现在第 5 章选型与调优建议，且要标注 `[待确认]`。
- comparison 必须做公平性核查；
- iteration 必须识别性能回归；
- custom 必须围绕用户指定维度展开。

评测关注：

- 是否按 report_type 走正确分析逻辑；
- 是否按场景独立分析，禁止合并均值；
- 是否正确标注 L1/L2/L3；
- 是否存在无依据推测。

### 5.4 `报告类型模板规范.md`

用于约束阶段 4/5：不同报告类型的章节结构、图表组合、必需内容。

重点：

| report_type | 章节重点 | 图表基线 |
|---|---|---|
| single | 测试结论、环境、数据、性能分析、优化建议、附录 | ≥ 11 张 |
| comparison | 横向对比、适用场景矩阵、选型建议、公平性说明 | ≥ 17 + N 张 |
| iteration | 演进结论、版本趋势、累计变化、回归点 | ≥ 12 张 |
| custom | 分析目标、数据范围、深度分析、不确定性披露 | ≥ 3 张 |

评测关注：

- 章节是否完整；
- 图表数量和命名是否符合要求；
- 是否缺失公平性、回归点、不确定性等关键章节。

### 5.5 `三格式输出规范.md`

用于约束输出格式。

重点：

输出顺序强制为：

```text
1. report.md
2. report.docx
3. report.html
```

约束：

- Markdown 必须符合 GFM；
- docx 全文微软雅黑、表头浅蓝、图片居中；
- html 需要完整 head、内联 CSS、TOC、中文字体 fallback；
- 不允许 pandoc 直接转换。

评测关注：

- 是否三格式都生成；
- 是否只是简单转换；
- 格式是否合规；
- 图表路径是否相对。

### 5.6 `最小交付标准.md`

用于约束最终交付前的质量门控。

A~F 六类检查：

| 类别 | 含义 |
|---|---|
| A | 文件完整性 |
| B | 数据完整性 |
| C | 图表质量 |
| D | 分析质量 |
| E | 排版质量 |
| F | 占位符与一致性 |

评测关注：

- 是否能按 A~F 自动或半自动校验；
- 任一失败是否停止流程并给出修复建议；
- 是否能防止“看似生成报告但内容不达标”。

---

## 6. 当前推荐交付方案

### 6.1 主交付物

```text
submission/
├── README.md
├── ideal_state.md
├── rubrics.yaml
├── testcases.yaml
├── meta_testcase.md
└── test_report.md
```

### 6.2 加分交付物

```text
submission/
├── tools/
│   ├── validate_assets.py
│   ├── run_meta_check.py
│   ├── score_mock_outputs.py
│   └── generate_report.py
├── sample_skill_outputs/
│   ├── TC001_single_normal.md
│   ├── TC002_comparison_fair.md
│   └── ...
├── reports/
│   ├── eval_results.json
│   └── test_report.html
└── appendix/
    ├── coverage_matrix.md
    └── design_notes.md
```

不要优先做完整 Web 平台。理由：正式要求重心是评估资产，不是产品化平台。

---

## 7. 理想态规范建议

`ideal_state.md` 应围绕 `db_report_skill` 写，而不是泛泛写“AI 回答好”。建议定义为：

> 一个理想的 `db_report_skill` 应在不依赖外部服务、不编造数据的前提下，准确识别输入类型和报告意图，将本地/粘贴性能数据转换为标准结构，按 report_type 执行正确分析逻辑，生成符合模板、图表、洞察分级和三格式规范的数据库性能测试报告，并在数据不足、不公平或输入异常时明确提示风险而不是生成虚假结论。

理想态可拆成 8 条：

1. 输入识别正确；
2. 意图解析正确；
3. 数据源处理真实可靠；
4. 分析逻辑按 report_type 分流；
5. 指标计算和场景分析准确；
6. 报告结构与图表符合规范；
7. md/docx/html 三格式输出合规；
8. 异常输入、数据不足、公平性风险能被识别并提示。

---

## 8. Rubrics 初版设计方向

建议 `rubrics.yaml` 使用 100 分制。

| 维度 | 权重 | 依据 |
|---|---:|---|
| 输入识别与意图解析 | 15 | `意图解析规范.md` |
| 数据源接入与数据真实性 | 15 | `数据源接入规范.md` |
| 报告类型分流与分析方法 | 20 | `分析方法论.md` |
| 报告结构、章节与图表完整性 | 15 | `报告类型模板规范.md` |
| 三格式输出与排版合规 | 15 | `三格式输出规范.md` |
| 最小交付门控与鲁棒性 | 20 | `最小交付标准.md` |
| **合计** | **100** | |

每个维度需要定义：

```yaml
- id: R1
  name: 输入识别与意图解析
  weight: 15
  source_references:
    - references/意图解析规范.md
  levels:
    excellent: "..."
    pass: "..."
    fail: "..."
  hard_fail:
    - "report_type 识别错误导致后续报告类型完全错误"
```

---

## 9. Testcase 覆盖策略

正式要求每个 Skill 不少于 5 条。建议做到 8~10 条，显得更有覆盖意识，但不要过多导致写不完。

推荐 8 条：

| ID | 用例主题 | 覆盖点 | 类型 |
|---|---|---|---|
| TC001 | 单次报告正常生成 | local_file + single + 5 场景 + 三格式 | 正常流 |
| TC002 | 公平条件下性能对比报告 | comparison + 2 产品 + 公平性通过 | 正常流 |
| TC003 | 不公平数据对比 | comparison + 并发档/数据集不一致 + 风险提示 | 边界/风险 |
| TC004 | 版本迭代与性能回归识别 | iteration + ≥3 版本 + 回归点 | 正常/分析 |
| TC005 | 客制化高并发专项分析 | custom + focus_dimension=高并发 | 正常/专项 |
| TC006 | 无数据 keyword_only 输入 | keyword_only + 禁止编造数据 + 提示补充文件 | 异常流 |
| TC007 | 粘贴 JSON 数据生成报告 | local_data + 标准 records | 输入多样性 |
| TC008 | 中文场景 AND/OR 解析 | test_name_keywords vs test_name_keywords_or | 易错点 |

每条 testcase 至少包含：

```yaml
- id:
  name:
  purpose:
  priority:
  report_type:
  related_references:
  preconditions:
  input:
  expected_behavior:
  expected_outputs:
  evaluation_points:
  pass_criteria:
  fail_criteria:
  tags:
```

最关键的是 `pass_criteria` 和 `fail_criteria`，不能只写“回答合理即可”。

---

## 10. Meta-Testcase 自检方案

Meta-Testcase 不是测 Skill，而是测 testcase 写得好不好。

建议 `meta_testcase.md` 定义 10 条自检规则：

| ID | 自检项 | 目的 |
|---|---|---|
| MTC001 | 字段完整性 | 每条 testcase 是否字段齐全 |
| MTC002 | Reference 绑定 | 是否绑定至少 1 个规范文件 |
| MTC003 | 输入明确性 | 输入是否可执行，不是泛泛描述 |
| MTC004 | 预期输出明确性 | expected 是否具体可判定 |
| MTC005 | 判定规则可执行性 | pass/fail 是否能人工或脚本判定 |
| MTC006 | 覆盖矩阵完整性 | 是否覆盖 4 类 report_type 和关键 references |
| MTC007 | 异常/边界覆盖 | 是否包含 keyword_only、数据不公平、AND/OR 等易错点 |
| MTC008 | 反幻觉约束 | 是否检查禁止编造数据 |
| MTC009 | 区分度 | 是否能区分高质量输出和泛泛输出 |
| MTC010 | 可复现性 | 同一输入是否能重复执行评估 |

Meta-Testcase 输出建议：

```text
case_quality_score = 结构完整度 + 覆盖度 + 判定明确性 + 可复现性 + 区分度
目标分数：>= 80
```

---

## 11. 代码是否需要写？

需要，但不必重。

正确方向：

> **写轻量 Python 工具，把 YAML 资产校验、Meta-Testcase 自检、测试报告生成做出来。**

不建议做：

- 完整 Web 平台；
- 登录系统；
- 数据库；
- 前后端分离；
- 多用户管理。

建议做：

```text
tools/validate_assets.py
  校验 rubrics.yaml / testcases.yaml 字段完整性。

tools/run_meta_check.py
  按 MTC001~MTC010 检查 testcase 质量。

tools/score_mock_outputs.py
  读取 sample_skill_outputs，按 rubrics 生成模拟评分。

tools/generate_report.py
  输出 test_report.md / test_report.html。
```

命令行示例：

```bash
python tools/validate_assets.py assets/rubrics.yaml assets/testcases.yaml
python tools/run_meta_check.py assets/testcases.yaml assets/meta_rules.yaml
python tools/generate_report.py reports/eval_results.json
```

---

## 12. Codex 继续开发的推荐任务顺序

### 阶段 1：读材料与生成资产骨架

1. 阅读 `materials_extracted/db_report_skill/SKILL.md`；
2. 阅读 6 个 references；
3. 生成 `ideal_state.md`；
4. 生成 `rubrics.yaml` 初版；
5. 生成 `testcases.yaml` 初版 8 条；
6. 生成 `meta_testcase.md` 初版。

### 阶段 2：资产质量打磨

1. 用 coverage matrix 检查 6 个 references 是否都被覆盖；
2. 检查 4 类 report_type 是否都覆盖；
3. 检查是否有异常/边界用例；
4. 检查所有 pass/fail criteria 是否明确；
5. 检查所有权重加总是否为 100。

### 阶段 3：轻量工程化

1. 写 `validate_assets.py`；
2. 写 `run_meta_check.py`；
3. 写 `generate_report.py`；
4. 跑出 `test_report.md` 和 `test_report.html`。

### 阶段 4：最终打包

1. 主交付文件放根目录；
2. 附加脚本放 `tools/`；
3. 原材料是否放入最终提交，根据要求决定。若最终平台只收方案资产，不一定要附所有原材料；但本地交接包必须保留；
4. 文件名按要求：`姓名-意向方向-项目三.pdf` 或 `.md` / `.zip`。正式打包时再按腾讯文档要求确认。

---

## 13. 最容易踩坑的地方

1. **把 testcase 当成 Skill。** 纠正：Skill 是被测对象，testcase 是测试题。
2. **把 Meta-Testcase 写成测 Skill 的用例。** 纠正：Meta-Testcase 是检查 testcase 本身质量。
3. **只写泛泛的 AI 评分标准。** 纠正：必须绑定 6 个 references。
4. **只做 Demo，不写核心 YAML。** 纠正：Rubrics 和 testcases 才是核心交付。
5. **忽视 `keyword_only` 场景。** 这是反幻觉关键用例：没有数据就必须提示补充数据。
6. **没有覆盖 comparison 的公平性核查。** 这是性能报告生成中非常关键的质量点。
7. **没有覆盖 iteration 的回归检测。** 这是迭代报告的核心价值。
8. **Rubrics 没权重，或权重不等于 100。** 这会显得不工程化。
9. **判定规则写“合理即可”。** 这是无效 testcase。
10. **花太多时间做前端。** 当前时间紧，优先资产质量。

---

## 14. 推荐给 Codex 的第一条 Prompt

见本交接包中的：

```text
CODEX_START_PROMPT.md
```

建议在 Codex 中先打开整个交接包目录，然后把该 prompt 粘进去，让 Codex 先完成：

1. 阅读材料；
2. 生成 workspace 结构；
3. 生成核心资产骨架；
4. 再逐步补全内容。

---

## 15. 当前状态总结

已完成：

- 理解项目三不是做普通 Demo，而是做 Skill 评估资产包；
- 确认目标 Skill 是新版 `db_report_skill`；
- 确认最终提交是 zip，内部模块单独文档；
- 确认 Rubrics 用 YAML，用例最好 YAML；
- 确认 references 应有 6 个，并已在交接包中附带；
- 确认代码是加分项，不是主交付；
- 确认下一步开发应围绕资产工程化，而不是完整平台。

下一步：

> 在 Codex 中基于本交接包，先生成 `ideal_state.md`、`rubrics.yaml`、`testcases.yaml`、`meta_testcase.md` 四个核心文件，再考虑 Python 校验脚本和报告生成。
