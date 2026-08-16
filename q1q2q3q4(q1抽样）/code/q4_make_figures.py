from __future__ import annotations

import csv
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib import font_manager


ROOT = Path(__file__).resolve().parents[1]
Q4_DIR = ROOT / "results" / "q4"
FIG_DIR = Q4_DIR / "figures"
DATA_DIR = Q4_DIR / "figure_data"

SAMPLE_ORDER = [100, 368, 400, 1000]
SAMPLE_POS = list(range(len(SAMPLE_ORDER)))
RATE_ORDER = [0.05, 0.10, 0.20]


def read_csv(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8-sig", newline="") as f:
        return list(csv.DictReader(f))


def write_csv(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames: list[str] = []
    for row in rows:
        for key in row:
            if key not in fieldnames:
                fieldnames.append(key)
    with path.open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def setup_style() -> None:
    zh_fonts = ["SimSun", "Microsoft YaHei", "SimHei", "Noto Sans CJK SC", "Source Han Sans SC"]
    installed = {font.name for font in font_manager.fontManager.ttflist}
    for font in zh_fonts:
        if font in installed:
            plt.rcParams["font.sans-serif"] = [font]
            break
    plt.rcParams["axes.unicode_minus"] = False
    plt.rcParams["figure.dpi"] = 150
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.size"] = 8
    plt.rcParams["axes.titlesize"] = 9
    plt.rcParams["axes.labelsize"] = 8
    plt.rcParams["xtick.labelsize"] = 7
    plt.rcParams["ytick.labelsize"] = 7
    plt.rcParams["legend.fontsize"] = 7
    plt.rcParams["lines.linewidth"] = 1.2
    plt.rcParams["lines.markersize"] = 4.0
    plt.rcParams["axes.linewidth"] = 0.55
    plt.rcParams["axes.edgecolor"] = "#595959"
    plt.rcParams["xtick.color"] = "#333333"
    plt.rcParams["ytick.color"] = "#333333"
    plt.rcParams["text.color"] = "#222222"
    plt.rcParams["axes.labelcolor"] = "#222222"
    plt.rcParams["legend.frameon"] = True
    plt.rcParams["legend.framealpha"] = 0.88
    plt.rcParams["legend.edgecolor"] = "#d0d0d0"


def savefig(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(path, dpi=300, bbox_inches="tight", facecolor="white")
    plt.close()


def percent(value: float) -> str:
    return f"{value * 100:.0f}%"


def build_cp_width_data() -> list[dict]:
    rows = read_csv(Q4_DIR / "q4_q2_ci_inputs.csv") + read_csv(Q4_DIR / "q4_q3_ci_inputs.csv")
    grouped: dict[tuple[int, float], list[float]] = {}
    for row in rows:
        n = int(row["sample_size"])
        rate = round(float(row["nominal_rate_from_table"]), 10)
        if rate not in RATE_ORDER:
            continue
        grouped.setdefault((n, rate), []).append(float(row["ci_width"]))

    out = []
    for rate in RATE_ORDER:
        for n in SAMPLE_ORDER:
            values = grouped[(n, rate)]
            width = sum(values) / len(values)
            out.append(
                {
                    "sample_size": n,
                    "nominal_rate": rate,
                    "nominal_rate_label": percent(rate),
                    "ci_width": width,
                    "ci_width_percent_points": width * 100,
                    "source_row_count": len(values),
                }
            )
    write_csv(DATA_DIR / "q4_fig_cp_width_by_nominal_rate.csv", out)
    return out


def plot_cp_width(rows: list[dict]) -> None:
    colors = {0.05: "#4C9A8A", 0.10: "#C06A2C", 0.20: "#6E64A8"}
    markers = {0.05: "o", 0.10: "s", 0.20: "^"}
    fig, ax = plt.subplots(figsize=(5.2, 3.15))
    for rate in RATE_ORDER:
        data = [row for row in rows if row["nominal_rate"] == rate]
        xs = SAMPLE_POS
        ys = [row["ci_width_percent_points"] for row in data]
        ax.plot(xs, ys, marker=markers[rate], color=colors[rate], label=f"{percent(rate)}")
        for i, (x, y) in enumerate(zip(xs, ys)):
            offset = 7 if i != 2 else -14
            ax.annotate(f"{y:.2f}", (x, y), textcoords="offset points", xytext=(0, offset), ha="center", fontsize=6.5)
    ax.set_xlabel("样本量 n")
    ax.set_ylabel("95% CP区间宽度（百分点）")
    ax.set_xticks(SAMPLE_POS, [str(n) for n in SAMPLE_ORDER])
    ax.grid(axis="y", color="#d8d8d8", linewidth=0.45, alpha=0.9)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.legend(title="名义次品率", title_fontsize=7, loc="upper right", borderpad=0.35, handlelength=1.8)
    savefig(FIG_DIR / "q4_cp_interval_width.png")


def build_q2_case56_data() -> list[dict]:
    rows = read_csv(Q4_DIR / "q4_q2_strategy_comparison.csv")
    out = []
    for row in rows:
        case_id = int(row["case_id"])
        if case_id not in (5, 6):
            continue
        out.append(
            {
                "sample_size": int(row["sample_size"]),
                "case_id": case_id,
                "nominal_best_strategy": row["nominal_best_strategy"],
                "robust_best_strategy": row["robust_best_strategy"],
                "strategy_change_type": row["strategy_change_type"],
                "nominal_best_profit_at_point": float(row["nominal_best_profit_at_point"]),
                "nominal_strategy_worst_profit": float(row["nominal_strategy_worst_profit"]),
                "robust_strategy_worst_profit": float(row["robust_strategy_worst_profit"]),
                "worst_profit_gain": float(row["robust_strategy_worst_profit"]) - float(row["nominal_strategy_worst_profit"]),
                "robust_profit_cost_at_point": float(row["robust_profit_cost_at_point"]),
            }
        )
    out.sort(key=lambda item: (item["case_id"], item["sample_size"]))
    write_csv(DATA_DIR / "q4_fig_q2_case56_profit_by_sample.csv", out)
    return out


def plot_q2_case56(rows: list[dict]) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(6.45, 2.9), sharex=True)
    colors = {"nominal_worst": "#C58A19", "robust_worst": "#7B4A7E"}
    bar_width = 0.34
    for ax, case_id in zip(axes, [5, 6]):
        data = [row for row in rows if row["case_id"] == case_id]
        xs_left = [x - bar_width / 2 for x in SAMPLE_POS]
        xs_right = [x + bar_width / 2 for x in SAMPLE_POS]
        nominal_worst = [row["nominal_strategy_worst_profit"] for row in data]
        robust_worst = [row["robust_strategy_worst_profit"] for row in data]
        gains = [row["worst_profit_gain"] for row in data]
        ax.bar(xs_left, nominal_worst, width=bar_width, color=colors["nominal_worst"], alpha=0.86, label="点估计策略最坏")
        ax.bar(xs_right, robust_worst, width=bar_width, color=colors["robust_worst"], alpha=0.86, label="稳健策略最坏")
        for x, y, gain in zip(xs_right, robust_worst, gains):
            ax.annotate(f"+{gain:.2f}", (x, y), textcoords="offset points", xytext=(0, 4), ha="center", fontsize=6.3)
        ax.set_title(f"情形{case_id}")
        ax.set_xlabel("样本量 n")
        ax.set_xticks(SAMPLE_POS, [str(n) for n in SAMPLE_ORDER])
        ax.grid(axis="y", color="#d8d8d8", linewidth=0.45, alpha=0.9)
        ax.spines["top"].set_visible(False)
        ax.spines["right"].set_visible(False)
        y_min = min(nominal_worst + robust_worst) - 1.3
        y_max = max(nominal_worst + robust_worst) + 1.7
        ax.set_ylim(y_min, y_max)
        ax.text(0.56, 0.06, "四个样本量均为严格切换", transform=ax.transAxes, fontsize=6.5, color="#555555")
    axes[0].set_ylabel("最坏情形期望利润")
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="upper center", ncol=3, bbox_to_anchor=(0.5, 1.04), borderpad=0.3, handlelength=1.6)
    savefig(FIG_DIR / "q4_q2_case56_profit_by_sample.png")


def plot_q2_case56_line(rows: list[dict]) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(6.6, 2.9), sharex=True)
    palette = {
        "nominal": "#3E7C9A",
        "nominal_worst": "#C58A19",
        "robust_worst": "#7B4A7E",
    }
    for ax, case_id in zip(axes, [5, 6]):
        data = [row for row in rows if row["case_id"] == case_id]
        xs = SAMPLE_POS
        nominal = [row["nominal_best_profit_at_point"] for row in data]
        nominal_worst = [row["nominal_strategy_worst_profit"] for row in data]
        robust_worst = [row["robust_strategy_worst_profit"] for row in data]
        ax.plot(xs, nominal, marker="o", color=palette["nominal"], label="点估计最优")
        ax.plot(xs, nominal_worst, marker="s", color=palette["nominal_worst"], label="点估计策略最坏")
        ax.plot(xs, robust_worst, marker="^", color=palette["robust_worst"], label="稳健策略最坏")
        for i, (x, y) in enumerate(zip(xs, robust_worst)):
            offset = 8 if i != 2 else -15
            ax.annotate(f"{y:.2f}", (x, y), textcoords="offset points", xytext=(0, offset), ha="center", fontsize=6.3)
        ax.set_title(f"情形{case_id}")
        ax.set_xlabel("样本量 n")
        ax.set_xticks(SAMPLE_POS, [str(n) for n in SAMPLE_ORDER])
        ax.grid(axis="y", color="#d8d8d8", linewidth=0.45, alpha=0.9)
        ax.spines["top"].set_visible(False)
        ax.spines["right"].set_visible(False)
        y_min = min(nominal_worst + robust_worst) - 1.5
        y_max = max(nominal) + 1.5
        ax.set_ylim(y_min, y_max)
        ax.text(0.56, 0.06, "四个样本量均为严格切换", transform=ax.transAxes, fontsize=6.5, color="#555555")
    axes[0].set_ylabel("期望利润")
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="upper center", ncol=3, bbox_to_anchor=(0.5, 1.04), borderpad=0.3, handlelength=1.6)
    savefig(FIG_DIR / "q4_q2_case56_profit_by_sample_line.png")


