import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { useAuth } from "../../contexts/AuthContext";
import { app } from "../../utils/cloudbase";
import type { IssueCard, IssueStatus } from "../../types";
import { ISSUE_STATUS_COLORS, ISSUE_STATUS_LABELS } from "../../types";
import { normalizeDocList } from "../../utils/documents";
import { getProgressColor, getThresholdProgress } from "../../utils/threshold";

export default function InternIssues() {
  const { user } = useAuth();
  const navigate = useNavigate();
  const [issues, setIssues] = useState<IssueCard[]>([]);
  const [loading, setLoading] = useState(true);
  const [resolvingIssueId, setResolvingIssueId] = useState<string | null>(null);

  useEffect(() => {
    if (!user) return;
    const db = app.database();
    db.collection("issue_cards")
      .where({ internId: user.id })
      .orderBy("createdAt", "desc")
      .limit(30)
      .get()
      .then((res) => setIssues(normalizeDocList((res.data as Array<IssueCard & { _id?: string }>) || [])))
      .catch(console.error)
      .finally(() => setLoading(false));
  }, [user]);

  const handleResolveIssue = async (issue: IssueCard) => {
    if (!user || resolvingIssueId) return;
    setResolvingIssueId(issue.id);

    try {
      const db = app.database();
      const now = new Date().toISOString();

      await db.collection("issue_cards").doc(issue.id).update({
        status: "resolved" as IssueStatus,
        updatedAt: now,
      });

      await db.collection("issue_events").add({
        issueId: issue.id,
        type: "status_change",
        content: "实习生已将该问题标记为已解决。",
        fromStatus: issue.status,
        toStatus: "resolved",
        actorId: user.id,
        actorRole: "intern",
        createdAt: now,
      });

      setIssues((prev) =>
        prev.map((item) =>
          item.id === issue.id
            ? {
                ...item,
                status: "resolved",
                updatedAt: now,
              }
            : item
        )
      );
    } catch (error) {
      console.error("标记问题为已解决失败:", error);
    } finally {
      setResolvingIssueId(null);
    }
  };

  if (loading) return <div className="text-sm text-gray-500">加载中...</div>;

  const openIssues = issues.filter((issue) => issue.status !== "resolved");
  const resolvedIssues = issues.filter((issue) => issue.status === "resolved");

  return (
    <div className="space-y-5">
      <div className="flex items-center justify-between">
        <h1 className="text-xl font-bold text-gray-900">问题卡片</h1>
        <button
          type="button"
          onClick={() => navigate("/intern/issues/new")}
          className="rounded-md bg-blue-600 px-3 py-1.5 text-sm font-medium text-white transition-colors hover:bg-blue-700"
        >
          + 创建
        </button>
      </div>

      {issues.length === 0 ? (
        <div className="rounded-lg bg-white p-8 text-center shadow-sm ring-1 ring-gray-200">
          <p className="text-sm text-gray-400">暂无问题</p>
          <button
            type="button"
            onClick={() => navigate("/intern/issues/new")}
            className="mt-3 text-sm text-blue-600 hover:underline"
          >
            遇到卡点？创建问题卡片
          </button>
        </div>
      ) : (
        <>
          {openIssues.length > 0 && (
            <div className="space-y-3">
              <h2 className="text-sm font-semibold text-gray-600">进行中 ({openIssues.length})</h2>
              {openIssues.map((issue) => (
                <div
                  key={issue.id}
                  role="button"
                  tabIndex={0}
                  onClick={() => navigate(`/intern/issues/${issue.id}`)}
                  onKeyDown={(event) => {
                    if (event.key === "Enter" || event.key === " ") {
                      event.preventDefault();
                      navigate(`/intern/issues/${issue.id}`);
                    }
                  }}
                  className="w-full rounded-lg bg-white p-4 text-left shadow-sm ring-1 ring-gray-200 transition-colors hover:ring-blue-300"
                >
                  <div className="flex items-start justify-between gap-3">
                    <div className="min-w-0">
                      <h3 className="text-sm font-semibold text-gray-900">{issue.title}</h3>
                      <p className="mt-1 text-xs text-gray-500 line-clamp-1">{issue.description}</p>
                    </div>
                    <div className="flex shrink-0 items-center gap-2">
                      <span
                        className={`rounded-full px-2 py-0.5 text-xs font-medium ${ISSUE_STATUS_COLORS[issue.status as IssueStatus]}`}
                      >
                        {ISSUE_STATUS_LABELS[issue.status as IssueStatus]}
                      </span>
                      <button
                        type="button"
                        onClick={(event) => {
                          event.stopPropagation();
                          void handleResolveIssue(issue);
                        }}
                        disabled={resolvingIssueId === issue.id}
                        className="rounded-md bg-blue-600 px-2.5 py-1 text-xs font-medium text-white transition-colors hover:bg-blue-700 disabled:cursor-not-allowed disabled:opacity-60"
                      >
                        {resolvingIssueId === issue.id ? "处理中..." : "已解决"}
                      </button>
                    </div>
                  </div>

                  <div className="mt-2 flex items-center gap-3 text-xs text-gray-400">
                    <span>{issue.issueType}</span>
                    {issue.blocksMainTask && <span className="text-orange-600">阻塞主线</span>}
                    <span>
                      {issue.elapsedMinutes}/{issue.thresholdMinutes}min
                    </span>
                  </div>

                  <div className="mt-2 h-1 rounded-full bg-gray-100">
                    <div
                      className={`h-1 rounded-full ${getProgressColor(issue.elapsedMinutes, issue.thresholdMinutes)}`}
                      style={{ width: `${getThresholdProgress(issue.elapsedMinutes, issue.thresholdMinutes)}%` }}
                    />
                  </div>
                </div>
              ))}
            </div>
          )}

          {resolvedIssues.length > 0 && (
            <div className="space-y-3">
              <h2 className="text-sm font-semibold text-gray-400">已解决 ({resolvedIssues.length})</h2>
              {resolvedIssues.map((issue) => (
                <div
                  key={issue.id}
                  role="button"
                  tabIndex={0}
                  onClick={() => navigate(`/intern/issues/${issue.id}`)}
                  onKeyDown={(event) => {
                    if (event.key === "Enter" || event.key === " ") {
                      event.preventDefault();
                      navigate(`/intern/issues/${issue.id}`);
                    }
                  }}
                  className="w-full rounded-lg bg-gray-50 p-4 text-left opacity-70 shadow-sm ring-1 ring-gray-100"
                >
                  <div className="flex items-start justify-between">
                    <h3 className="text-sm font-medium text-gray-600">{issue.title}</h3>
                    <span className="rounded-full bg-emerald-50 px-2 py-0.5 text-xs font-medium text-emerald-700">
                      已解决
                    </span>
                  </div>
                  <p className="mt-1 text-xs text-gray-400">{issue.issueType}</p>
                </div>
              ))}
            </div>
          )}
        </>
      )}
    </div>
  );
}
