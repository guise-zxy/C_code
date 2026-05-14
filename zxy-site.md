# 腾讯 HR / AI-HR 个人能力证据站

一个面向腾讯 HR / AI-HR 岗位投递场景设计的长期维护型个人网站。它不是普通技术作品集，而是一个“个人能力证据站”。

## 项目介绍

这个站点的目标不是展示“我会哪些技术”，而是帮助面试官更快判断：

- 我有技术背景，也理解人和组织问题
- 我不是泛泛投递 HR，而是在持续探索 AI-HR
- 我具备自驱、推进、协作、成就他人和持续复盘能力

## 技术栈

- Next.js 16
- TypeScript
- App Router
- Tailwind CSS 4
- MDX 内容管理
- Vercel Analytics
- Vercel 部署

## 页面结构

- `/` 首页
- `/tencent-hr` 腾讯 HR 岗位匹配页
- `/projects` 项目总览页
- `/projects/[slug]` 项目详情页
- `/thinking` 思考文章列表页
- `/thinking/[slug]` 文章详情页
- `/proof` 证明材料页
- `/about` 关于我页

## 本地启动方式

安装依赖：

```bash
pnpm install
```

启动开发环境：

```bash
pnpm dev
```

生产构建：

```bash
pnpm build
pnpm start
```

代码检查：

```bash
pnpm lint
```

## Vercel 部署准备

项目已经补齐以下部署准备项：

- `vercel.json`
- `robots.txt`
- `sitemap.xml`
- `NEXT_PUBLIC_SITE_URL` 环境变量支持
- 本地 `vercel` CLI 依赖

首次新建 Vercel 项目时建议：

```bash
pnpm vercel:preview
```

部署到正式环境时：

```bash
pnpm vercel:prod
```

部署后请在 Vercel 项目环境变量中设置：

```txt
NEXT_PUBLIC_SITE_URL=https://你的项目域名.vercel.app
```

## 内容维护方式

项目采用 `/data + /content` 混合管理。

- `src/data`
  - 放站点配置、导航、能力模型、项目摘要、证明材料和个人信息
- `src/content/projects`
  - 放项目详情正文 MDX
- `src/content/thinking`
  - 放思考文章正文 MDX

### 新增一个项目

1. 在 `src/data/projects.ts` 中新增项目摘要。
2. 在 `src/content/projects/` 中新增同名 `.mdx` 文件。
3. 在 `public/images/projects/` 中放入封面图。
4. 如有证明材料，在 `src/data/proofs.ts` 中新增记录。

### 新增一篇文章

1. 在 `src/content/thinking/` 中新增 `.mdx` 文件。
2. 填写 frontmatter：
   - `title`
   - `summary`
   - `category`
   - `date`
   - `readTime`
   - `featured`

## 图片替换方式

- 项目封面图：`public/images/projects/`
- 证明材料图：`public/images/proof/`

当前仓库中的 SVG 多为首版占位素材，后续可直接用真实截图、照片或导出图替换，路径保持一致即可。

## 简历 PDF 替换方式

当前简历文件路径：

```txt
public/resume/resume.pdf
```

直接用你的真实 PDF 覆盖即可，全站“下载简历”按钮会自动继续使用它。

## 部署方式

推荐部署到 Vercel：

1. 将仓库推送到 GitHub
2. 在 Vercel 中导入该仓库
3. Framework Preset 选择 Next.js
4. 使用默认构建命令即可完成部署

## 目录说明

```txt
src/
  app/               页面路由
  components/        基础组件、卡片组件、首页区块组件
  content/           项目与文章 MDX 内容
  data/              结构化数据
  lib/               内容读取、SEO、工具函数、类型

public/
  images/            项目图与证明材料图
  resume/            简历 PDF
```

## 后续维护说明

- 每周更新 1 条项目进展或短思考
- 每月整理 1 篇完整的 HR / AI / 项目复盘文章
- 每次投递前，根据岗位调整首页项目顺序和强调能力
- 每完成一个项目节点，补充截图、文档、数据和复盘