def build_q3_data() -> list[dict]:
    rows = read_csv(Q4_DIR / "q4_q3_strategy_comparison.csv")
    out = []
    for row in rows:
        robust_strategy = row["robust_best_strategy"]
        out.append(
            {
                "sample_size": int(row["sample_size"]),
                "nominal_best_strategy_id": int(row["nominal_best_strategy_id"]),
                "robust_best_strategy_id": int(row["robust_best_strategy_id"]),
                "strategy_change_type": row["strategy_change_type"],
                "adds_final_test": "test_final" in robust_strategy and "test_final" not in row["nominal_best_strategy"],
                "nominal_best_profit_at_point": float(row["nominal_best_profit_at_point"]),
                "nominal_strategy_worst_profit": float(row["nominal_strategy_worst_profit"]),
                "robust_strategy_worst_profit": float(row["robust_strategy_worst_profit"]),
                "worst_profit_gain": float(row["robust_strategy_worst_profit"]) - float(row["nominal_strategy_worst_profit"]),
                "robust_profit_cost_at_point": float(row["robust_profit_cost_at_point"]),
            }
        )
    out.sort(key=lambda item: item["sample_size"])
    write_csv(DATA_DIR / "q4_fig_q3_worst_profit_and_switch.csv", out)
    return out


def plot_q3(rows: list[dict]) -> None:
    fig, ax = plt.subplots(figsize=(5.35, 3.15))
    y_positions = list(reversed(SAMPLE_POS))
    all_values = []
    for i, row in enumerate(rows):
        y = y_positions[i]
        x0 = row["nominal_strategy_worst_profit"]
        x1 = row["robust_strategy_worst_profit"]
        all_values.extend([x0, x1])
        ax.plot([x0, x1], [y, y], color="#b9b9b9", linewidth=0.9, zorder=1)
        ax.scatter([x0], [y], marker="s", s=24, color="#A8751C", label="冻结策略最坏利润" if i == 0 else "", zorder=3)
        ax.scatter([x1], [y], marker="o", s=28, color="#3F8777", label="稳健最优最坏利润" if i == 0 else "", zorder=3)
        if abs(x1 - x0) <= 1e-8:
            ax.annotate(f"{x1:.2f}", (x1, y), textcoords="offset points", xytext=(0, 7), ha="center", fontsize=6.3)
        else:
            ax.annotate(f"{x0:.2f}", (x0, y), textcoords="offset points", xytext=(-4, 6), ha="right", fontsize=6.3)
            ax.annotate(f"{x1:.2f}", (x1, y), textcoords="offset points", xytext=(4, 6), ha="left", fontsize=6.3)
        if row["worst_profit_gain"] > 1e-8:
            ax.annotate(
                f"+{row['worst_profit_gain']:.2f}",
                ((x0 + x1) / 2, y),
                textcoords="offset points",
                xytext=(0, -12),
                ha="center",
                fontsize=6.3,
                color="#7A2630",
            )
        if row["adds_final_test"]:
            ax.scatter([x1], [y], s=55, facecolors="none", edgecolors="#B1424C", linewidths=1.0, zorder=4)
            ax.annotate("n=100：增加最终成品检测", (x1, y), textcoords="offset points", xytext=(12, -14), fontsize=6.5, color="#7A2630")
    ax.set_xlabel("最坏情形期望利润")
    ax.set_ylabel("样本量 n")
    ax.set_yticks(y_positions, [str(n) for n in SAMPLE_ORDER])
    ax.set_xlim(min(all_values) - 0.8, max(all_values) + 0.8)
    ax.grid(axis="x", color="#d8d8d8", linewidth=0.45, alpha=0.9)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.legend(loc="lower left", borderpad=0.35, handlelength=1.8)
    savefig(FIG_DIR / "q4_q3_worst_profit_and_switch.png")


