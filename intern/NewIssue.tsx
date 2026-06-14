import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { useAuth } from "../../contexts/AuthContext";
import { recommendKnowledge } from "../../services/aiService";
import type {
  IssueCard,
  IssueHandlingIntent,
  IssueType,
  IssueTypeLike,
  KnowledgeItem,
  RecommendKnowledgeResult,
  Task,
} from "../../types";
import { DEFAULT_THRESHOLDS, ISSUE_TYPE_LABELS } from "../../types";
import { app } from "../../utils/cloudbase";
import {
  detectIssueTypeFallback as detectIssueTypeByRule,
  getIssueRuleOverride,
  resolveKnowledgeItems,
} from "../../utils/issueAnalysis";
import { calcIssueStatus } from "../../utils/threshold";

type HandlingIntent = IssueHandlingIntent;

const HANDLING_OPTIONS: { value: HandlingIntent; label: string }[] = [
  { value: "self_check", label: "暂不影响主线，我先自己排查" },
  { value: "need_mentor", label: "希望 Mentor 尽快看一下" },
];

interface AiResult {
  issueType: IssueTypeLike;
  thresholdMinutes: number;
  knowledgeItems: KnowledgeItem[];
  suggestion: string;
  riskLevel: "low" | "medium" | "high";
  riskReasons: string[];
  recommendNeedMentor: boolean;
}

type PreparedIssuePayload = {
  issue: Omit<IssueCard, "id">;
  knowledgeItems: KnowledgeItem[];
  aiResult: AiResult;
};

type RiskPromptState = {
  prepared: PreparedIssuePayload;
};

function detectIssueTypeFallback(description: string): IssueTypeLike {
  const mappedType = detectIssueTypeByRule(description);
  if (mappedType !== "other") {
    return mappedType;
  }

  const text = description.toLowerCase();

  if (
    text.includes("权限") ||
    text.includes("白名单") ||
    text.includes("账号") ||
    text.includes("access") ||
    text.includes("permission")
  ) {
    return "permission";
  }

  if (
    text.includes("环境") ||
    text.includes("配置") ||
    text.includes("启动") ||
    text.includes("数据库") ||
    text.includes("network") ||
    text.includes("env")
  ) {
    return "environment";
  }

  if (
    text.includes("需求") ||
    text.includes("业务") ||
    text.includes("客户") ||
    text.includes("优先级") ||
    text.includes("范围")
  ) {
    return "business";
  }

  if (
    text.includes("代码") ||
    text.includes("接口") ||
    text.includes("报错") ||
    text.includes("bug") ||
    text.includes("功能")
  ) {
    return "technical";
  }

  return "other";
}

function buildRuleBasedRisk(description: string) {
  const text = description.toLowerCase();
  const riskReasons: string[] = [];
  const ruleOverride = getIssueRuleOverride(description);

  if (ruleOverride) {
    return {
      blocksMainTask: false,
      recommendNeedMentor: ruleOverride.recommendNeedMentor,
      riskLevel: ruleOverride.riskLevel,
      riskReasons: ruleOverride.riskReasons,
      recommendationText: ruleOverride.recommendationText,
    } as const;
  }

  const blocksMainTask =
    text.includes("主线") ||
    text.includes("阻塞") ||
    text.includes("卡住") ||
    text.includes("无法继续");

  if (blocksMainTask) {
    riskReasons.push("问题可能已经影响主线任务推进");
  }
  if (text.includes("权限") || text.includes("白名单") || text.includes("账号")) {
    riskReasons.push("问题涉及权限或资源协调，建议尽快联系 Mentor");
  }
  if (text.includes("客户") || text.includes("线上") || text.includes("生产")) {
    riskReasons.push("问题可能涉及客户或线上环境影响");
  }
  if (text.includes("合规") || text.includes("安全") || text.includes("敏感")) {
    riskReasons.push("问题存在合规或安全风险");
  }

  const recommendNeedMentor = riskReasons.length > 0;

  return {
    blocksMainTask,
    recommendNeedMentor,
    riskLevel: riskReasons.length >= 2 ? "high" : recommendNeedMentor ? "medium" : "low",
    riskReasons,
    recommendationText: recommendNeedMentor
      ? "该问题可能不适合继续自行排查，建议尽快联系 Mentor。"
      : "可以先继续自主排查。",
  } as const;
}

