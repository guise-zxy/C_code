# 实验一 · 示例代码：AI编程工具环境搭建与初体验

本目录是实验一的**可直接运行**示例成果，演示“环境自检 → 行内补全写函数 → 自然语言生成网页”。

## 文件清单

| 文件 | 说明 |
|------|------|
| `check_env.py` | 环境自检脚本：检查 Python 版本、API Key 环境变量、关键标准库 |
| `hello.py` | 由 AI 行内补全完成、并补充注释/文档字符串的示例函数集合 |
| `test_hello.py` | `hello.py` 的单元测试（标准库 `unittest`，无需安装依赖） |
| `index.html` | 用自然语言提示词生成的“基础 Web 页面骨架”示例 |

## 运行方法（无需安装任何第三方库）

```bash
# 1) 环境自检
python3 check_env.py

# 2) 运行示例函数，查看输出
python3 hello.py

# 3) 运行单元测试（应全部通过）
python3 -m unittest test_hello -v

# 4) 在浏览器中打开网页骨架
#    直接双击 index.html，或：
python3 -m http.server 8000   # 然后访问 http://localhost:8000/index.html
```

## 配置 API Key（接入验证用）

```bash
# macOS / Linux
export ANTHROPIC_API_KEY="你的_key"
# Windows PowerShell
$Env:ANTHROPIC_API_KEY="你的_key"
```

> ⚠ 切勿把 Key 写死在代码里或提交到仓库。`check_env.py` 只检查变量是否存在，不会读取其内容、也不会联网。

## 学习要点
- 体验三种 AI 交互：行内补全（写 `hello.py`）、对话生成（写 `index.html`）、命令执行（运行脚本）。
- 养成习惯：**AI 生成的每一段代码都要读懂、运行、测试后再使用。**
