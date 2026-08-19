import type { Article } from './types.js';

export interface ReportOptions {
  generatedAt: Date;
  /** 时间窗口（小时） */
  hours: number;
  /** 源名列表，用于固定统计行的顺序 */
  sourceNames: string[];
}

const p2 = (n: number) => String(n).padStart(2, '0');

const formatDate = (d: Date) => `${d.getFullYear()}-${p2(d.getMonth() + 1)}-${p2(d.getDate())}`;

const formatDateTime = (d: Date) => `${p2(d.getMonth() + 1)}-${p2(d.getDate())} ${p2(d.getHours())}:${p2(d.getMinutes())}`;

/** 标题中的方括号会破坏 Markdown 链接语法，转义掉 */
const escapeTitle = (title: string) => title.replace(/([[\]])/g, '\\$1');

export function renderReport(articles: Article[], options: ReportOptions): string {
  const counts = new Map<string, number>(options.sourceNames.map((name) => [name, 0]));
  for (const article of articles) {
    counts.set(article.source, (counts.get(article.source) ?? 0) + 1);
  }

  const stats = options.sourceNames.map((name) => `${name} ${counts.get(name) ?? 0}`).join(' · ');
  const timezone = Intl.DateTimeFormat().resolvedOptions().timeZone;
  const now = options.generatedAt;

  const lines: string[] = [
    `# AI 新闻日报 · ${formatDate(now)}`,
    '',
    `> 生成时间：${formatDate(now)} ${p2(now.getHours())}:${p2(now.getMinutes())}（${timezone}） · 时间窗口：最近 ${options.hours} 小时 · 共 ${articles.length} 篇`,
    `> 来源统计：${stats}`,
    '',
  ];

  if (articles.length === 0) {
    lines.push('时间窗口内没有抓到任何文章。');
  } else {
    for (const article of articles) {
      let line = `- \`${formatDateTime(article.publishedAt)}\` **${article.source}** — [${escapeTitle(article.title)}](${article.url})`;
      if (article.discussionUrl) {
        line += `｜[💬 讨论](${article.discussionUrl})`;
      }
      lines.push(line);
    }
  }

  return lines.join('\n') + '\n';
}
