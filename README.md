# 实习生成长导航 (WorkBuddy)

基于 React + TypeScript + Vite + Tailwind CSS + CloudBase 的实习生成长导航项目，覆盖实习生、Mentor、人力运营三个角色视角，并已发布到 CloudBase 静态托管。

## 公网访问

- **公网地址**: `https://mentor-d9gfw9bk85f65ce29-1387474988.tcloudbaseapp.com/`
- **登录页直达**: `https://mentor-d9gfw9bk85f65ce29-1387474988.tcloudbaseapp.com/#/login`
- **当前环境 ID**: `mentor-d9gfw9bk85f65ce29`
- **发布方式**: CloudBase 静态托管（目录 `dist/` 上传到站点根目录 `/`）

## 演示账号

> 当前 CloudBase 个人版环境的应用用户配额已满（现有 3 个用户：`intern01`、`mentor_li`、`administrator`），因此 **HR 演示账号暂使用现有 `administrator` 登录，并在前端映射为 `hr_01` 的业务视角**。

- **实习生**: `intern01`
- **Mentor**: `mentor_li`
- **人力运营（临时复用）**: `administrator`
- **统一演示密码**: `Demo2026!`

## 本次已完成内容

### 角色与页面

- **登录页**
  - 支持用户名密码登录
  - 支持按业务角色自动跳转
  - 补充了三类演示账号提示

- **实习生侧**
  - 仪表盘
  - 任务列表
  - 进展记录
  - 问题卡与问题详情

- **Mentor 侧**
  - 重构 Mentor 首页信息架构为 3 个区块：
    - 顶部概览
    - 需要关注的问题
    - 实习生状态
  - 删除“问题协助”页顶部的“需联系”筛选按钮

- **HR 侧**
  - 实现 `/hr/dashboard`
  - 页面回答三个核心问题：
    - 这一批实习生整体推进是否正常
    - 哪些异常需要人力运营进一步协同
    - 哪些问题反映了流程或知识库缺口
  - 完成 4 个模块：
    - 批次概览
    - 异常关注清单
    - 适岗观察证据摘要
    - 高频问题与知识库缺口
  - 当 `issue_clusters` / `mentor_feedback` 集合无数据或无前端读取权限时，使用 Seed fallback 展示

### CloudBase 连接与部署

- CloudBase Web SDK 已接入
- 用户名密码登录已开启
- Publishable Key 已配置可用
- 项目已成功构建并部署到 CloudBase 静态托管
- 公网地址已验证可访问

### AI 调用边界

- 前端统一通过 `ai-gateway` 云函数调用 AI 能力
- 前端源码中**不保存**模型 API Key
- 云函数中已实现：
  - `summarizeProgress`
  - `recommendKnowledge`
  - `generateHelpSummary`
  - `clusterIssues`（当前保持 mock）
- 当云函数未配置外部模型环境变量，或模型调用异常时，会 fallback 到 mock 结果

## 当前环境资源清单

### 数据库集合

当前环境中存在以下集合：

- `anomaly_events`
- `issue_cards`
- `issue_clusters`
- `issue_events`
- `knowledge_items`
- `mentor_feedback`
- `progress_logs`
- `relation_data_depart`
- `sys_department`
- `sys_user`
- `tasks`
- `threshold_rules`
- `users`

### 云函数

- `ai-gateway`（`Nodejs18.15`，状态 `Active`）

## 当前权限状态（真实环境） 

以下为当前关键集合的实际权限配置：

- `users`: `CUSTOM`，规则 `{"read":"auth != null","write":false}`
- `tasks`: `CUSTOM`，规则 `{"read":"auth != null","write":false}`
- `progress_logs`: `CUSTOM`，规则 `{"read":"auth != null","write":"auth != null"}`
- `issue_cards`: `CUSTOM`，规则 `{"read":"auth != null","write":"auth != null"}`
- `issue_events`: `CUSTOM`，规则 `{"read":"auth != null","write":"auth != null"}`
- `knowledge_items`: `CUSTOM`，规则 `{"read":"auth != null","write":false}`
- `anomaly_events`: `CUSTOM`，规则 `{"read":"auth != null","write":false}`
- `mentor_feedback`: `PRIVATE`
- `issue_clusters`: `PRIVATE`
- `threshold_rules`: `READONLY`

## 环境变量说明

### 前端 `.env.local`

在项目根目录创建 `.env.local`：

```env
VITE_ENV_ID=mentor-d9gfw9bk85f65ce29
VITE_PUBLISHABLE_KEY=<你的 Publishable Key>
```

### 云函数 `ai-gateway` 环境变量

云函数需要以下环境变量，且**只能配置在云函数侧**：

```env
LLM_API_KEY=<你的模型 API Key>
LLM_BASE_URL=<你的模型 API 基础地址>
LLM_MODEL=<可选，默认 deepseek-v3.2>
```

说明：

- 若 `LLM_API_KEY` 或 `LLM_BASE_URL` 缺失，云函数自动回退到 mock 逻辑
- 前端不会暴露任何模型密钥

## 本地启动方式

### 1. 安装依赖

```bash
npm install
```

### 2. 启动开发服务器

```bash
npm run dev
```

