from __future__ import annotations

import csv
import json
from dataclasses import replace
from pathlib import Path

from q2_static_enumeration import Case, evaluate_strategy, read_cases


def best_strategy(case: Case, retest_known_good: bool = False) -> tuple[tuple[int, int, int, int], dict]:
    best = None
    for d1 in (0, 1):
        for d2 in (0, 1):
            for df in (0, 1):
                for dr in (0, 1):
                    strategy = (d1, d2, df, dr)
                    result = evaluate_strategy(case, strategy, retest_known_good)
                    if result["feasible"] and (best is None or result["expected_profit"] > best[1]["expected_profit"]):
                        best = (strategy, result)
    if best is None:
        raise RuntimeError("no feasible strategy")
    return best


def bisection_threshold(fn, lo: float, hi: float, iterations: int = 80) -> float:
    f_lo = fn(lo)
    f_hi = fn(hi)
    if f_lo == 0:
        return lo
    if f_hi == 0:
        return hi
    if f_lo * f_hi > 0:
        raise ValueError("bisection interval does not bracket a root")
    for _ in range(iterations):
        mid = (lo + hi) / 2
        f_mid = fn(mid)
        if f_lo * f_mid <= 0:
            hi = mid
            f_hi = f_mid
        else:
            lo = mid
            f_lo = f_mid
    return (lo + hi) / 2


def write_csv(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    cases = read_cases(root / "data" / "q2_table1.csv")
    case5 = cases[4]
    case6 = cases[5]
    out_dir = root / "results" / "q2"

    case5_rows = []
    case5_thresholds = {}
    for retest in (False, True):
        policy = "retest_known_good" if retest else "no_retest_known_good"

        def diff_case5(exchange_loss: float) -> float:
            c = replace(case5, exchange_loss=exchange_loss)
            no_final = evaluate_strategy(c, (0, 1, 0, 0), retest)["expected_profit"]
            test_final = evaluate_strategy(c, (0, 1, 1, 0), retest)["expected_profit"]
            return no_final - test_final

        threshold = bisection_threshold(diff_case5, 10.0, 11.0)
        case5_thresholds[policy] = threshold
        for exchange_loss in [10.0, 10.25, 10.5, threshold, 10.53, 10.75, 11.0]:
            c = replace(case5, exchange_loss=exchange_loss)
            no_final = evaluate_strategy(c, (0, 1, 0, 0), retest)
            test_final = evaluate_strategy(c, (0, 1, 1, 0), retest)
            best = best_strategy(c, retest)
            case5_rows.append(
                {
                    "inspection_policy": policy,
                    "exchange_loss": exchange_loss,
                    "profit_no_final_test_strategy_0100": no_final["expected_profit"],
                    "profit_final_test_strategy_0110": test_final["expected_profit"],
                    "profit_diff_no_final_minus_final": no_final["expected_profit"] - test_final["expected_profit"],
                    "best_strategy": str(best[0]),
                    "best_profit": best[1]["expected_profit"],
                }
            )

    case6_rows = []
    case6_thresholds = {}
    for retest in (False, True):
        policy = "retest_known_good" if retest else "no_retest_known_good"

        def diff_case6(p: float) -> float:
            c = replace(case6, p1=p, p2=p, pf=p)
            test_part1 = evaluate_strategy(c, (1, 0, 0, 0), retest)["expected_profit"]
            no_test = evaluate_strategy(c, (0, 0, 0, 0), retest)["expected_profit"]
            return test_part1 - no_test

        threshold = bisection_threshold(diff_case6, 0.05, 0.07)
        case6_thresholds[policy] = threshold
        for p in [0.05, 0.055, 0.058, threshold, 0.0588, 0.059, 0.06, 0.065]:
            c = replace(case6, p1=p, p2=p, pf=p)
            no_test = evaluate_strategy(c, (0, 0, 0, 0), retest)
            test_part1 = evaluate_strategy(c, (1, 0, 0, 0), retest)
            best = best_strategy(c, retest)
            case6_rows.append(
                {
                    "inspection_policy": policy,
                    "p1_p2_pf": p,
                    "profit_no_test_strategy_0000": no_test["expected_profit"],
                    "profit_test_part1_strategy_1000": test_part1["expected_profit"],
                    "profit_diff_test_part1_minus_no_test": test_part1["expected_profit"] - no_test["expected_profit"],
                    "best_strategy": str(best[0]),
                    "best_profit": best[1]["expected_profit"],
                }
            )

    summary = {
        "case5_exchange_loss_threshold": case5_thresholds,
        "case6_synced_defect_rate_threshold": case6_thresholds,
        "interpretation": {
            "case5": "Above the threshold, final product inspection (0,1,1,0) overtakes no final inspection (0,1,0,0).",
            "case6": "Above the threshold, testing part 1 (1,0,0,0) overtakes no testing (0,0,0,0).",
            "baseline_unchanged": "These are sensitivity checks only and do not change the baseline result.",
        },
    }

    write_csv(out_dir / "q2_sensitivity_case5_exchange_loss.csv", case5_rows)
    write_csv(out_dir / "q2_sensitivity_case6_synced_defect_rate.csv", case6_rows)
    (out_dir / "q2_sensitivity_summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
