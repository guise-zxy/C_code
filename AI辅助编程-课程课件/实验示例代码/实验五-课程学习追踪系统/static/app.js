// 实验五 · 课程学习追踪系统前端（原生 JS + fetch）
const J = (r) => r.json();
async function send(url, method, body) {
  const res = await fetch(url, {
    method, headers: { "Content-Type": "application/json" },
    body: body ? JSON.stringify(body) : undefined,
  });
  const data = await res.json().catch(() => ({}));
  if (!res.ok) throw new Error(data.error || `请求失败(${res.status})`);
  return data;
}

const els = {
  stats: document.getElementById("stats"),
  reminders: document.getElementById("reminders"),
  courses: document.getElementById("courses"),
  form: document.getElementById("course-form"),
  name: document.getElementById("c-name"),
  hours: document.getElementById("c-hours"),
  error: document.getElementById("error"),
};

function err(msg) { els.error.textContent = msg || ""; els.error.hidden = !msg; }

function renderStats(s) {
  const cards = [
    [s.course_count, "课程数"],
    [s.total_hours, "累计学时"],
    [(s.today_minutes / 60).toFixed(1), "今日学时"],
    [s.completed_courses, "已达标"],
  ];
  els.stats.innerHTML = cards.map(([n, l]) =>
    `<div class="stat"><div class="stat__num">${n}</div><div class="stat__lab">${l}</div></div>`
  ).join("");
}

function renderReminders(list) {
  if (!list.length) { els.reminders.hidden = true; return; }
  els.reminders.hidden = false;
  els.reminders.innerHTML =
    `<h3>⏰ 学习提醒</h3><ul>` +
    list.map(c => `<li>${escapeHtml(c.name)} —— ${c.reason}（进度 ${c.progress_percent}%）</li>`).join("") +
    `</ul>`;
}

async function renderCourses(list) {
  if (!list.length) {
    els.courses.innerHTML = `<p class="muted">还没有课程，先在上方添加一个吧。</p>`;
    return;
  }
  els.courses.innerHTML = "";
  for (const c of list) {
    const div = document.createElement("div");
    div.className = "course";
    div.innerHTML = `
      <div class="course__top">
        <span class="course__name ${c.completed ? "is-done" : ""}">${escapeHtml(c.name)}</span>
        <button class="course__del" title="删除">×</button>
      </div>
      <div class="bar"><div class="bar__fill" style="width:${c.progress_percent}%"></div></div>
      <div class="course__meta">
        <span>${c.studied_hours} / ${c.target_hours} 学时</span>
        <span>${c.progress_percent}%${c.last_study ? " · 最近 " + c.last_study : " · 未开始"}</span>
      </div>
      <div class="logbar">
        <input type="number" min="1" placeholder="分钟" class="mins" />
        <input type="text" placeholder="备注（可选）" class="note" />
        <button class="add-log">记录学习</button>
      </div>
      <div class="records"></div>`;

    div.querySelector(".course__del").addEventListener("click",
      () => guard(async () => { await fetch(`/api/courses/${c.id}`, { method: "DELETE" }); await refresh(); }));

    const mins = div.querySelector(".mins");
    const note = div.querySelector(".note");
    div.querySelector(".add-log").addEventListener("click", () => guard(async () => {
      const v = parseInt(mins.value, 10);
      if (!v || v <= 0) throw new Error("请输入正整数分钟数");
      await send(`/api/courses/${c.id}/records`, "POST", { minutes: v, note: note.value });
      await refresh();
    }));

    // 展示最近 3 条记录
    const recs = await fetch(`/api/courses/${c.id}/records`).then(J);
    div.querySelector(".records").innerHTML =
      recs.slice(0, 3).map(r => `<span>· ${r.study_date}　${r.minutes} 分钟${r.note ? "　" + escapeHtml(r.note) : ""}</span>`).join("");

    els.courses.appendChild(div);
  }
}

async function refresh() {
  err("");
  try {
    const [stats, reminders, courses] = await Promise.all([
      fetch("/api/stats").then(J),
      fetch("/api/reminders").then(J),
      fetch("/api/courses").then(J),
    ]);
    renderStats(stats);
    renderReminders(reminders);
    await renderCourses(courses);
  } catch (e) { err(e.message); }
}

async function guard(fn) { try { await fn(); } catch (e) { err(e.message); } }

function escapeHtml(s) {
  return String(s).replace(/[&<>"']/g, m =>
    ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[m]));
}

els.form.addEventListener("submit", (e) => {
  e.preventDefault();
  guard(async () => {
    const name = els.name.value.trim();
    const hours = parseFloat(els.hours.value);
    if (!name) throw new Error("请输入课程名称");
    if (!hours || hours <= 0) throw new Error("请输入正确的目标学时");
    await send("/api/courses", "POST", { name, target_hours: hours });
    els.name.value = ""; els.hours.value = "";
    await refresh();
  });
});

refresh();