function parseAttemptedMethods(value: string) {
  return value
    .split(/[，。,、\n]/)
    .map((item) => item.trim())
    .filter(Boolean);
}

export default function NewIssue() {
  const { user } = useAuth();
  const navigate = useNavigate();
  const [tasks, setTasks] = useState<Task[]>([]);
  const [currentTask, setCurrentTask] = useState<Task | null>(null);
  const [description, setDescription] = useState("");
  const [attemptedMethods, setAttemptedMethods] = useState("");
  const [handlingIntent, setHandlingIntent] = useState<HandlingIntent>("self_check");
  const [submitting, setSubmitting] = useState(false);
  const [aiResult, setAiResult] = useState<AiResult | null>(null);
  const [createdIssueId, setCreatedIssueId] = useState("");
  const [riskPrompt, setRiskPrompt] = useState<RiskPromptState | null>(null);

  useEffect(() => {
    if (!user) return;
    const db = app.database();

    db.collection("tasks")
      .where({ internId: user.id })
      .limit(20)
      .get()
      .then((res) => {
        const list = (res.data as Task[]) || [];
        setTasks(list);
        const active = list.find((task) => task.status === "in_progress");
        if (active) setCurrentTask(active);
      })
      .catch(() => {});
  }, [user]);

  const finalizeCreate = async (
    prepared: PreparedIssuePayload,
    overrides?: Partial<Omit<IssueCard, "id">>
  ) => {
    if (!user) return;

    const db = app.database();
    const now = new Date().toISOString();
    const issueToCreate = {
      ...prepared.issue,
      ...overrides,
      updatedAt: now,
    };

    const addRes = await db.collection("issue_cards").add(issueToCreate);
    const issueId = (addRes as { id?: string }).id || "";

    if (issueId) {
      await db.collection("issue_cards").doc(issueId).update({
        id: issueId,
        updatedAt: now,
      });
    }

    await db.collection("issue_events").add({
      issueId,
      type: "created",
      content: `创建了问题卡片：${issueToCreate.title}`,
      toStatus: issueToCreate.status,
      actorId: user.id,
      actorRole: "intern",
      createdAt: now,
    });

    if (issueToCreate.aiRecommendNeedMentor) {
      await db.collection("issue_events").add({
        issueId,
        type: "ai_action",
        content: `AI 风险提示：${issueToCreate.aiRiskReasons?.join("；") || "建议尽快联系 Mentor"}`,
        actorId: user.id,
        actorRole: "intern",
        createdAt: now,
      });
    }

    setAiResult(prepared.aiResult);
    setCreatedIssueId(issueId);
    setRiskPrompt(null);
  };

  const buildPreparedIssue = async (): Promise<PreparedIssuePayload> => {
    if (!user) {
      throw new Error("用户未登录");
    }

    const db = app.database();
    const now = new Date().toISOString();
    const descriptionText = description.trim();
    const attemptedText = attemptedMethods.trim();
    const attemptedList = parseAttemptedMethods(attemptedText);
    const analysisText = [currentTask?.title || "", descriptionText, attemptedText].filter(Boolean).join(" ");
    const fallbackType = detectIssueTypeFallback(analysisText);
    const fallbackRisk = buildRuleBasedRisk(analysisText);
    const issueRuleOverride = getIssueRuleOverride(analysisText);

    let recommendedIds: string[] = [];
    let knowledgeItems: KnowledgeItem[] = [];
    let aiAnalysis: RecommendKnowledgeResult | null = null;

    try {
      const knowledgeRes = await db.collection("knowledge_items").limit(50).get();
      const allIds = ((knowledgeRes.data as Array<{ id: string }>) || []).map((item) => item.id);
      aiAnalysis = await recommendKnowledge(analysisText, fallbackType, user.department || "", allIds);

      recommendedIds = aiAnalysis?.recommendedKnowledgeIds ?? aiAnalysis?.recommendedIds ?? [];

      if (recommendedIds.length > 0) {
        const knowledgeMatchRes = await db.collection("knowledge_items")
          .where({ id: db.command.in(recommendedIds) })
          .limit(5)
          .get();
        knowledgeItems = resolveKnowledgeItems(recommendedIds, (knowledgeMatchRes.data as KnowledgeItem[]) || []);
      }
    } catch (error) {
      console.warn("AI recommendKnowledge failed:", error);
    }

    const resolvedRecommendedIds = issueRuleOverride?.recommendedKnowledgeIds
      || aiAnalysis?.recommendedKnowledgeIds
      || aiAnalysis?.recommendedIds
      || recommendedIds;
    if (issueRuleOverride) {
      knowledgeItems = resolveKnowledgeItems(resolvedRecommendedIds, knowledgeItems);
    }

    const issueType = issueRuleOverride?.issueType || aiAnalysis?.issueType || fallbackType;
    const threshold = issueRuleOverride?.thresholdMinutes
      || DEFAULT_THRESHOLDS[issueType as IssueType]
      || DEFAULT_THRESHOLDS[fallbackType]
      || 60;
    const blocksMainTask = fallbackRisk.blocksMainTask;
    const recommendNeedMentor = issueRuleOverride?.recommendNeedMentor
      ?? aiAnalysis?.recommendNeedMentor
      ?? fallbackRisk.recommendNeedMentor;
    const riskReasons = issueRuleOverride?.riskReasons
      ?? (aiAnalysis?.riskReasons?.length ? aiAnalysis.riskReasons : fallbackRisk.riskReasons);
    const riskLevel = issueRuleOverride?.riskLevel || aiAnalysis?.riskLevel || fallbackRisk.riskLevel;
    const recommendationText = issueRuleOverride?.recommendationText
      || aiAnalysis?.recommendationText
      || aiAnalysis?.reasoning
      || fallbackRisk.recommendationText;
    const initialStatus =
      handlingIntent === "need_mentor"
        ? "waiting_mentor"
        : calcIssueStatus(0, threshold, "self_checking", blocksMainTask, riskLevel === "high", "self_check");

    return {
      issue: {
        internId: user.id,
        internName: user.name,
        taskId: currentTask?.id || "",
        taskTitle: currentTask?.title || "",
        title: descriptionText.substring(0, 50),
        description: descriptionText,
        attemptedMethods: attemptedList,
        initialJudgement: "",
        expectedHelp: "",
        blocksMainTask,
        issueType,
        status: initialStatus,
        handlingIntent,
        elapsedMinutes: 0,
        thresholdMinutes: threshold,
        aiSummary: recommendationText,
        recommendedKnowledgeIds: resolvedRecommendedIds,
        recommendedKnowledge: knowledgeItems,
        aiRiskLevel: riskLevel,
        aiRiskReasons: riskReasons,
        aiRecommendNeedMentor: recommendNeedMentor,
        aiRiskWarningAcknowledged: false,
        isHighRisk: riskLevel === "high",
        createdAt: now,
        updatedAt: now,
      },
      knowledgeItems,
      aiResult: {
        issueType,
        thresholdMinutes: threshold,
        knowledgeItems,
        suggestion: recommendationText,
        riskLevel,
        riskReasons,
        recommendNeedMentor,
      },
    };
  };

  const handleSubmit = async () => {
    if (!user || !description.trim()) return;
    setSubmitting(true);

    try {
      const prepared = await buildPreparedIssue();

      if (handlingIntent === "self_check" && prepared.aiResult.recommendNeedMentor) {
        setRiskPrompt({ prepared });
        return;
      }

      await finalizeCreate(prepared);
    } catch (error) {
      console.error("创建问题卡片失败:", error);
    } finally {
      setSubmitting(false);
    }
  };

  const currentTaskId = currentTask?.id ?? "";

  if (aiResult) {
    return (
      <div className="mx-auto max-w-lg space-y-5">
        <div className="flex items-center gap-3">
          <div className="flex h-8 w-8 items-center justify-center rounded-full bg-blue-100">
            <svg className="h-4 w-4 text-blue-600" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 10V3L4 14h7v7l9-11h-7z" />
            </svg>
          </div>
          <h1 className="text-lg font-bold text-gray-900">AI 已完成辅助分析</h1>
        </div>

        <div className="rounded-lg border border-blue-200 bg-blue-50 p-4 space-y-3">
          <div className="flex items-center justify-between">
            <span className="text-sm text-blue-700">问题类型</span>
            <span className="rounded-full bg-blue-600 px-3 py-1 text-xs font-medium text-white">
              {ISSUE_TYPE_LABELS[aiResult.issueType] || aiResult.issueType}
            </span>
          </div>
          <div className="flex items-center justify-between">
            <span className="text-sm text-blue-700">默认沟通阈值</span>
            <span className="text-sm font-semibold text-blue-900">{aiResult.thresholdMinutes} 分钟</span>
          </div>
        </div>

        {aiResult.knowledgeItems.length > 0 && (
          <div className="rounded-lg bg-white p-4 shadow-sm ring-1 ring-gray-200">
            <h3 className="text-sm font-semibold text-gray-900">可优先参考的资料</h3>
            <div className="mt-3 space-y-2">
              {aiResult.knowledgeItems.map((item) => (
                <div key={item.id} className="rounded-md border border-gray-100 px-3 py-2">
                  <p className="text-sm font-medium text-gray-800">{item.title}</p>
                  {item.tags?.length > 0 && (
                    <div className="mt-1 flex flex-wrap gap-1">
                      {item.tags.map((tag) => (
                        <span key={`${item.id}-${tag}`} className="rounded bg-blue-50 px-1.5 py-0.5 text-[10px] text-blue-600">
                          {tag}
                        </span>
                      ))}
                    </div>
                  )}
                </div>
              ))}
            </div>
          </div>
        )}

        {aiResult.suggestion && (
          <div className={`rounded-lg border p-4 ${aiResult.recommendNeedMentor ? "border-amber-200 bg-amber-50" : "border-slate-200 bg-slate-50"}`}>
            <h3 className={`text-sm font-semibold ${aiResult.recommendNeedMentor ? "text-amber-800" : "text-slate-800"}`}>
              AI 建议
            </h3>
            <p className={`mt-2 text-sm whitespace-pre-wrap ${aiResult.recommendNeedMentor ? "text-amber-700" : "text-slate-700"}`}>
              {aiResult.suggestion}
            </p>
          </div>
        )}

        <div className="space-y-3">
          <button
            type="button"
            onClick={() => navigate(`/intern/issues/${createdIssueId}`)}
            className="w-full rounded-lg bg-blue-600 px-4 py-3 text-sm font-medium text-white shadow-sm hover:bg-blue-700 transition-colors"
          >
            查看问题详情
          </button>
          <button
            type="button"
            onClick={() => navigate("/intern/issues")}
            className="w-full rounded-lg bg-white px-4 py-3 text-sm font-medium text-gray-700 ring-1 ring-gray-300 shadow-sm hover:bg-gray-50 transition-colors"
          >
            返回问题列表
          </button>
        </div>
      </div>
    );
  }

  return (
    <div className="mx-auto max-w-lg space-y-5">
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
        <h1 className="text-lg font-bold text-gray-900">创建问题卡片</h1>
      </div>

      <div className="rounded-lg bg-white p-5 shadow-sm ring-1 ring-gray-200 space-y-5">
        <div>
          <label className="mb-1 block text-sm font-medium text-gray-700">当前任务</label>
          {currentTask ? (
            <div className="rounded-md border border-blue-200 bg-blue-50 px-3 py-2.5 text-sm">
              <span className="text-blue-500">自动关联：</span>
              <span className="font-medium text-blue-900">{currentTask.title}</span>
            </div>
          ) : tasks.length > 0 ? (
            <select
              value={currentTaskId}
              onChange={(event) => {
                const matched = tasks.find((task) => task.id === event.target.value);
                setCurrentTask(matched || null);
              }}
              className="w-full rounded-md border border-gray-200 px-3 py-2 text-sm focus:border-blue-400 focus:outline-none focus:ring-1 focus:ring-blue-400"
            >
              <option value="">选择关联任务</option>
              {tasks.map((task) => (
                <option key={task.id} value={task.id}>{task.title}</option>
              ))}
            </select>
          ) : (
            <p className="text-sm text-gray-400">暂无进行中的任务</p>
          )}
        </div>

        <div>
          <label className="mb-1 block text-sm font-medium text-gray-700">我卡在哪里？</label>
          <textarea
            value={description}
            onChange={(event) => setDescription(event.target.value)}
            placeholder="简单描述问题，例如：启动服务后数据库连接失败。"
            rows={3}
            className="w-full resize-none rounded-md border border-gray-200 px-3 py-2 text-sm placeholder-gray-400 focus:border-blue-400 focus:outline-none focus:ring-1 focus:ring-blue-400"
          />
        </div>

        <div>
          <label className="mb-1 block text-sm font-medium text-gray-700">
            我已经试过什么？<span className="font-normal text-gray-400">（选填）</span>
          </label>
          <textarea
            value={attemptedMethods}
            onChange={(event) => setAttemptedMethods(event.target.value)}
            placeholder="例如：检查配置文件、重启服务、搜索报错关键词。"
            rows={2}
            className="w-full resize-none rounded-md border border-gray-200 px-3 py-2 text-sm placeholder-gray-400 focus:border-blue-400 focus:outline-none focus:ring-1 focus:ring-blue-400"
          />
        </div>

        <div>
          <label className="mb-2 block text-sm font-medium text-gray-700">当前希望如何处理？</label>
          <div className="space-y-2">
            {HANDLING_OPTIONS.map((option) => {
              const active = handlingIntent === option.value;
              const mentorOption = option.value === "need_mentor";

              return (
                <label
                  key={option.value}
                  className={`flex cursor-pointer items-center gap-3 rounded-lg border px-4 py-3 transition-colors ${
                    active
                      ? mentorOption
                        ? "border-red-300 bg-red-50"
                        : "border-blue-300 bg-blue-50"
                      : "border-gray-200 bg-white hover:bg-gray-50"
                  }`}
                >
                  <input
                    type="radio"
                    name="handlingIntent"
                    value={option.value}
                    checked={active}
                    onChange={() => setHandlingIntent(option.value)}
                    className={`h-4 w-4 ${mentorOption ? "text-red-600 focus:ring-red-500" : "text-blue-600 focus:ring-blue-500"}`}
                  />
                  <span className={`text-sm ${active ? (mentorOption ? "font-medium text-red-800" : "font-medium text-blue-800") : "text-gray-700"}`}>
                    {option.label}
                  </span>
                </label>
              );
            })}
          </div>
        </div>
      </div>

      {riskPrompt && (
        <div className="rounded-lg border border-amber-200 bg-amber-50 p-4 space-y-3">
          <div>
            <h2 className="text-sm font-semibold text-amber-900">AI 建议尽快联系 Mentor</h2>
            <p className="mt-2 text-sm leading-6 text-amber-800">
              该问题可能涉及：
              {riskPrompt.prepared.aiResult.riskReasons.length > 0
                ? ` ${riskPrompt.prepared.aiResult.riskReasons.join("；")}`
                : " 继续自行排查的收益可能较低"}
            </p>
            <p className="mt-2 text-sm text-amber-700">
              继续自己排查也可以，但系统建议你优先同步 Mentor。
            </p>
          </div>
          <div className="space-y-2">
            <button
              type="button"
              onClick={async () => {
                setSubmitting(true);
                try {
                  await finalizeCreate(riskPrompt.prepared, {
                    handlingIntent: "need_mentor",
                    status: "waiting_mentor",
                    aiRiskWarningAcknowledged: true,
                  });
                } finally {
                  setSubmitting(false);
                }
              }}
              className="w-full rounded-lg bg-amber-500 px-4 py-3 text-sm font-medium text-white shadow-sm transition-colors hover:bg-amber-600"
            >
              改为联系 Mentor 并提交
            </button>
            <button
              type="button"
              onClick={async () => {
                setSubmitting(true);
                try {
                  await finalizeCreate(riskPrompt.prepared, {
                    handlingIntent: "self_check",
                    aiRiskWarningAcknowledged: true,
                  });
                } finally {
                  setSubmitting(false);
                }
              }}
              className="w-full rounded-lg bg-white px-4 py-3 text-sm font-medium text-amber-800 ring-1 ring-amber-200 transition-colors hover:bg-amber-100"
            >
              仍先自己排查
            </button>
          </div>
        </div>
      )}

      <button
        type="button"
        onClick={handleSubmit}
        disabled={submitting || !description.trim()}
        className="w-full rounded-lg bg-blue-600 px-4 py-3 text-sm font-medium text-white shadow-sm transition-colors hover:bg-blue-700 disabled:cursor-not-allowed disabled:opacity-50"
      >
        {submitting ? "AI 分析中..." : "提交问题卡片"}
      </button>
    </div>
  );
}
