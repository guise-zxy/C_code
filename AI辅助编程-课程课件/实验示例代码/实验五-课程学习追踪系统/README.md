# 实验五 · 课程学习追踪系统（综合参考项目）

一个**前后端完整、可直接运行**的综合参考项目，作为实验五“综合项目设计、实现与展示”的范例。
**零第三方依赖**——仅使用 Python 标准库（`http.server` + `sqlite3`）。

## 功能
- 课程管理：添加 / 删除课程，设定目标学时
- 学习记录：为课程登记每次学习时长与备注
- 进度追踪：自动计算并以进度条展示完成度（学习小时 / 目标学时）
- 统计报表：课程数、累计学时、今日学时、已达标数
- 智能提醒：列出“尚未开始”或“多日未学习”的未达标课程

## 目录结构
```
实验五-课程学习追踪系统/
├── app.py                  # HTTP 层：路由 + 静态文件 + JSON API
├── tracker_service.py      # 业务逻辑层：TrackerStore（sqlite3）
├── static/
│   ├── index.html          # 前端页面
│   ├── style.css           # 样式
│   └── app.js              # 前端逻辑（fetch 调用 API）
├── test_tracker_service.py # 业务逻辑单元测试
├── test_api.py             # HTTP API 冒烟测试
├── docs/
│   ├── 架构说明.md          # 系统架构与数据模型
│   └── AI使用日志模板.md     # 综合项目交付物模板
└── README.md
```

## 运行
```bash
python3 app.py            # 启动后访问 http://localhost:8000
python3 app.py 8080       # 自定义端口
```
> 💡 若项目位于 **OneDrive / 网盘等同步目录** 运行时报 sqlite `disk I/O error`，
> 请复制到本地磁盘，或指定本地数据库：`TRACKER_DB=/tmp/tracker.db python3 app.py`

## 测试
```bash
python3 -m unittest discover -v     # 运行全部测试（逻辑 + API）
```

## 把它当作你综合项目的“脚手架”
本项目展示了一个**结构清晰、可测试、可扩展**的小型全栈系统。你可以：
- 直接以它为模板，替换为你自选的题目（如智能简历助手、代码题目生成器）；
- 参考其分层结构（HTTP 层 / 业务逻辑层 / 数据层）组织你的代码；
- 参考 `docs/架构说明.md` 撰写你的系统架构图与设计说明；
- 用 `docs/AI使用日志模板.md` 记录你的 AI 工具使用过程。

> 记住实验五的核心：**人是目标的定义者、质量的把关者与结果的负责人。**
