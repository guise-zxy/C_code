import { mkdir, writeFile } from 'node:fs/promises';
import path from 'node:path';
import { SOURCES } from './sources.js';
import { fetchFeed } from './feed.js';
import { renderReport } from './report.js';
import type { Article } from './types.js';

interface CliOptions {
  hours: number;
  outDir: string;
}

function parseArgs(argv: string[]): CliOptions {
  const options: CliOptions = { hours: 24, outDir: 'output' };
  for (const arg of argv) {
    if (arg.startsWith('--hours=')) {
      const value = Number(arg.slice('--hours='.length));
      if (!Number.isFinite(value) || value <= 0) {
        throw new Error(`无效的 --hours 值：${arg}`);
      }
      options.hours = value;
    } else if (arg.startsWith('--out=')) {
      options.outDir = arg.slice('--out='.length);
    } else {
      throw new Error(`未知参数：${arg}\n用法：tsx src/index.ts [--hours=24] [--out=output]`);
    }
  }
  return options;
}

async function main(): Promise<void> {
  const options = parseArgs(process.argv.slice(2));
  const now = new Date();
  const cutoff = new Date(now.getTime() - options.hours * 3_600_000);

  console.log(`⏳ 正在抓取 ${SOURCES.length} 个源（时间窗口：最近 ${options.hours} 小时）…`);

  // 并发抓取，单源失败不影响其他源
  const results = await Promise.allSettled(SOURCES.map((source) => fetchFeed(source)));

  const articles: Article[] = [];
  let failedCount = 0;
  for (const [index, result] of results.entries()) {
    const source = SOURCES[index];
    if (result.status === 'fulfilled') {
      const fresh = result.value.filter((article) => article.publishedAt >= cutoff);
      console.log(`  ✅ ${source.name}：${result.value.length} 条，窗口内 ${fresh.length} 条`);
      articles.push(...fresh);
    } else {
      failedCount++;
      console.error(`  ⚠️ ${source.name}：抓取失败 — ${String(result.reason)}`);
    }
  }

  if (failedCount === SOURCES.length) {
    console.error('❌ 所有源都抓取失败。');
    process.exitCode = 1;
    return;
  }

  // 按 URL 去重后按时间倒序
  const seen = new Set<string>();
  const unique = articles.filter((article) => {
    if (seen.has(article.url)) return false;
    seen.add(article.url);
    return true;
  });
  unique.sort((a, b) => b.publishedAt.getTime() - a.publishedAt.getTime());

  const markdown = renderReport(unique, {
    generatedAt: now,
    hours: options.hours,
    sourceNames: SOURCES.map((source) => source.name),
  });

  await mkdir(options.outDir, { recursive: true });
  const date = `${now.getFullYear()}-${String(now.getMonth() + 1).padStart(2, '0')}-${String(now.getDate()).padStart(2, '0')}`;
  const outPath = path.join(options.outDir, `ai-daily-${date}.md`);
  await writeFile(outPath, markdown, 'utf8');

  console.log(`\n📄 共 ${unique.length} 篇，日报已生成：${outPath}`);
}

main().catch((error) => {
  console.error('❌ 运行出错:', error);
  process.exitCode = 1;
});
