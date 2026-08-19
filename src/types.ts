/** 归一化后的文章条目，三个源抓到的内容都转成这个形状 */
export interface Article {
  title: string;
  url: string;
  publishedAt: Date;
  /** 来源名，取自 sources.ts 中的配置 */
  source: string;
  /** 讨论页链接（仅 Hacker News 条目有） */
  discussionUrl?: string;
}

export interface SourceConfig {
  name: string;
  url: string;
}
