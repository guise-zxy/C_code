import Parser from 'rss-parser';
import type { Article, SourceConfig } from './types.js';

const REQUEST_TIMEOUT_MS = 15_000;
const USER_AGENT = 'ai-news-cli/0.1.0 (RSS aggregator; +https://github.com)';

/** rss-parser 默认不解析 <comments> 标签，用 customFields 带出来 */
interface CustomItem {
  comments?: string;
}

const parser = new Parser<CustomItem>({
  customFields: { item: ['comments'] },
});

const HTML_ENTITIES: Record<string, string> = {
  amp: '&',
  lt: '<',
  gt: '>',
  quot: '"',
  apos: "'",
  nbsp: ' ',
};

/** 剥离标题里的 HTML 标签并解码常见实体（The Verge 的 title 是 type="html"） */
function cleanTitle(raw: string): string {
  return raw
    .replace(/<[^>]*>/g, '')
    .replace(/&(#x?[0-9a-fA-F]+|[a-zA-Z]+);/g, (match, entity: string) => {
      if (/^#x/i.test(entity)) {
        const code = Number.parseInt(entity.slice(2), 16);
        return code >= 0 && code <= 0x10ffff ? String.fromCodePoint(code) : match;
      }
      if (entity.startsWith('#')) {
        const code = Number.parseInt(entity.slice(1), 10);
        return code >= 0 && code <= 0x10ffff ? String.fromCodePoint(code) : match;
      }
      return HTML_ENTITIES[entity.toLowerCase()] ?? match;
    })
    .replace(/\s+/g, ' ')
    .trim();
}

/** 抓取并解析单个源，归一化为 Article 列表。失败时直接抛错，由调用方决定如何降级。 */
export async function fetchFeed(source: SourceConfig): Promise<Article[]> {
  const response = await fetch(source.url, {
    headers: {
      'User-Agent': USER_AGENT,
      Accept: 'application/rss+xml, application/atom+xml, application/xml, text/xml, */*',
    },
    redirect: 'follow',
    signal: AbortSignal.timeout(REQUEST_TIMEOUT_MS),
  });
  if (!response.ok) {
    throw new Error(`HTTP ${response.status} ${response.statusText}`);
  }

  const xml = await response.text();
  const feed = await parser.parseString(xml);

  const articles: Article[] = [];
  let datelessCount = 0;
  for (const item of feed.items ?? []) {
    const title = item.title ? cleanTitle(item.title) : '';
    const url = item.link?.trim();
    if (!title || !url) continue;

    // 无发布时间的条目无法参与时间过滤和排序，丢弃并计数
    const date = item.isoDate ? new Date(item.isoDate) : undefined;
    if (!date || Number.isNaN(date.getTime())) {
      datelessCount++;
      continue;
    }

    // 只有 HN 的 <comments> 是讨论页；WordPress 的 <comments> 是评论 feed，不展示
    const discussionUrl = item.comments?.startsWith('https://news.ycombinator.com/')
      ? item.comments
      : undefined;

    articles.push({ title, url, publishedAt: date, source: source.name, discussionUrl });
  }
  if (datelessCount > 0) {
    console.warn(`  ⚠️ ${source.name}: ${datelessCount} 条无发布时间，已丢弃`);
  }
  return articles;
}
