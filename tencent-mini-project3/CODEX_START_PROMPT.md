# 给 Codex 的启动 Prompt

我正在完成“2026 深技大腾讯 mini 项目考题”的项目三：性能工程 Skill 评估专项。请你基于当前目录中的材料继续开发。

## 项目定位

我不是要做一个完整 Web 平台，而是要围绕目标 Skill：`db_report_skill`，构建一套高质量 Skill 评估资产包。

核心交付物包括：

1. `ideal_state.md`：定义 `db_report_skill` 的理想态规范；
2. `rubrics.yaml`：定义评分规则、评分维度、权重、等级标准、硬失败项；
3. `testcases.yaml`：编写不少于 5 条测试用例，建议 8 条，覆盖更多场景；
4. `meta_testcase.md`：设计 Meta-Testcase 自检方案，用来检查 testcase 本身质量；
5. `test_report.md`：输出测试报告；
6. 可选加分：Python 脚本自动校验 YAML、执行 Meta-Testcase 自检、生成 HTML 报告。

## 必读材料

请优先阅读：

```text
PROJECT_HANDOFF.md
materials_extracted/db_report_skill/SKILL.md
materials_extracted/db_report_skill/references/意图解析规范.md
materials_extracted/db_report_skill/references/数据源接入规范.md
materials_extracted/db_report_skill/references/分析方法论.md
materials_extracted/db_report_skill/references/报告类型模板规范.md
materials_extracted/db_report_skill/references/三格式输出规范.md
materials_extracted/db_report_skill/references/最小交付标准.md
```

`materials_raw/` 里保存的是原始 PDF 与新版 Skill zip，不要删除。

## 要做的事

请先完成以下任务：

1. 在 `workspace_seed/assets/` 下生成：
   - `ideal_state.md`
   - `rubrics.yaml`
   - `testcases.yaml`
   - `meta_testcase.md`
   - `README.md`

2. `rubrics.yaml` 使用 100 分制，至少包含以下维度：
   - 输入识别与意图解析；
   - 数据源接入与数据真实性；
   - 报告类型分流与分析方法；
   - 报告结构、章节与图表完整性；
   - 三格式输出与排版合规；
   - 最小交付门控与鲁棒性。

3. `testcases.yaml` 至少包含 8 条用例：
   - TC001：single 正常单次报告；
   - TC002：comparison 公平对比报告；
   - TC003：comparison 不公平数据对比，必须提示风险；
   - TC004：iteration 三版本迭代与回归检测；
   - TC005：custom 高并发专项分析；
   - TC006：keyword_only 无数据输入，禁止编造数据；
   - TC007：粘贴 JSON 数据生成报告；
   - TC008：中文场景 AND/OR 解析。

每条 testcase 至少包含：

```yaml
id:
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

4. `meta_testcase.md` 至少包含 10 条自检规则：字段完整性、reference 绑定、输入明确性、预期输出明确性、判定规则可执行性、覆盖矩阵完整性、异常/边界覆盖、反幻觉约束、区分度、可复现性。

5. 如果继续写代码，请只做轻量 Python 工具，不要做完整 Web 平台：

```text
workspace_seed/tools/validate_assets.py
workspace_seed/tools/run_meta_check.py
workspace_seed/tools/generate_report.py
```

## 输出原则

- 不要泛泛写“AI 回答质量”；必须绑定 `db_report_skill` 的 SKILL.md 和 6 个 references。
- 不要把 testcase 当成 Skill；Skill 是被测对象，testcase 是测试题。
- 不要把 Meta-Testcase 写成测试 Skill 的用例；Meta-Testcase 是检查 testcase 本身是否合格。
- 核心资产质量优先于界面和 Demo。
