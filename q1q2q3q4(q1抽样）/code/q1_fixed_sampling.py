from __future__ import annotations

import csv
import json
import math
from pathlib import Path


def binom_pmf_values(n: int, p: float) -> list[float]:
    if n < 0:
        raise ValueError("n must be non-negative")
    if not 0 <= p <= 1:
        raise ValueError("p must be in [0, 1]")
    if p == 0:
        return [1.0] + [0.0] * n
    if p == 1:
        return [0.0] * n + [1.0]

    values = [(1 - p) ** n]
    ratio = p / (1 - p)
    for k in range(n):
        values.append(values[-1] * (n - k) / (k + 1) * ratio)
    return values


def binom_accept_prob(n: int, a: int, p: float) -> float:
    """Return P_p(X <= a), X ~ Bin(n, p)."""
    if a < 0:
        return 0.0
    if a >= n:
        return 1.0
    return math.fsum(binom_pmf_values(n, p)[: a + 1])


def binom_reject_prob(n: int, r: int, p: float) -> float:
    """Return P_p(X >= r), X ~ Bin(n, p)."""
    if r <= 0:
        return 1.0
    if r > n:
        return 0.0
    return math.fsum(binom_pmf_values(n, p)[r:])


def evaluate_plan(n: int, a: int, r: int, p0: float = 0.10, p_bad: float = 0.15) -> dict:
    p0_accept = binom_accept_prob(n, a, p0)
    p0_reject = binom_reject_prob(n, r, p0)
    pbad_reject = binom_reject_prob(n, r, p_bad)
    return {
        "n": n,
        "a": a,
        "r": r,
        "p0": p0,
        "p_bad": p_bad,
        "p0_accept": p0_accept,
        "p0_reject": p0_reject,
        "p0_inconclusive": 1.0 - p0_accept - p0_reject,
        "pbad_accept": binom_accept_prob(n, a, p_bad),
        "pbad_reject": pbad_reject,
        "pbad_inconclusive": 1.0 - binom_accept_prob(n, a, p_bad) - pbad_reject,
        "pbad_not_reject": 1.0 - pbad_reject,
    }


def thresholds_for_n(n: int, p0: float, accept_error: float, reject_error: float) -> tuple[int | None, int | None]:
    a = None
    for candidate in range(n + 1):
        if binom_accept_prob(n, candidate, p0) <= accept_error:
            a = candidate
        else:
            break

    r = None
    for candidate in range(n + 1):
        if binom_reject_prob(n, candidate, p0) <= reject_error:
            r = candidate
            break
    return a, r


def search_min_nar(
    p0: float = 0.10,
    p_bad: float = 0.15,
    accept_error: float = 0.10,
    reject_error: float = 0.05,
    reject_power: float = 0.90,
    n_max: int = 5000,
) -> dict:
    for n in range(1, n_max + 1):
        a, r = thresholds_for_n(n, p0, accept_error, reject_error)
        if a is None or r is None or not a < r:
            continue
        if binom_reject_prob(n, r, p_bad) >= reject_power:
            result = evaluate_plan(n, a, r, p0, p_bad)
            result.update(
                {
                    "accept_error_limit": accept_error,
                    "reject_error_limit": reject_error,
                    "reject_power_target": reject_power,
                }
            )
            return result
    raise RuntimeError(f"No feasible plan found up to n={n_max}")


def probability_curve(n: int, a: int, r: int, step: float = 0.001) -> list[dict]:
    rows = []
    count = int(round(0.30 / step))
    for i in range(count + 1):
        p = round(i * step, 6)
        accept = binom_accept_prob(n, a, p)
        reject = binom_reject_prob(n, r, p)
        rows.append(
            {
                "p": p,
                "accept_prob": accept,
                "reject_prob": reject,
                "inconclusive_prob": 1.0 - accept - reject,
            }
        )
    return rows


def write_csv(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8-sig") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    out_dir = root / "results" / "q1"
    out_dir.mkdir(parents=True, exist_ok=True)

    plan = search_min_nar()
    n367_a, n367_r = thresholds_for_n(367, 0.10, 0.10, 0.05)
    n367 = evaluate_plan(367, n367_a, n367_r, 0.10, 0.15)

    selected_ps = [0.05, 0.10, 0.13, 0.15, 0.20]
    selected_rows = []
    for p in selected_ps:
        accept = binom_accept_prob(plan["n"], plan["a"], p)
        reject = binom_reject_prob(plan["n"], plan["r"], p)
        selected_rows.append(
            {
                "p": p,
                "accept_prob": accept,
                "reject_prob": reject,
                "inconclusive_prob": 1.0 - accept - reject,
            }
        )

    sensitivity_rows = []
    for p_bad in [0.13, 0.15, 0.20]:
        row = search_min_nar(p_bad=p_bad)
        sensitivity_rows.append(
            {
                "p_bad": p_bad,
                "n": row["n"],
                "a": row["a"],
                "r": row["r"],
                "pbad_reject": row["pbad_reject"],
            }
        )

    summary = {
        "main_plan": plan,
        "n367_check": n367,
        "sampling_cost_expression": f"{plan['n']} * C_s",
    }
    (out_dir / "q1_fixed_summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    write_csv(out_dir / "q1_selected_probabilities.csv", selected_rows)
    write_csv(out_dir / "q1_probability_curve.csv", probability_curve(plan["n"], plan["a"], plan["r"]))
    write_csv(out_dir / "q1_sensitivity_pbad.csv", sensitivity_rows)

    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
