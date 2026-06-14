import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { useAuth } from "../../contexts/AuthContext";
import { app } from "../../utils/cloudbase";
import { normalizeDocList } from "../../utils/documents";
import type { IssueCard, Task } from "../../types";
import { ISSUE_STATUS_COLORS, ISSUE_STATUS_LABELS } from "../../types";

export default function InternDashboard() {
  const { user } = useAuth();
  const navigate = useNavigate();
  const [tasks, setTasks] = useState<Task[]>([]);
  const [openIssues, setOpenIssues] = useState<IssueCard[]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    if (!user) return;

    const db = app.database();
    const load = async () => {
      try {
        const [taskRes, issueRes] = await Promise.all([
          db.collection("tasks").where({ internId: user.id }).limit(20).get(),
          db.collection("issue_cards").where({ internId: user.id, status: db.command.neq("resolved") }).limit(20).get(),
        ]);

        setTasks((taskRes.data as Task[]) || []);
        setOpenIssues(normalizeDocList((issueRes.data as Array<IssueCard & { _id?: string }>) || []));
      } catch (err) {
        console.error("加载数据失败:", err);
      } finally {
        setLoading(false);
      }
    };

    void load();
  }, [user]);

  if (loading) {
    return (
      <div className="flex items-center justify-center py-12">
        <div className="h-6 w-6 animate-spin rounded-full border-2 border-blue-500 border-t-transparent" />
      </div>
    );
  }

  const inProgressTasks = tasks.filter((task) => task.status === "in_progress");
  const doneCount = tasks.filter((task) => task.status === "done").length;
  const urgentIssues = openIssues.filter(
    (issue) =>
      issue.status === "near_threshold" ||
      issue.status === "need_communicate" ||
      issue.status === "waiting_mentor"
  );

  return (
    <div className="space-y-6">
      <div>
        <h1 className="text-xl font-bold text-gray-900">你好，{user?.name}</h1>
        <p className="mt-1 text-sm text-gray-500">
          {user?.stage || "入职适应期"} · 第 {user?.currentWeek || 1} 周 · {user?.department}
        </p>
      </div>

      {urgentIssues.length > 0 && (
        <div className="rounded-lg border border-red-200 bg-red-50 p-4">
          <h3 className="text-sm font-semibold text-red-800">需要关注的问题（{urgentIssues.length}）</h3>
          <div className="mt-2 space-y-1">
            {urgentIssues.map((issue) => (
              <button
                key={issue.id}
                type="button"
                onClick={() => navigate(`/intern/issues/${issue.id}`)}
                className="block w-full rounded bg-white px-3 py-2 text-left text-sm hover:bg-red-100"
              >
                <span
                  className={`mr-2 inline-block rounded-full px-2 py-0.5 text-xs font-medium ${
                    ISSUE_STATUS_COLORS[issue.status as keyof typeof ISSUE_STATUS_COLORS]
                  }`}
                >
                  {ISSUE_STATUS_LABELS[issue.status as keyof typeof ISSUE_STATUS_LABELS]}
                </span>
                <span className="text-gray-800">{issue.title}</span>
              </button>
            ))}
          </div>
        </div>
      )}

      <div className="grid grid-cols-3 gap-3">
        <div className="rounded-lg bg-white p-4 shadow-sm ring-1 ring-gray-200">
          <p className="text-xs text-gray-500">进行中</p>
          <p className="mt-1 text-2xl font-bold text-gray-900">{inProgressTasks.length}</p>
        </div>
        <div className="rounded-lg bg-white p-4 shadow-sm ring-1 ring-gray-200">
          <p className="text-xs text-gray-500">已完成</p>
          <p className="mt-1 text-2xl font-bold text-gray-900">{doneCount}</p>
        </div>
        <div className="rounded-lg bg-white p-4 shadow-sm ring-1 ring-gray-200">
          <p className="text-xs text-gray-500">未解决问题</p>
          <p className="mt-1 text-2xl font-bold text-gray-900">{openIssues.length}</p>
        </div>
      </div>

      <div className="rounded-lg bg-white p-5 shadow-sm ring-1 ring-gray-200">
        <div className="mb-3 flex items-center justify-between">
          <h2 className="text-lg font-semibold text-gray-900">本周核心任务</h2>
          <span className="rounded-full bg-blue-50 px-3 py-1 text-xs font-medium text-blue-700">
            {user?.stage || "入职适应期"}
          </span>
        </div>
        {inProgressTasks.length === 0 ? (
          <p className="text-sm text-gray-400">暂无进行中的任务</p>
        ) : (
          <ul className="space-y-3">
            {inProgressTasks.map((task) => (
              <li key={task.id} className="rounded-md border border-gray-100 px-4 py-3">
                <div className="flex items-start justify-between">
                  <div className="flex-1">
                    <p className="text-sm font-medium text-gray-900">{task.title}</p>
                    <p className="mt-1 text-xs text-gray-500">{task.description}</p>
                    {task.completionCriteria && (
                      <p className="mt-1 text-xs text-blue-600">完成标准: {task.completionCriteria}</p>
                    )}
                  </div>
                  <span className="ml-3 shrink-0 rounded-full bg-blue-50 px-2 py-0.5 text-xs font-medium text-blue-700">
                    进行中
                  </span>
                </div>
              </li>
            ))}
          </ul>
        )}
      </div>

      {user?.nextMentorSyncAt && (
        <div className="rounded-lg bg-white p-4 shadow-sm ring-1 ring-gray-200">
          <p className="text-sm text-gray-500">
            下次 Mentor 沟通:
            <span className="ml-2 font-medium text-gray-900">
              {new Date(user.nextMentorSyncAt).toLocaleString("zh-CN")}
            </span>
          </p>
        </div>
      )}

      <div className="flex gap-3">
        <button
          type="button"
          onClick={() => navigate("/intern/progress")}
          className="flex-1 rounded-lg bg-blue-600 px-4 py-3 text-sm font-medium text-white shadow-sm transition-colors hover:bg-blue-700"
        >
          记录今日进展
        </button>
        <button
          type="button"
          onClick={() => navigate("/intern/issues/new")}
          className="flex-1 rounded-lg bg-blue-600 px-4 py-3 text-sm font-medium text-white shadow-sm transition-colors hover:bg-blue-700"
        >
          遇到卡点？创建问题卡片
        </button>
      </div>
    </div>
  );
}
