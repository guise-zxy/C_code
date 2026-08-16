from __future__ import annotations

import csv
import json
import math
from pathlib import Path


P0 = 0.10
P_BAD = 0.15
N_MAX = 368
FIXED_N = 368

# Minimal feasible sequential baseline found by exact recursive screening.
# It keeps the fixed-sample final rejection boundary and only adds early rejection.
MIN_EARLY_REJECT_N = 180
LOG_LR_REJECT = 4.4
FINAL_ACCEPT_BOUNDARY = 29
FINAL_REJECT_BOUNDARY = 47

TYPICAL_P = [0.05, 0.08, 0.10, 0.12, 0.13, 0.15, 0.18, 0.20]


def log_lr(n: int, k: int, p0: float = P0, p_bad: float = P_BAD) -> float:
    return k * math.log(p_bad / p0) + (n - k) * math.log((1.0 - p_bad) / (1.0 - p0))


def make_boundaries() -> list[dict]:
    rows = []
    for n in range(1, N_MAX + 1):
        accept_boundary = -1
        reject_boundary = n + 1
        if n >= MIN_EARLY_REJECT_N:
            for k in range(n + 1):
                if log_lr(n, k) >= LOG_LR_REJECT:
                    reject_boundary = k
                    break
        if n == N_MAX:
            accept_boundary = max(accept_boundary, FINAL_ACCEPT_BOUNDARY)
            reject_boundary = min(reject_boundary, FINAL_REJECT_BOUNDARY)
        if accept_boundary >= reject_boundary:
            raise RuntimeError(f"overlapping boundaries at n={n}")
        continue_low = accept_boundary + 1
        continue_high = reject_boundary - 1
        rows.append(
            {
                "stage_n": n,
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
    stop_prob_by_stage = []
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
        stage_accept = 0.0
        stage_reject = 0.0
        for k, prob in enumerate(next_dist):
            if prob == 0.0:
                continue
            if k <= a:
                stage_accept += prob
            elif k >= r:
                stage_reject += prob
            else:
                alive[k] = prob
        stage_stop = stage_accept + stage_reject
        accept_prob += stage_accept
        reject_prob += stage_reject
        expected_n += n * stage_stop
        stop_prob_by_stage.append(stage_stop)

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
        "stop_prob_by_stage": stop_prob_by_stage,
    }


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


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    out_dir = root / "results" / "q1"
    boundaries = make_boundaries()

    operating_rows = []
    for p in TYPICAL_P:
        result = evaluate(boundaries, p)
        operating_rows.append({k: v for k, v in result.items() if k != "stop_prob_by_stage"})

    curve_rows = []
    for i in range(31):
        p = i / 100
        result = evaluate(boundaries, p)
        curve_rows.append({k: v for k, v in result.items() if k != "stop_prob_by_stage"})

    p0_result = evaluate(boundaries, P0)
    pbad_result = evaluate(boundaries, P_BAD)
    validation = {
        "p0_reject_prob_le_0.05": p0_result["reject_prob"] <= 0.05 + 1e-12,
        "p0_accept_prob_le_0.10": p0_result["accept_prob"] <= 0.10 + 1e-12,
        "pbad_reject_prob_ge_0.90": pbad_result["reject_prob"] >= 0.90 - 1e-12,
        "max_probability_sum_error": max(row["probability_sum_error"] for row in operating_rows + curve_rows),
    }
    summary = {
        "model_scope": "minimal exact sequential sampling baseline; Q1 fixed-sample main model remains frozen",
        "p0": P0,
        "p_bad": P_BAD,
        "n_max": N_MAX,
        "boundary_rule": {
            "min_early_reject_n": MIN_EARLY_REJECT_N,
            "log_likelihood_ratio_reject_boundary": LOG_LR_REJECT,
            "final_accept_if_defects_le": FINAL_ACCEPT_BOUNDARY,
            "final_reject_if_defects_ge": FINAL_REJECT_BOUNDARY,
            "early_accept": False,
        },
        "p0_result": {k: v for k, v in p0_result.items() if k != "stop_prob_by_stage"},
        "pbad_result": {k: v for k, v in pbad_result.items() if k != "stop_prob_by_stage"},
        "validation": validation,
        "interpretation": "Feasible under the three constraints. ASN reduction is material near p=15%, but negligible near p=10%; use as an improvement direction rather than replacing the frozen fixed-sample baseline.",
    }

    write_csv(out_dir / "q1_sequential_boundaries.csv", boundaries)
    write_csv(out_dir / "q1_sequential_operating_characteristics.csv", operating_rows)
    write_csv(out_dir / "q1_sequential_probability_curve.csv", curve_rows)
    (out_dir / "q1_sequential_summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
