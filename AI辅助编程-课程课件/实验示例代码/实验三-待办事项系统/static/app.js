// 实验三 · 待办事项系统前端逻辑（原生 JS + fetch）
const api = {
  list: (status) => fetch(`/api/todos?status=${status}`).then(r => r.json()),
  add: (title) => send("/api/todos", "POST", { title }),
  toggle: (id) => send(`/api/todos/${id}`, "PUT", { toggle: true }),
  remove: (id) => fetch(`/api/todos/${id}`, { method: "DELETE" }),
  clearDone: async () => {
    const done = await api.list("completed");
    await Promise.all(done.map(t => api.remove(t.id)));
  },
};

async function send(url, method, body) {
  const res = await fetch(url, {
    method,
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  const data = await res.json().catch(() => ({}));
  if (!res.ok) throw new Error(data.error || `请求失败(${res.status})`);
  return data;
}

let currentStatus = "all";

const els = {
  form: document.getElementById("add-form"),
  input: document.getElementById("title-input"),
  list: document.getElementById("list"),
  filters: document.getElementById("filters"),
  stats: document.getElementById("stats"),
  clear: document.getElementById("clear-done"),
  error: document.getElementById("error"),
};

function showError(msg) {
  els.error.textContent = msg;
  els.error.hidden = !msg;
}

function render(todos) {
  els.list.innerHTML = "";
  if (todos.length === 0) {
    const li = document.createElement("li");
    li.className = "empty";
    li.textContent = "暂无待办，添加一个吧 ✦";
    els.list.appendChild(li);
    return;
  }
  for (const t of todos) {
    const li = document.createElement("li");
    li.className = "item" + (t.done ? " is-done" : "");

    const check = document.createElement("input");
    check.type = "checkbox";
    check.className = "item__check";
    check.checked = t.done;
    check.addEventListener("change", () => withRefresh(() => api.toggle(t.id)));

    const title = document.createElement("span");
    title.className = "item__title";
    title.textContent = t.title;

    const time = document.createElement("span");
    time.className = "item__time";
    time.textContent = (t.created_at || "").replace("T", " ");

    const del = document.createElement("button");
    del.className = "item__del";
    del.textContent = "×";
    del.title = "删除";
    del.addEventListener("click", () => withRefresh(() => api.remove(t.id)));

    li.append(check, title, time, del);
    els.list.appendChild(li);
  }
}

async function refresh() {
  showError("");
  try {
    const [todos, stats] = await Promise.all([
      api.list(currentStatus),
      fetch("/api/stats").then(r => r.json()),
    ]);
    render(todos);
    els.stats.textContent = `共 ${stats.total} 项 · 进行中 ${stats.active} · 已完成 ${stats.completed}`;
  } catch (e) {
    showError(e.message);
  }
}

async function withRefresh(action) {
  try {
    await action();
    await refresh();
  } catch (e) {
    showError(e.message);
  }
}

els.form.addEventListener("submit", (e) => {
  e.preventDefault();
  const title = els.input.value.trim();
  if (!title) return;
  withRefresh(async () => {
    await api.add(title);
    els.input.value = "";
  });
});

els.filters.addEventListener("click", (e) => {
  const btn = e.target.closest(".filters__btn");
  if (!btn) return;
  currentStatus = btn.dataset.status;
  [...els.filters.children].forEach(b => b.classList.toggle("is-active", b === btn));
  refresh();
});

els.clear.addEventListener("click", () => withRefresh(api.clearDone));

refresh();
