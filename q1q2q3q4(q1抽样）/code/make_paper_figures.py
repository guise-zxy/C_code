from __future__ import annotations

import csv
import json
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib import font_manager


ROOT = Path(__file__).resolve().parents[1]
RESULTS = ROOT / "results"
OUT_DIR = RESULTS / "paper_figures"
FIG_DIR = OUT_DIR / "png"
DATA_DIR = OUT_DIR / "csv"

PALETTE = {
    "teal": "#4C9A8A",
    "orange": "#C06A2C",
    "purple": "#6E64A8",
    "gold": "#C58A19",
    "wine": "#B1424C",
    "bluegray": "#3E7C9A",
    "gray": "#777777",
}


def read_csv(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8-sig", newline="") as f:
        return list(csv.DictReader(f))


def write_csv(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    if not rows:
        raise ValueError(f"empty rows for {path}")
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
    plt.rcParams["legend.frameon"] = True
    plt.rcParams["legend.framealpha"] = 0.88
    plt.rcParams["legend.edgecolor"] = "#d0d0d0"


def savefig(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(path, dpi=300, bbox_inches="tight", facecolor="white")
    plt.close()


def plot_q1_probability_curve() -> None:
    rows = read_csv(RESULTS / "q1" / "q1_probability_curve.csv")
    out = []
    for row in rows:
        p = float(row["p"])
        if 0.02 <= p <= 0.22:
            out.append(
                {
                    "p": p,
                    "p_percent": p * 100,
                    "accept_prob": float(row["accept_prob"]),
                    "reject_prob": float(row["reject_prob"]),
                    "inconclusive_prob": float(row["inconclusive_prob"]),
                }
            )
    write_csv(DATA_DIR / "paper_fig_q1_fixed_probability_curve.csv", out)

    fig, ax = plt.subplots(figsize=(5.25, 3.15))
    xs = [row["p_percent"] for row in out]
    ax.plot(xs, [row["accept_prob"] for row in out], color=PALETTE["teal"], label="接收概率")
    ax.plot(xs, [row["reject_prob"] for row in out], color=PALETTE["wine"], label="拒收概率")
    ax.plot(xs, [row["inconclusive_prob"] for row in out], color=PALETTE["gray"], linestyle="--", label="未决概率")
    for x, label in [(10, "p=10%"), (15, "p=15%")]:
        ax.axvline(x, color="#bdbdbd", linewidth=0.65)
        ax.text(x + 0.3, 0.05, label, fontsize=6.5, color="#555555")
    ax.set_xlabel("真实次品率 p（%）")
    ax.set_ylabel("概率")
    ax.set_ylim(-0.02, 1.02)
    ax.grid(axis="y", color="#d8d8d8", linewidth=0.45, alpha=0.9)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.legend(loc="center right", borderpad=0.35, handlelength=1.8)
    savefig(FIG_DIR / "paper_fig_q1_fixed_probability_curve.png")


def plot_q2_action_matrix() -> None:
    rows = [
        row
        for row in read_csv(RESULTS / "q2" / "q2_static_best_strategies.csv")
        if row["inspection_policy"] == "no_retest_known_good"
    ]
    rows.sort(key=lambda row: int(row["case_id"]))
    actions = [
        ("d1_test_part1", "检零件1"),
        ("d2_test_part2", "检零件2"),
        ("df_test_final", "检成品"),
        ("dr_disassemble", "拆解"),
    ]
    out = []
    matrix = []
    for row in rows:
        values = [int(row[key]) for key, _ in actions]
        matrix.append(values)
        out.append({"case_id": int(row["case_id"]), **{label: value for value, (_, label) in zip(values, actions)}})
    write_csv(DATA_DIR / "paper_fig_q2_action_matrix.csv", out)

    fig, ax = plt.subplots(figsize=(4.9, 2.65))
    image = ax.imshow(matrix, cmap=plt.matplotlib.colors.ListedColormap(["#f7f2e9", PALETTE["teal"]]), vmin=0, vmax=1, aspect="auto")
    ax.set_xticks(range(len(actions)), [label for _, label in actions])
    ax.set_yticks(range(len(rows)), [f"情形{row['case_id']}" for row in rows])
    for i, values in enumerate(matrix):
        for j, value in enumerate(values):
            ax.text(j, i, "是" if value else "否", ha="center", va="center", fontsize=7, color="#222222")
    ax.tick_params(length=0)
    for spine in ax.spines.values():
        spine.set_visible(False)
    cbar = fig.colorbar(image, ax=ax, fraction=0.035, pad=0.03, ticks=[0, 1])
    cbar.ax.set_yticklabels(["不执行", "执行"])
    savefig(FIG_DIR / "paper_fig_q2_action_matrix.png")


def plot_q2_switching_curves() -> None:
    case5 = read_csv(RESULTS / "q2" / "q2_sensitivity_case5_exchange_loss.csv")
    case6 = read_csv(RESULTS / "q2" / "q2_sensitivity_case6_synced_defect_rate.csv")
    rows = []
    for row in case5:
        rows.append(
            {
                "panel": "case5_exchange_loss",
                "x": float(row["exchange_loss"]),
                "strategy_a_profit": float(row["profit_no_final_test_strategy_0100"]),
                "strategy_b_profit": float(row["profit_final_test_strategy_0110"]),
                "profit_diff": float(row["profit_diff_no_final_minus_final"]),
            }
        )
    for row in case6:
        rows.append(
            {
                "panel": "case6_synced_defect_rate",
                "x": float(row["p1_p2_pf"]) * 100,
                "strategy_a_profit": float(row["profit_no_test_strategy_0000"]),
                "strategy_b_profit": float(row["profit_test_part1_strategy_1000"]),
                "profit_diff": float(row["profit_diff_test_part1_minus_no_test"]),
            }
        )
    write_csv(DATA_DIR / "paper_fig_q2_switching_curves.csv", rows)

    fig, axes = plt.subplots(1, 2, figsize=(6.5, 2.85))
    specs = [
        ("case5_exchange_loss", "情形5：调换损失", "调换损失", "不检成品策略", "检成品策略", 10.5263157895),
        ("case6_synced_defect_rate", "情形6：同步次品率", "三类次品率（%）", "不检测策略", "检零件1策略", 5.882352941),
    ]
    for ax, (panel, title, xlabel, label_a, label_b, threshold) in zip(axes, specs):
        data = [row for row in rows if row["panel"] == panel]
        xs = [row["x"] for row in data]
        ax.plot(xs, [row["strategy_a_profit"] for row in data], marker="o", color=PALETTE["gold"], label=label_a)
        ax.plot(xs, [row["strategy_b_profit"] for row in data], marker="s", color=PALETTE["purple"], label=label_b)
        ax.axvline(threshold, color=PALETTE["wine"], linestyle="--", linewidth=0.8)
        ax.text(threshold, min(min(row["strategy_a_profit"], row["strategy_b_profit"]) for row in data), f"阈值 {threshold:.2f}", rotation=90, va="bottom", ha="right", fontsize=6.3, color="#7A2630")
        ax.set_title(title)
        ax.set_xlabel(xlabel)
        ax.grid(axis="y", color="#d8d8d8", linewidth=0.45, alpha=0.9)
        ax.spines["top"].set_visible(False)
        ax.spines["right"].set_visible(False)
    axes[0].set_ylabel("期望利润")
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="upper center", ncol=2, bbox_to_anchor=(0.5, 1.04), borderpad=0.3, handlelength=1.6)
    savefig(FIG_DIR / "paper_fig_q2_switching_curves.png")


def plot_q3_cost_composition() -> None:
    summary = json.loads((RESULTS / "q3" / "q3_summary_reuse.json").read_text(encoding="utf-8"))
    best = summary["best_strategy"]
    mapping = [
        ("purchase_cost", "采购"),
        ("part_test_cost", "零件检测"),
        ("assembly_cost", "装配"),
        ("product_test_cost", "产品检测"),
        ("disassembly_cost", "拆解"),
        ("exchange_loss_cost", "调换损失"),
    ]
    rows = [
        {"cost_item": label, "expected_cost": best[key], "share": best[key] / best["expected_cost"]}
        for key, label in mapping
    ]
    write_csv(DATA_DIR / "paper_fig_q3_cost_composition.csv", rows)

    fig, ax = plt.subplots(figsize=(4.8, 2.85))
    labels = [row["cost_item"] for row in rows]
    values = [row["expected_cost"] for row in rows]
    colors = [PALETTE["teal"], PALETTE["orange"], PALETTE["purple"], PALETTE["bluegray"], PALETTE["gold"], PALETTE["wine"]]
    bars = ax.bar(labels, values, color=colors, alpha=0.86)
    for bar, value in zip(bars, values):
        ax.text(bar.get_x() + bar.get_width() / 2, value + 1.0, f"{value:.1f}", ha="center", fontsize=6.5)
    ax.set_ylabel("期望成本")
    ax.grid(axis="y", color="#d8d8d8", linewidth=0.45, alpha=0.9)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    savefig(FIG_DIR / "paper_fig_q3_cost_composition.png")


def plot_q4_nominal_robust_comparison() -> None:
    q2_rows = [
        row for row in read_csv(RESULTS / "q4" / "q4_q2_strategy_comparison.csv") if int(row["sample_size"]) == 368
    ]
    q3_rows = [
        row for row in read_csv(RESULTS / "q4" / "q4_q3_strategy_comparison.csv") if int(row["sample_size"]) == 368
    ]
    out = []
    for row in q2_rows:
        out.append(
            {
                "scenario": f"Q2-{row['case_id']}",
                "nominal_strategy_worst_profit": float(row["nominal_strategy_worst_profit"]),
                "robust_strategy_worst_profit": float(row["robust_strategy_worst_profit"]),
                "change_type": row["strategy_change_type"],
            }
        )
    for row in q3_rows:
        out.append(
            {
                "scenario": "Q3",
                "nominal_strategy_worst_profit": float(row["nominal_strategy_worst_profit"]),
                "robust_strategy_worst_profit": float(row["robust_strategy_worst_profit"]),
                "change_type": row["strategy_change_type"],
            }
        )
    write_csv(DATA_DIR / "paper_fig_q4_nominal_robust_comparison.csv", out)

    fig, ax = plt.subplots(figsize=(6.2, 3.05))
    xs = list(range(len(out)))
    width = 0.34
    ax.bar([x - width / 2 for x in xs], [row["nominal_strategy_worst_profit"] for row in out], width=width, color=PALETTE["gold"], alpha=0.86, label="名义策略最坏利润")
    ax.bar([x + width / 2 for x in xs], [row["robust_strategy_worst_profit"] for row in out], width=width, color=PALETTE["purple"], alpha=0.86, label="稳健策略最坏利润")
    for x, row in zip(xs, out):
        if row["change_type"] == "strict_switch":
            ax.text(x, max(row["nominal_strategy_worst_profit"], row["robust_strategy_worst_profit"]) + 0.7, "切换", ha="center", fontsize=6.3, color="#7A2630")
    ax.set_xticks(xs, [row["scenario"] for row in out])
    ax.set_ylabel("n=368 最坏情形期望利润")
    ax.grid(axis="y", color="#d8d8d8", linewidth=0.45, alpha=0.9)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.legend(loc="upper left", borderpad=0.35, handlelength=1.6)
    savefig(FIG_DIR / "paper_fig_q4_nominal_robust_comparison.png")


def main() -> None:
    FIG_DIR.mkdir(parents=True, exist_ok=True)
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    setup_style()
    plot_q1_probability_curve()
    plot_q2_action_matrix()
    plot_q2_switching_curves()
    plot_q3_cost_composition()
    plot_q4_nominal_robust_comparison()
    print(
        json.dumps(
            {
                "figures": [str(path) for path in sorted(FIG_DIR.glob("*.png"))],
                "csv": [str(path) for path in sorted(DATA_DIR.glob("*.csv"))],
            },
            ensure_ascii=False,
            indent=2,
        )
    )


if __name__ == "__main__":
    main()
