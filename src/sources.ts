import type { SourceConfig } from './types.js';

/** RSS 源配置——新增源只需在这里加一行 */
export const SOURCES: SourceConfig[] = [
  { name: 'TechCrunch AI', url: 'https://techcrunch.com/category/artificial-intelligence/feed/' },
  { name: 'The Verge AI', url: 'https://www.theverge.com/rss/ai-artificial-intelligence/index.xml' },
  { name: 'Hacker News', url: 'https://hnrss.org/newest?q=AI&count=30' },
];
