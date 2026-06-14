import { useEffect, useState } from "react";
import { useAuth } from "../../contexts/AuthContext";
import { listInternTasks } from "../../services/taskService";
import type { Task } from "../../types";
import {
  TASK_PRIORITY_COLORS,
  TASK_PRIORITY_LABELS,
  TASK_STATUS_COLORS,
  TASK_STATUS_LABELS,
} from "../../types";

function formatDateTime(value?: string) {
  if (!value) return "未设置";
  const date = new Date(value);
  if (Number.isNaN(date.getTime())) return "未设置";
  return date.toLocaleString("zh-CN", {
    year: "numeric",
    month: "2-digit",
    day: "2-digit",
    hour: "2-digit",
    minute: "2-digit",
  });
}

export default function InternTasks() {
  const { user } = useAuth();
  const [tasks, setTasks] = useState<Task[]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    if (!user) return;

    const load = async () => {
      try {
        const taskList = await listInternTasks(user.id);
        setTasks(taskList);
      } catch (err) {
        console.error("加载任务失败:", err);
      } finally {
        setLoading(false);
      }
    };

    void load();
  }, [user]);

  if (loading) return <div className="text-sm text-gray-500">加载中...</div>;

  return (
    <div className="space-y-4">
      <h1 className="text-xl font-bold text-gray-900">我的任务</h1>
      {tasks.length === 0 ? (
        <p className="text-sm text-gray-400">暂无任务</p>
      ) : (
        <div className="space-y-3">
          {tasks.map((task) => {
            const priority = task.priority || "medium";
            return (
              <div key={task.id} className="rounded-lg bg-white p-4 shadow-sm ring-1 ring-gray-200">
                <div className="flex flex-col gap-3 sm:flex-row sm:items-start sm:justify-between">
                  <div className="min-w-0 flex-1">
                    <div className="flex flex-wrap items-center gap-2">
                      <h3 className="text-sm font-semibold text-gray-900">{task.title}</h3>
                      <span className={`rounded-full px-2 py-0.5 text-xs font-medium ${TASK_PRIORITY_COLORS[priority]}`}>
                        优先级 {TASK_PRIORITY_LABELS[priority]}
                      </span>
                      <span className={`rounded-full px-2 py-0.5 text-xs font-medium ${TASK_STATUS_COLORS[task.status]}`}>
                        {TASK_STATUS_LABELS[task.status]}
                      </span>
                    </div>
                    <p className="mt-2 text-sm text-gray-600">{task.description}</p>
                    <p className="mt-2 text-xs text-gray-500">完成标准：{task.completionCriteria}</p>
                  </div>
                  <div className="shrink-0 text-xs text-gray-500">
                    截止时间：{formatDateTime(task.dueDate)}
                  </div>
                </div>
              </div>
            );
          })}
        </div>
      )}
    </div>
  );
}
