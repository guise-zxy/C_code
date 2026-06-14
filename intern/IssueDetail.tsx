import { useCallback, useEffect, useState } from "react";
import { useNavigate, useParams } from "react-router-dom";
import { useAuth } from "../../contexts/AuthContext";
import { generateHelpSummary } from "../../services/aiService";
import type { IssueCard, IssueEvent, IssueStatus, KnowledgeItem } from "../../types";
import { ISSUE_STATUS_COLORS, ISSUE_STATUS_LABELS } from "../../types";
import { app } from "../../utils/cloudbase";
import { normalizeDocId } from "../../utils/documents";
import { resolveKnowledgeItems } from "../../utils/issueAnalysis";
import {
  calcIssueStatus,
  getProgressColor,
  getThresholdProgress,
  shouldShowContactMentor,
} from "../../utils/threshold";

export default function InternIssueDetail() {
  const { id } = useParams<{ id: string }>();
  const { user } = useAuth();
  const navigate = useNavigate();
  const [issue, setIssue] = useState<IssueCard | null>(null);
  const [events, setEvents] = useState<IssueEvent[]>([]);
  const [knowledge, setKnowledge] = useState<KnowledgeItem[]>([]);
  const [loading, setLoading] = useState(true);
  const [updateNote, setUpdateNote] = useState("");
  const [submitting, setSubmitting] = useState(false);

  const loadIssue = useCallback(async () => {
    if (!id || !user) return;
    const db = app.database();

    try {
      const [issueRes, eventRes] = await Promise.all([
        db.collection("issue_cards").doc(id).get(),
        db.collection("issue_events").where({ issueId: id }).orderBy("createdAt", "asc").limit(50).get(),
      ]);

      const rawIssue = (issueRes.data as Array<IssueCard & { _id?: string }>)[0];
      if (!rawIssue) return;

      const issueData = normalizeDocId(rawIssue);
      const elapsed = Math.max(issueData.elapsedMinutes, calcElapsedMinutesRaw(issueData.createdAt));
      const newStatus = calcIssueStatus(
        elapsed,
        issueData.thresholdMinutes,
        issueData.status as IssueStatus,
        issueData.blocksMainTask,
        issueData.isHighRisk || false,
        issueData.handlingIntent
      );

      if (newStatus !== issueData.status) {
        issueData.status = newStatus;
        issueData.elapsedMinutes = elapsed;
        await db.collection("issue_cards").doc(id).update({
          status: newStatus,
          elapsedMinutes: elapsed,
          updatedAt: new Date().toISOString(),
        });
      } else {
        issueData.elapsedMinutes = elapsed;
      }

      setIssue(issueData);
      setEvents((eventRes.data as IssueEvent[]) || []);

      if (issueData.recommendedKnowledgeIds?.length > 0) {
        const knowledgeRes = await db
          .collection("knowledge_items")
          .where({ id: db.command.in(issueData.recommendedKnowledgeIds) })
          .limit(20)
          .get();
        setKnowledge(resolveKnowledgeItems(issueData.recommendedKnowledgeIds, (knowledgeRes.data as KnowledgeItem[]) || []));
      } else {
        setKnowledge(issueData.recommendedKnowledge || []);
      }
    } catch (err) {
      console.error("加载问题详情失败:", err);
    } finally {
      setLoading(false);
    }
  }, [id, user]);

  useEffect(() => {
    void loadIssue();
  }, [loadIssue]);

  useEffect(() => {
    const timer = setInterval(() => {
      if (issue && issue.status !== "resolved") {
        const elapsed = calcElapsedMinutesRaw(issue.createdAt);
        const newStatus = calcIssueStatus(
          elapsed,
          issue.thresholdMinutes,
          issue.status as IssueStatus,
          issue.blocksMainTask,
          issue.isHighRisk || false,
          issue.handlingIntent
        );
        setIssue((prev) => (prev ? { ...prev, elapsedMinutes: elapsed, status: newStatus } : prev));
      }
    }, 30000);

    return () => clearInterval(timer);
  }, [issue]);

  const handleUpdateProgress = async () => {
    if (!issue || !user || !updateNote.trim()) return;
    setSubmitting(true);

    try {
      const db = app.database();
      const now = new Date().toISOString();
      const elapsed = calcElapsedMinutesRaw(issue.createdAt);

      await db.collection("issue_events").add({
        issueId: id,
        type: "update",
        content: updateNote.trim(),
        actorId: user.id,
        actorRole: "intern",
        createdAt: now,
      });

      await db.collection("issue_cards").doc(id!).update({
        elapsedMinutes: elapsed,
        updatedAt: now,
      });

      setUpdateNote("");
      await loadIssue();
    } catch (err) {
      console.error("更新排查进展失败:", err);
    } finally {
      setSubmitting(false);
    }
  };

  const handleContactMentor = async () => {
    if (!issue || !user) return;
    setSubmitting(true);

    try {
      const db = app.database();
      const now = new Date().toISOString();

      let helpSummary = "";
      try {
        const aiResult = await generateHelpSummary(
          issue.title,
          issue.description,
          issue.attemptedMethods,
          issue.initialJudgement,
          issue.elapsedMinutes,
          issue.issueType
        );
        if (aiResult) helpSummary = aiResult.helpSummary;
      } catch (err) {
        console.warn("AI generateHelpSummary failed:", err);
      }

      const newStatus: IssueStatus = "waiting_mentor";

      await db.collection("issue_events").add({
        issueId: id,
        type: "status_change",
        content: "实习生主动联系 Mentor",
        fromStatus: issue.status,
        toStatus: newStatus,
        actorId: user.id,
        actorRole: "intern",
        createdAt: now,
      });

      await db.collection("issue_cards").doc(id!).update({
        handlingIntent: "need_mentor",
        status: newStatus,
        helpSummary: helpSummary || undefined,
        updatedAt: now,
      });

      await loadIssue();
    } catch (err) {
      console.error("联系 Mentor 失败:", err);
    } finally {
      setSubmitting(false);
    }
  };

  if (loading) {
    return (
      <div className="flex items-center justify-center py-12">
        <div className="h-6 w-6 animate-spin rounded-full border-2 border-blue-500 border-t-transparent" />
      </div>
    );
  }

  if (!issue) {
    return <p className="text-sm text-gray-500">问题不存在</p>;
  }

  const progress = getThresholdProgress(issue.elapsedMinutes, issue.thresholdMinutes);
  const progressColor = getProgressColor(issue.elapsedMinutes, issue.thresholdMinutes);
  const showContact = shouldShowContactMentor(issue);
  const timelineEvents = [...events].sort(
    (left, right) => new Date(right.createdAt).getTime() - new Date(left.createdAt).getTime()
  );
  const latestEvent = timelineEvents[0] ?? null;
  const hasAttemptedMethods = Array.isArray(issue.attemptedMethods) && issue.attemptedMethods.length > 0;

  return (
    <div
      className="space-y-5"
      style={{ fontFamily: '"Noto Sans SC", "PingFang SC", "Microsoft YaHei", sans-serif' }}
    >
      <div className="flex items-center gap-3">
        <button
          type="button"
          onClick={() => navigate("/intern/issues")}
          className="rounded-md p-1.5 text-gray-400 hover:bg-gray-100 hover:text-gray-600"
        >
          <svg className="h-5 w-5" fill="none" viewBox="0 0 24 24" stroke="currentColor">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M15 19l-7-7 7-7" />
          </svg>
        </button>
        <div>
          <h1 className="text-lg font-bold text-slate-900">问题详情</h1>
        </div>
      </div>

      <div className="grid gap-5 xl:grid-cols-[minmax(0,1.15fr)_minmax(300px,0.85fr)]">
        <div className="space-y-5">
          <div className="overflow-hidden rounded-2xl border border-sky-100 bg-gradient-to-br from-white via-sky-50 to-blue-50 p-5 shadow-sm ring-1 ring-sky-100">
            <div className="flex flex-wrap items-start justify-between gap-3">
              <div className="space-y-2">
                <div className="flex flex-wrap items-center gap-2 text-xs">
                  {issue.blocksMainTask && (
                    <span className="rounded-full border border-orange-200 bg-orange-50 px-2.5 py-1 font-medium text-orange-700">
                      阻塞主线
                    </span>
                  )}
                  {issue.isHighRisk && (
                    <span className="rounded-full border border-red-200 bg-red-50 px-2.5 py-1 font-medium text-red-700">
                      高风险
                    </span>
                  )}
                </div>
                <h2 className="text-xl font-semibold leading-8 text-slate-900">{issue.title}</h2>
                <p className="max-w-2xl text-sm leading-6 text-slate-600">{issue.description}</p>
              </div>
              <span
                className={`shrink-0 rounded-full px-3 py-1.5 text-xs font-semibold ${ISSUE_STATUS_COLORS[issue.status as IssueStatus]}`}
              >
                {ISSUE_STATUS_LABELS[issue.status as IssueStatus]}
              </span>
            </div>

            <div className="mt-5 grid gap-3 sm:grid-cols-3">
              <div className="rounded-xl border border-slate-200 bg-white/85 px-4 py-3">
                <p className="text-[11px] font-semibold uppercase tracking-[0.18em] text-slate-400">当前任务</p>
                <p className="mt-2 text-sm font-medium leading-6 text-slate-800">{issue.taskTitle || "未关联任务"}</p>
              </div>
              <div className="rounded-xl border border-slate-200 bg-white/85 px-4 py-3">
                <p className="text-[11px] font-semibold uppercase tracking-[0.18em] text-slate-400">已排查时长</p>
                <p className="mt-2 text-lg font-semibold text-slate-900">{issue.elapsedMinutes} 分钟</p>
              </div>
              <div className="rounded-xl border border-slate-200 bg-white/85 px-4 py-3">
                <p className="text-[11px] font-semibold uppercase tracking-[0.18em] text-slate-400">默认阈值</p>
                <p className="mt-2 text-lg font-semibold text-slate-900">{issue.thresholdMinutes} 分钟</p>
              </div>
            </div>

            <div className="mt-5 rounded-xl border border-slate-200 bg-white/90 p-4">
              <div className="mb-2 flex items-center justify-between text-xs font-medium text-slate-500">
                <span>排查进度</span>
                <span>{progress}%</span>
              </div>
              <div className="h-2.5 rounded-full bg-slate-100">
                <div className={`h-2.5 rounded-full transition-all ${progressColor}`} style={{ width: `${progress}%` }} />
              </div>
              <div className="mt-2 flex items-center justify-between text-xs text-slate-500">
                <span>已排查 {issue.elapsedMinutes} 分钟</span>
                <span>阈值 {issue.thresholdMinutes} 分钟</span>
              </div>
            </div>
          </div>

          {knowledge.length > 0 && (
            <div className="rounded-2xl border border-sky-200 bg-gradient-to-br from-white via-sky-50/75 to-blue-50/85 p-5 shadow-sm ring-1 ring-sky-100">
              <div className="flex items-start justify-between gap-3">
                <div>
                  <p className="text-base font-semibold tracking-[0.04em] text-slate-900">AI 推荐资料</p>
                  <h3 className="mt-1 text-sm font-medium text-slate-500">建议优先查看这些内容</h3>
                </div>
                <div className="rounded-full bg-blue-100 px-2.5 py-1 text-xs font-medium text-blue-800">
                  {knowledge.length} 条建议
                </div>
              </div>
              <div className="mt-4 space-y-3">
                {knowledge.map((item, index) => (
                  <div key={item.id} className="rounded-xl border border-blue-100 bg-blue-50/60 px-4 py-3">
                    <div className="flex items-start gap-3">
                      <div className="mt-0.5 flex h-6 w-6 shrink-0 items-center justify-center rounded-full bg-blue-600 text-[11px] font-semibold text-white">
                        {index + 1}
                      </div>
                      <div className="min-w-0 flex-1">
                        <p className="text-sm font-semibold text-slate-900">{item.title}</p>
                        <p className="mt-1 text-xs leading-5 text-slate-600 line-clamp-2">{item.content}</p>
                        {item.tags?.length > 0 && (
                          <div className="mt-2 flex flex-wrap gap-1.5">
                            {item.tags.map((tag) => (
                              <span
                                key={`${item.id}-${tag}`}
                                className="rounded-full bg-white px-2 py-1 text-[10px] font-medium text-blue-700 ring-1 ring-blue-200"
                              >
                                {tag}
                              </span>
                            ))}
                          </div>
                        )}
                      </div>
                    </div>
                  </div>
                ))}
              </div>
            </div>
          )}

          <div className="rounded-2xl border border-slate-200 bg-white p-5 shadow-sm ring-1 ring-gray-100">
            <div className="flex items-start justify-between gap-3">
              <div>
                <h3 className="mt-1 text-base font-semibold text-slate-900">最近动态与处理记录</h3>
              </div>
              <div className="rounded-full bg-slate-100 px-2.5 py-1 text-xs font-medium text-slate-600">
                {timelineEvents.length} 条记录
              </div>
            </div>

            {latestEvent && (
              <div className="mt-4 rounded-xl border border-slate-200 bg-slate-50 px-4 py-3">
                <p className="text-xs font-medium text-slate-500">最近一次更新</p>
                <p className="mt-1 text-sm font-medium leading-6 text-slate-900">{latestEvent.content}</p>
                <p className="mt-1 text-xs text-slate-500">{new Date(latestEvent.createdAt).toLocaleString("zh-CN")}</p>
              </div>
            )}

            {timelineEvents.length === 0 ? (
              <p className="mt-4 text-sm text-slate-400">暂无记录</p>
            ) : (
              <div className="mt-4 space-y-3">
                {timelineEvents.map((event, index) => {
                  const dotColor =
                    event.type === "status_change"
                      ? "bg-orange-400"
                      : event.type === "created"
                        ? "bg-slate-700"
                        : "bg-amber-500";

                  return (
                    <div key={event.id} className="flex gap-3 rounded-xl border border-slate-100 px-3 py-3">
                      <div className="flex flex-col items-center pt-1">
                        <div className={`h-2.5 w-2.5 rounded-full ${dotColor}`} />
                        {index !== timelineEvents.length - 1 && <div className="mt-1 h-full w-px bg-slate-200" />}
                      </div>
                      <div className="min-w-0 flex-1">
                        <p className="text-sm font-medium leading-6 text-slate-800">{event.content}</p>
                        <p className="mt-1 text-xs text-slate-500">{new Date(event.createdAt).toLocaleString("zh-CN")}</p>
                      </div>
                    </div>
                  );
                })}
              </div>
            )}
          </div>
        </div>

        <div className="space-y-5">
          {issue.helpSummary && (
            <div className="rounded-2xl border border-blue-200 bg-blue-50 p-4">
              <h3 className="text-sm font-semibold text-blue-800">AI 求助摘要</h3>
              <p className="mt-2 whitespace-pre-wrap text-sm leading-6 text-blue-700">{issue.helpSummary}</p>
            </div>
          )}

          {issue.status !== "resolved" && (
            <div className="rounded-2xl border border-slate-200 bg-white p-5 shadow-sm ring-1 ring-gray-100">
              <h3 className="text-base font-semibold text-slate-900">更新排查进展</h3>
              <p className="mt-1 text-xs text-slate-500">
                把刚做过的动作和结果记录下来，方便自己和 Mentor 快速对齐。
              </p>
              <textarea
                value={updateNote}
                onChange={(event) => setUpdateNote(event.target.value)}
                placeholder="描述你最新的排查进展..."
                rows={3}
                className="mt-3 w-full resize-none rounded-lg border border-gray-200 px-3 py-2.5 text-sm placeholder-gray-400 focus:border-blue-400 focus:outline-none focus:ring-1 focus:ring-blue-400"
              />
              <button
                type="button"
                onClick={handleUpdateProgress}
                disabled={submitting || !updateNote.trim()}
                className="mt-3 rounded-lg bg-blue-600 px-4 py-2.5 text-sm font-medium text-white transition-colors hover:bg-blue-700 disabled:opacity-50"
              >
                记录进展
              </button>
            </div>
          )}

          {hasAttemptedMethods && (
            <div className="rounded-2xl border border-slate-200 bg-white p-5 shadow-sm ring-1 ring-gray-100">
              <p className="text-[11px] font-semibold uppercase tracking-[0.22em] text-slate-900">已尝试动作</p>
              <div className="mt-3 flex flex-wrap gap-2">
                {issue.attemptedMethods.map((method, index) => (
                  <span key={index} className="rounded-full bg-slate-100 px-3 py-1.5 text-xs font-medium text-slate-900">
                    {method}
                  </span>
                ))}
              </div>
            </div>
          )}

          {showContact && issue.status !== "resolved" && (
            <div className="relative rounded-2xl border border-rose-200 bg-rose-50 p-4">
              <span className="absolute right-4 top-4 flex h-7 w-7 items-center justify-center rounded-full bg-rose-500 text-sm font-bold text-white shadow-sm">
                !
              </span>
              <p className="text-xs font-semibold uppercase tracking-[0.2em] text-rose-700">需要升级沟通？</p>
              <p className="mt-2 text-sm leading-6 text-orange-800">
                如果你已经排查了一段时间，或者当前问题开始影响主线任务，可以直接发起沟通。
              </p>
              <button
                type="button"
                onClick={handleContactMentor}
                disabled={submitting}
                className="mt-3 w-full rounded-lg bg-orange-500 px-4 py-3 text-sm font-medium text-white shadow-sm transition-colors hover:bg-orange-600 disabled:opacity-50"
              >
                {submitting ? "生成求助摘要中..." : "主动联系 Mentor"}
              </button>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

function calcElapsedMinutesRaw(createdAt: string): number {
  const created = new Date(createdAt).getTime();
  const now = Date.now();
  return Math.max(0, Math.round((now - created) / 60000));
}
