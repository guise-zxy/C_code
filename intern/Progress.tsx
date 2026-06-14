import { useEffect, useState } from "react";
import { useAuth } from "../../contexts/AuthContext";
import { app } from "../../utils/cloudbase";
import { summarizeProgress } from "../../services/aiService";
import type { ProgressLog } from "../../types";

export default function InternProgress() {
  const { user } = useAuth();
  const [logs, setLogs] = useState<ProgressLog[]>([]);
  const [loading, setLoading] = useState(true);
  const [inputText, setInputText] = useState("");
  const [submitting, setSubmitting] = useState(false);
  const [aiResult, setAiResult] = useState<ProgressLog | null>(null);

  useEffect(() => {
    if (!user) return;
    const db = app.database();
    db.collection("progress_logs")
      .where({ internId: user.id })
      .orderBy("createdAt", "desc")
      .limit(30)
      .get()
      .then((res) => setLogs((res.data as unknown as ProgressLog[]) || []))
      .catch(console.error)
      .finally(() => setLoading(false));
  }, [user]);

  const handleSubmit = async () => {
    if (!user || !inputText.trim()) return;
    setSubmitting(true);
    setAiResult(null);

    try {
      // Call AI to summarize
      const aiData = await summarizeProgress(inputText.trim());

      const now = new Date().toISOString();
      const log: Omit<ProgressLog, "id"> = {
        internId: user.id,
        rawText: inputText.trim(),
        completedItems: aiData?.completedItems || [],
        blockers: aiData?.blockers || [],
        attemptedActions: aiData?.attemptedActions || [],
        impactsMainTask: aiData?.impactsMainTask || false,
        nextSuggestion: aiData?.nextSuggestion || "",
        aiSummary: aiData
          ? `完成: ${aiData.completedItems.join("、")}${aiData.blockers.length ? ` | 卡点: ${aiData.blockers.join("、")}` : ""} | 建议: ${aiData.nextSuggestion}`
          : undefined,
        createdAt: now,
      };

      // Save to database
      const db = app.database();
      await db.collection("progress_logs").add(log);

      const savedLog = { ...log, id: Date.now().toString() } as ProgressLog;
      setLogs([savedLog, ...logs]);
      setAiResult(savedLog);
      setInputText("");
    } catch (err) {
      console.error("保存进展失败:", err);
    } finally {
      setSubmitting(false);
    }
  };

  return (
    <div className="space-y-6">
      <h1 className="text-xl font-bold text-gray-900">进展记录</h1>

      {/* Input area */}
      <div className="rounded-lg bg-white p-5 shadow-sm ring-1 ring-gray-200">
        <textarea
          value={inputText}
          onChange={(e) => setInputText(e.target.value)}
          placeholder="今天做了什么？遇到了什么问题？可以用自然语言描述..."
          rows={4}
          className="w-full resize-none rounded-md border border-gray-200 px-3 py-2 text-sm text-gray-800 placeholder-gray-400 focus:border-blue-400 focus:outline-none focus:ring-1 focus:ring-blue-400"
        />
        <div className="mt-3 flex items-center justify-between">
          <p className="text-xs text-gray-400">提交后 AI 会自动归纳关键信息</p>
          <button
            type="button"
            onClick={handleSubmit}
            disabled={submitting || !inputText.trim()}
            className="rounded-md bg-blue-600 px-4 py-2 text-sm font-medium text-white hover:bg-blue-700 disabled:opacity-50 disabled:cursor-not-allowed transition-colors"
          >
            {submitting ? "AI 归纳中..." : "提交进展"}
          </button>
        </div>
      </div>

      {/* AI result preview */}
      {aiResult && (
        <div className="rounded-lg border border-blue-200 bg-blue-50 p-4">
          <h3 className="text-sm font-semibold text-blue-800">AI 归纳结果</h3>
          <div className="mt-2 space-y-2 text-sm">
            {aiResult.completedItems.length > 0 && (
              <div>
                <span className="font-medium text-emerald-700">已完成: </span>
                {aiResult.completedItems.map((item, i) => (
                  <span key={i} className="mr-1 inline-block rounded bg-emerald-100 px-2 py-0.5 text-xs text-emerald-800">{item}</span>
                ))}
              </div>
            )}
            {aiResult.blockers.length > 0 && (
              <div>
                <span className="font-medium text-red-700">卡点: </span>
                {aiResult.blockers.map((b, i) => (
                  <span key={i} className="mr-1 inline-block rounded bg-red-100 px-2 py-0.5 text-xs text-red-800">{b}</span>
                ))}
              </div>
            )}
            {aiResult.nextSuggestion && (
              <div>
                <span className="font-medium text-blue-700">建议: </span>
                <span className="text-blue-800">{aiResult.nextSuggestion}</span>
              </div>
            )}
            {aiResult.impactsMainTask && (
              <p className="text-xs font-medium text-orange-600">⚠ 可能影响主线任务进度</p>
            )}
          </div>
        </div>
      )}

      {/* History */}
      {loading ? (
        <div className="text-sm text-gray-500">加载中...</div>
      ) : logs.length === 0 ? (
        <p className="text-sm text-gray-400">暂无记录</p>
      ) : (
        <div className="space-y-3">
          {logs.map((log) => (
            <div key={log.id} className="rounded-lg bg-white p-4 shadow-sm ring-1 ring-gray-200">
              <p className="text-sm text-gray-800 whitespace-pre-wrap">{log.rawText}</p>
              {log.completedItems.length > 0 && (
                <div className="mt-2 flex flex-wrap gap-1">
                  {log.completedItems.map((item, i) => (
                    <span key={i} className="rounded bg-emerald-50 px-2 py-0.5 text-xs text-emerald-700">{item}</span>
                  ))}
                </div>
              )}
              {log.blockers.length > 0 && (
                <div className="mt-2 flex flex-wrap gap-1">
                  {log.blockers.map((b, i) => (
                    <span key={i} className="rounded bg-red-50 px-2 py-0.5 text-xs text-red-700">{b}</span>
                  ))}
                </div>
              )}
              {log.aiSummary && (
                <div className="mt-2 rounded bg-blue-50 px-3 py-1.5 text-xs text-blue-700">{log.aiSummary}</div>
              )}
              <p className="mt-2 text-xs text-gray-400">{new Date(log.createdAt).toLocaleString("zh-CN")}</p>
            </div>
          ))}
        </div>
      )}
    </div>
  );
}
