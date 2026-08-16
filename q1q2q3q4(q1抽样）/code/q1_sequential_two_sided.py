from __future__ import annotations

import csv
import json
import math
from pathlib import Path


P0 = 0.10
P_BAD = 0.15
N_MAX = 368
FIXED_N = 368

FINAL_ACCEPT_BOUNDARY = 29
FINAL_REJECT_BOUNDARY = 47

BASELINE_MIN_EARLY_REJECT_N = 180
BASELINE_LOG_LR_REJECT = 4.4

INITIAL_ACCEPT_BOUNDARY = -8.94
TYPICAL_P = [0.05, 0.08, 0.10, 0.12, 0.15, 0.20]


def log_lr(n: int, k: int, p0: float = P0, p_bad: float = P_BAD) -> float:
    return k * math.log(p_bad / p0) + (n - k) * math.log((1.0 - p_bad) / (1.0 - p0))


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


def make_boundaries(
    log_lr_accept: float | None,
    log_lr_reject: float,
    min_early_reject_n: int = BASELINE_MIN_EARLY_REJECT_N,
) -> list[dict]:
    rows = []
    for n in range(1, N_MAX + 1):
        accept_boundary = -1
        reject_boundary = n + 1
        if n < N_MAX and log_lr_accept is not None:
            for k in range(n + 1):
                if log_lr(n, k) <= log_lr_accept:
                    accept_boundary = k
                else:
                    break
        if n < N_MAX and n >= min_early_reject_n:
            for k in range(n + 1):
                if log_lr(n, k) >= log_lr_reject:
                    reject_boundary = k
                    break
        if n == N_MAX:
            accept_boundary = FINAL_ACCEPT_BOUNDARY
            reject_boundary = FINAL_REJECT_BOUNDARY
        if accept_boundary >= reject_boundary:
            raise RuntimeError(f"overlapping boundaries at n={n}")
        continue_low = accept_boundary + 1
        continue_high = reject_boundary - 1
        rows.append(
            {
                "stage_n": n,
                "log_lr_accept": "" if log_lr_accept is None else log_lr_accept,
                "log_lr_reject": log_lr_reject,
                "accept_if_defects_le": "" if accept_boundary < 0 else accept_boundary,
                "reject_if_defects_ge": "" if reject_boundary > n else reject_boundary,
                "continue_defects_from": continue_low if continue_low <= continue_high else "",
                "continue_defects_to": continue_high if continue_low <= continue_high else "",
            }
        )
    return rows


def boundary_pair(row: dict) -> tuple[int, int]:
    n = int(row["stage_n"])
    a = -1 if row["accept_if_defects_le"] == "" else int(row["accept_if_defects_le"])
    r = n + 1 if row["reject_if_defects_ge"] == "" else int(row["reject_if_defects_ge"])
    return a, r


def evaluate(boundaries: list[dict], p: float) -> dict:
    alive = [1.0]
    accept_prob = 0.0
    reject_prob = 0.0
    expected_n = 0.0
    q = 1.0 - p

    for n, row in enumerate(boundaries, 1):
        a, r = boundary_pair(row)
        next_dist = [0.0] * (n + 1)
        for k, prob in enumerate(alive):
            if prob == 0.0:
                continue
            next_dist[k] += prob * q
            next_dist[k + 1] += prob * p

        alive = [0.0] * (n + 1)
        for k, prob in enumerate(next_dist):
            if prob == 0.0:
                continue
            if k <= a:
                accept_prob += prob
                expected_n += n * prob
            elif k >= r:
                reject_prob += prob
                expected_n += n * prob
            else:
                alive[k] = prob

    undecided_prob = math.fsum(alive)
    expected_n += N_MAX * undecided_prob
    return {
        "p": p,
        "accept_prob": accept_prob,
        "reject_prob": reject_prob,
        "undecided_prob": undecided_prob,
        "asn": expected_n,
        "fixed_n": FIXED_N,
        "asn_reduction": FIXED_N - expected_n,
        "asn_reduction_rate": (FIXED_N - expected_n) / FIXED_N,
        "probability_sum_error": abs(accept_prob + reject_prob + undecided_prob - 1.0),
    }


def validate(boundaries: list[dict]) -> dict:
    p0 = evaluate(boundaries, P0)
    pbad = evaluate(boundaries, P_BAD)
    return {
        "p0_accept_prob": p0["accept_prob"],
        "p0_reject_prob": p0["reject_prob"],
        "pbad_reject_prob": pbad["reject_prob"],
        "p0_accept_prob_le_0.10": p0["accept_prob"] <= 0.10 + 1e-12,
        "p0_reject_prob_le_0.05": p0["reject_prob"] <= 0.05 + 1e-12,
        "pbad_reject_prob_ge_0.90": pbad["reject_prob"] >= 0.90 - 1e-12,
    }


def feasible(boundaries: list[dict]) -> bool:
    v = validate(boundaries)
    return v["p0_accept_prob_le_0.10"] and v["p0_reject_prob_le_0.05"] and v["pbad_reject_prob_ge_0.90"]


def fixed_sample_result(p: float) -> dict:
    boundaries = make_boundaries(None, 1e9)
    return evaluate(boundaries, p)


def score(boundaries: list[dict]) -> float:
    # Prioritize good lots; lightly penalize losing the high-defect early rejection benefit.
    return (
        0.35 * evaluate(boundaries, 0.05)["asn"]
        + 0.30 * evaluate(boundaries, 0.08)["asn"]
        + 0.25 * evaluate(boundaries, 0.10)["asn"]
        + 0.10 * evaluate(boundaries, 0.15)["asn"]
    )


def scan_candidates() -> list[dict]:
    rows = []
    for a_i in range(-980, -779, 2):
        accept_boundary = a_i / 100
        for b_i in range(420, 463, 2):
            reject_boundary = b_i / 100
            boundaries = make_boundaries(accept_boundary, reject_boundary)
            validation = validate(boundaries)
            if not (validation["p0_reject_prob_le_0.05"] and validation["p0_accept_prob_le_0.10"] and validation["pbad_reject_prob_ge_0.90"]):
                continue
            p05 = evaluate(boundaries, 0.05)
            p08 = evaluate(boundaries, 0.08)
            p10 = evaluate(boundaries, 0.10)
            p15 = evaluate(boundaries, 0.15)
            p20 = evaluate(boundaries, 0.20)
            rows.append(
                {
                    "log_lr_accept": accept_boundary,
                    "log_lr_reject": reject_boundary,
                    "score": 0.35 * p05["asn"] + 0.30 * p08["asn"] + 0.25 * p10["asn"] + 0.10 * p15["asn"],
                    "asn_p05": p05["asn"],
                    "asn_p08": p08["asn"],
                    "asn_p10": p10["asn"],
                    "asn_p15": p15["asn"],
                    "asn_p20": p20["asn"],
                    **validation,
                }
            )
    rows.sort(key=lambda row: row["score"])
    return rows


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    out_dir = root / "results" / "q1"
    baseline_boundaries = make_boundaries(None, BASELINE_LOG_LR_REJECT)
    initial_boundaries = make_boundaries(INITIAL_ACCEPT_BOUNDARY, BASELINE_LOG_LR_REJECT)

    initial_validation = validate(initial_boundaries)
    candidates = scan_candidates()
    if not candidates:
        raise RuntimeError("no feasible two-sided sequential candidate found in the local search window")

    best = candidates[0]
    best_boundaries = make_boundaries(best["log_lr_accept"], best["log_lr_reject"])

    oc_rows = []
    compare_rows = []
    for p in TYPICAL_P:
        fixed = fixed_sample_result(p)
        one_sided = evaluate(baseline_boundaries, p)
        two_sided = evaluate(best_boundaries, p)
        oc_rows.append(
            {
                "p": p,
                "accept_prob": two_sided["accept_prob"],
                "reject_prob": two_sided["reject_prob"],
                "undecided_prob": two_sided["undecided_prob"],
                "asn": two_sided["asn"],
                "asn_reduction_vs_fixed": two_sided["asn_reduction"],
                "asn_reduction_rate_vs_fixed": two_sided["asn_reduction_rate"],
                "asn_improvement_vs_one_sided": one_sided["asn"] - two_sided["asn"],
            }
        )
        for name, result in [("fixed_368", fixed), ("one_sided_early_reject", one_sided), ("two_sided_early_accept_reject", two_sided)]:
            compare_rows.append(
                {
                    "p": p,
                    "scheme": name,
                    "accept_prob": result["accept_prob"],
                    "reject_prob": result["reject_prob"],
                    "undecided_prob": result["undecided_prob"],
                    "asn": result["asn"],
                    "asn_reduction_rate_vs_fixed": result["asn_reduction_rate"],
                    "asn_improvement_vs_one_sided": "" if name == "fixed_368" else one_sided["asn"] - result["asn"],
                }
            )

    boundary_plot_rows = []
    for row in best_boundaries:
        boundary_plot_rows.append(
            {
                "stage_n": row["stage_n"],
                "accept_if_defects_le": row["accept_if_defects_le"],
                "reject_if_defects_ge": row["reject_if_defects_ge"],
            }
        )

    asn_plot_rows = []
    for i in range(31):
        p = i / 100
        one_sided = evaluate(baseline_boundaries, p)
        two_sided = evaluate(best_boundaries, p)
        asn_plot_rows.append(
            {
                "p": p,
                "p_percent": p * 100,
                "fixed_368_asn": FIXED_N,
                "one_sided_asn": one_sided["asn"],
                "two_sided_asn": two_sided["asn"],
                "two_sided_improvement_vs_one_sided": one_sided["asn"] - two_sided["asn"],
            }
        )

    summary = {
        "model_scope": "local two-sided sequential sampling attempt; fixed Q1 baseline and Q2-Q4 remain unchanged",
        "initial_test": {
            "log_lr_accept": INITIAL_ACCEPT_BOUNDARY,
            "log_lr_reject": BASELINE_LOG_LR_REJECT,
            "validation": initial_validation,
        },
        "selected_candidate": best,
        "validation": validate(best_boundaries),
        "interpretation": "Kept only if validation passes and ASN improves around p=5%, 8%, and 10% without materially losing the p=15% and p=20% early rejection benefit.",
    }

    write_csv(out_dir / "q1_sequential_two_sided_boundaries.csv", best_boundaries)
    write_csv(out_dir / "q1_sequential_two_sided_operating_characteristics.csv", oc_rows)
    write_csv(out_dir / "q1_sequential_scheme_comparison.csv", compare_rows)
    write_csv(out_dir / "q1_sequential_two_sided_boundary_plot_data.csv", boundary_plot_rows)
    write_csv(out_dir / "q1_sequential_two_sided_asn_plot_data.csv", asn_plot_rows)
    write_csv(out_dir / "q1_sequential_two_sided_candidate_scan.csv", candidates[:50])
    (out_dir / "q1_sequential_two_sided_summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