def plot_q3_line(rows: list[dict]) -> None:
    xs = SAMPLE_POS
    nominal_worst = [row["nominal_strategy_worst_profit"] for row in rows]
    robust_worst = [row["robust_strategy_worst_profit"] for row in rows]
    fig, ax = plt.subplots(figsize=(5.35, 3.2))
    ax.plot(xs, nominal_worst, marker="s", color="#A8751C", label="冻结策略最坏利润")
    ax.plot(xs, robust_worst, marker="o", color="#3F8777", label="稳健最优最坏利润")
    for i, row in enumerate(rows):
        x = SAMPLE_POS[i]
        y = row["robust_strategy_worst_profit"]
        if abs(row["robust_strategy_worst_profit"] - row["nominal_strategy_worst_profit"]) <= 1e-8:
            offset = 7 if i != 2 else -15
            ax.annotate(f"{y:.2f}", (x, y), textcoords="offset points", xytext=(0, offset), ha="center", fontsize=6.5)
        else:
            ax.annotate(
                f"{row['nominal_strategy_worst_profit']:.2f}",
                (x, row["nominal_strategy_worst_profit"]),
                textcoords="offset points",
                xytext=(-10, -2),
                ha="right",
                fontsize=6.3,
            )
            ax.annotate(f"{y:.2f}", (x, y), textcoords="offset points", xytext=(7, 7), ha="left", fontsize=6.5)
        if row["adds_final_test"]:
            ax.scatter([x], [y], s=45, facecolors="none", edgecolors="#B1424C", linewidths=1.0, zorder=4)
            ax.annotate("n=100：增加最终成品检测", (x, y), textcoords="offset points", xytext=(14, -15), fontsize=6.5, color="#7A2630")
    ax.set_xlabel("样本量 n")
    ax.set_ylabel("最坏情形期望利润")
    ax.set_xticks(SAMPLE_POS, [str(n) for n in SAMPLE_ORDER])
    ax.set_ylim(min(nominal_worst + robust_worst) - 0.9, max(nominal_worst + robust_worst) + 0.8)
    ax.grid(axis="y", color="#d8d8d8", linewidth=0.45, alpha=0.9)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.legend(loc="lower right", borderpad=0.35, handlelength=1.8)
    savefig(FIG_DIR / "q4_q3_worst_profit_and_switch_line.png")


def main() -> None:
    FIG_DIR.mkdir(parents=True, exist_ok=True)
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    setup_style()
    cp_rows = build_cp_width_data()
    plot_cp_width(cp_rows)
    q2_rows = build_q2_case56_data()
    plot_q2_case56(q2_rows)
    plot_q2_case56_line(q2_rows)
    q3_rows = build_q3_data()
    plot_q3(q3_rows)
    plot_q3_line(q3_rows)
    print(
        {
            "figures": [
                str(FIG_DIR / "q4_cp_interval_width.png"),
                str(FIG_DIR / "q4_q2_case56_profit_by_sample.png"),
                str(FIG_DIR / "q4_q2_case56_profit_by_sample_line.png"),
                str(FIG_DIR / "q4_q3_worst_profit_and_switch.png"),
                str(FIG_DIR / "q4_q3_worst_profit_and_switch_line.png"),
            ],
            "csv": [
                str(DATA_DIR / "q4_fig_cp_width_by_nominal_rate.csv"),
                str(DATA_DIR / "q4_fig_q2_case56_profit_by_sample.csv"),
                str(DATA_DIR / "q4_fig_q3_worst_profit_and_switch.csv"),
            ],
        }
    )


if __name__ == "__main__":
    main()
