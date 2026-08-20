# 实验三 · 待办事项管理系统（完整参考实现）

一个**前后端完整、可直接运行**的待办事项系统，演示“AI 辅助全栈小项目开发”的成果与架构。
**零第三方依赖**——仅使用 Python 标准库（`http.server` + `sqlite3`）。

## 功能
- 新增、删除待办；勾选标记完成 / 取消完成
- 按「全部 / 进行中 / 已完成」筛选
- 实时统计（总数 / 进行中 / 已完成）
- 一键清除已完成
- 输入校验（空标题、超长标题被拒绝）

## 目录结构
```
实验三-待办事项系统/
├── app.py                # HTTP 层：路由 + 静态文件 + JSON API
├── todo_service.py       # 业务逻辑层：TodoStore（sqlite3，与框架解耦）
├── static/
│   ├── index.html        # 前端页面
│   ├── style.css         # 样式
│   └── app.js            # 前端逻辑（fetch 调用 API）
├── test_todo_service.py  # 业务逻辑单元测试
├── test_api.py           # HTTP API 冒烟测试
└── README.md
```
> 架构要点：**业务逻辑与 Web 框架解耦**。`todo_service.py` 不依赖任何框架，
> 因此可独立做单元测试，也便于将来替换为 Flask / FastAPI。

## 运行
```bash
python3 app.py            # 启动后访问 http://localhost:8000
python3 app.py 8080       # 自定义端口
```

> 💡 若项目放在 **OneDrive / 网盘等同步目录**，运行时可能报 sqlite `disk I/O error`。
> 解决：把项目复制到本地磁盘运行，或指定本地数据库路径：
> ```bash
> TODO_DB=/tmp/todo.db python3 app.py      # macOS / Linux
> ```

## 测试
```bash
python3 -m unittest discover -v      # 运行全部测试（逻辑 + API）
# 或单独运行：
python3 -m unittest test_todo_service -v
python3 -m unittest test_api -v
```

## REST API
| 方法 | 路径 | 说明 |
|------|------|------|
| GET | `/api/todos?status=all\|active\|completed` | 列出待办 |
| POST | `/api/todos`  body `{"title":"..."}` | 新增（201） |
| PUT | `/api/todos/<id>` body `{"title"?,"done"?}` 或 `{"toggle":true}` | 更新 |
| DELETE | `/api/todos/<id>` | 删除（204） |
| GET | `/api/stats` | 统计 |

## 与 AI 协作的建议流程（呼应实验三任务）
1. 用 AI 完成需求分析与模块划分（参见实验三讲义）。
2. 让 AI 先生成项目骨架（目录与接口占位），确认设计。
3. 逐个接口生成 → 立即运行测试 → 再生成下一个。
4. 把报错与相关代码完整交给 AI 排查；联调跑通完整流程。
5. **每段生成代码都要读懂、测试后再合入。**