默认本地地址：`http://127.0.0.1:5173`

### 3. 构建生产版本

```bash
npm run build
```

构建产物输出到 `dist/`。

## 云端部署方式

### 方式一：使用当前项目内配置发布到 CloudBase 静态托管

当前 `cloudbaserc.json` 已配置：

- `envId`: `mentor-d9gfw9bk85f65ce29`
- `buildCommand`: `npm run build`
- `outputDir`: `dist`
- `deployPath`: `/`

发布流程：

1. 运行 `npm install`
2. 运行 `npm run build`
3. 将 `dist/` 上传到 CloudBase 静态托管根目录

### 方式二：在 CloudBase 控制台手动上传

1. 打开 CloudBase 控制台静态托管
2. 选择环境 `mentor-d9gfw9bk85f65ce29`
3. 上传 `dist/` 目录内容到站点根目录

## 本次验证结果

### 已验证通过

- **CloudBase 环境信息**: 已确认环境存在且状态正常
- **认证方式**: 用户名密码登录已开启
- **Publishable Key**: 已存在
- **`npm install`**: 已通过
- **`npm run build`**: 已通过
- **`read_lints`**: 本次改动相关文件无新增问题
- **静态托管**: 上传成功
- **公网可访问**: 已通过抓取首页 HTML 验证
- **最新资源已生效**: 首页引用了最新哈希资源：
  - `index-CvGF32G2.js`
  - `index-CqZgEuQX.css`

### 代码层已确认

- 根路径未登录会跳转 `/login`
- 路由守卫会拦截未授权访问
- 不同角色登录后会跳转到各自 dashboard
- AI 调用走云函数，不在前端暴露密钥

## 暂未完全实现 / 仍需继续优化

以下事项当前为**已识别但尚未彻底完成**：

- **严格的数据权限边界仍未完全收口**
  - 当前多数集合的规则仍是 `auth != null` 级别
  - 这意味着“实习生只能看自己、Mentor 只能看自己带教对象、HR 只看必要摘要”的服务端强约束还没有完全落到安全规则里

- **HR 独立账号受环境配额限制**
  - 由于个人版应用用户配额已满，无法新建 `hr_ops`
  - 当前先用 `administrator` 作为 HR 演示入口

- **`issue_clusters` 与 `mentor_feedback` 仍以 fallback 为主**
  - 当前环境里这两个集合要么没有数据，要么前端无读取权限
  - HR 首页已做兼容，不影响展示，但不是最终态

- **运行态登录 / 写入链路未做自动化浏览器实测**
  - 当前完成了代码检查、构建验证、静态托管验证和公网可访问验证
  - 但“真实浏览器中逐角色登录并提交写入数据”的验收，还建议继续补一轮人工回归或自动化浏览器测试

- **前端产物体积偏大**
  - 当前主 JS chunk 约 `1,050 kB`
  - 构建可通过，但后续建议做代码分包

## 下一步建议

1. **升级环境套餐或释放一个用户配额**，创建独立的 `hr_ops` 账号
2. **重构集合安全规则**，把权限从“登录即可读写”收紧到“按角色和所属对象访问”
3. **给 `mentor_feedback` / `issue_clusters` 补齐真实数据写入链路**，减少 HR 页面 fallback 依赖
4. **增加浏览器自动化验收**，覆盖登录、角色跳转、写入与刷新持久化
5. **对前端做代码分包**，降低主 bundle 体积

## 目录结构

```text
├── cloudfunctions/
│   └── ai-gateway/
├── scripts/
├── src/
│   ├── components/
│   ├── contexts/
│   ├── pages/
│   │   ├── intern/
│   │   ├── mentor/
│   │   └── hr/
│   ├── services/
│   ├── types/
│   ├── utils/
│   ├── App.tsx
│   └── main.tsx
├── dist/
├── cloudbaserc.json
├── package.json
└── README.md
```

## 交接文档

已补充完整交接包，请优先阅读：

- `docs/handoff/PROJECT_HANDOFF.md`
- `docs/handoff/LOCAL_SETUP.md`
- `docs/handoff/PROJECT_TREE.md`
- `docs/handoff/CLOUDBASE_RESOURCE_INVENTORY.md`
- `docs/handoff/DATA_EXPORT_GUIDE.md`
- `docs/handoff/SECURITY_CHECK.md`
- `docs/handoff/CODEX_ONBOARDING.md`
- 根目录 `AGENTS.md`

## 交接验证脚本

- `npm.cmd run typecheck`：只读 TypeScript 检查
- `npm.cmd run lint:check`：只读 ESLint 检查
- `npm.cmd run build`：生产构建检查
- `npm.cmd run verify`：串联执行上述关键检查

> 在 Windows PowerShell 下若遇到 `npm.ps1` 执行策略限制，请直接使用 `npm.cmd`。

## 备注

- 路由采用 `HashRouter`，适合静态托管直接发布
- 静态资源使用相对路径 `base: "./"`
- 登录链路采用 CloudBase Web SDK 的 `signInWithPassword`
- AI 密钥仅允许放在云函数环境变量中，不应写进前端仓库
- 当前工程目录尚未初始化 Git 仓库；若要正式交接，建议先在本地执行 `git init` 后再提交交接基线
