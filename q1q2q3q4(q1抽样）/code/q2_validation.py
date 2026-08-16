from __future__ import annotations

import csv
import json
import math
import random
from pathlib import Path

from q2_static_enumeration import (
    METRICS,
    Case,
    build_chain,
    evaluate_strategy,
    read_cases,
    strategy_rows,
)


def is_close(a: float, b: float, tol: float) -> bool:
    return abs(a - b) <= tol


def finite_sum(values: list[float]) -> float:
    return math.fsum(v for v in values if math.isfinite(v))


def validate_probability_normalization(cases: list[Case]) -> list[dict]:
    checks = []
    for retest in (False, True):
        policy = "retest_known_good" if retest else "no_retest_known_good"
        for case in cases:
            for d1 in (0, 1):
                for d2 in (0, 1):
                    for df in (0, 1):
                        for dr in (0, 1):
                            strategy = (d1, d2, df, dr)
                            states, expanded = build_chain(case, strategy, retest)
                            max_error = 0.0
                            for state in states:
                                _, transitions, success_prob = expanded[state]
                                total = success_prob + finite_sum(list(transitions.values()))
                                max_error = max(max_error, abs(total - 1.0))
                            checks.append(
                                {
                                    "check": "probability_normalization",
                                    "inspection_policy": policy,
                                    "case_id": case.case_id,
                                    "strategy": str(strategy),
                                    "state_count": len(states),
                                    "max_error": max_error,
                                    "passed": max_error <= 1e-9,
                                }
                            )
    return checks


def validate_cost_and_revenue(rows: list[dict]) -> list[dict]:
    checks = []
    cost_fields = [
        "expected_purchase_cost",
        "expected_part_test_cost",
        "expected_assembly_cost",
        "expected_final_test_cost",
        "expected_exchange_loss_cost",
        "expected_disassembly_cost",
    ]
    for row in rows:
        if not row["feasible"]:
            continue
        expected_cost = float(row["expected_cost"])
        expected_profit = float(row["expected_profit"])
        price = float(row["price"]) if "price" in row else None
        parts_sum = sum(float(row[name]) for name in cost_fields)
        cost_error = abs(parts_sum - expected_cost)
        checks.append(
            {
                "check": "cost_decomposition",
                "inspection_policy": row["inspection_policy"],
                "case_id": row["case_id"],
                "strategy": str((row["d1_test_part1"], row["d2_test_part2"], row["df_test_final"], row["dr_disassemble"])),
                "error": cost_error,
                "passed": cost_error <= 1e-8,
            }
        )
        if price is not None:
            revenue_error = abs((price - expected_cost) - expected_profit)
            checks.append(
                {
                    "check": "revenue_conservation",
                    "inspection_policy": row["inspection_policy"],
                    "case_id": row["case_id"],
                    "strategy": str((row["d1_test_part1"], row["d2_test_part2"], row["df_test_final"], row["dr_disassemble"])),
                    "error": revenue_error,
                    "passed": revenue_error <= 1e-8,
                }
            )
    return checks


def closed_form_no_test_no_disassemble(case: Case) -> float:
    q = (1.0 - case.p1) * (1.0 - case.p2) * (1.0 - case.pf)
    base = case.buy1 + case.buy2 + case.assemble
    return base / q + case.exchange_loss * (1.0 - q) / q


def closed_form_test_both_no_final_disassemble(case: Case, retest_known_good: bool) -> float:
    part_cost = (case.buy1 + case.test1) / (1.0 - case.p1) + (case.buy2 + case.test2) / (1.0 - case.p2)
    rework_cost = case.exchange_loss + case.disassemble
    if retest_known_good:
        rework_cost += case.test1 + case.test2
    return part_cost + case.assemble / (1.0 - case.pf) + rework_cost * case.pf / (1.0 - case.pf)


def validate_closed_form(cases: list[Case]) -> list[dict]:
    checks = []
    for case in cases:
        for retest in (False, True):
            examples = [
                ((0, 0, 0, 0), closed_form_no_test_no_disassemble(case)),
                ((1, 1, 0, 1), closed_form_test_both_no_final_disassemble(case, retest)),
            ]
            for strategy, expected in examples:
                policy = "retest_known_good" if retest else "no_retest_known_good"
                got = evaluate_strategy(case, strategy, retest)
                if not got["feasible"]:
                    checks.append(
                        {
                            "check": "closed_form",
                            "inspection_policy": policy,
                            "case_id": case.case_id,
                            "strategy": str(strategy),
                            "expected": expected,
                            "got": "infeasible",
                            "error": math.inf,
                            "passed": False,
                        }
                    )
                    continue
                error = abs(float(got["expected_cost"]) - expected)
                checks.append(
                    {
                        "check": "closed_form",
                        "inspection_policy": policy,
                        "case_id": case.case_id,
                        "strategy": str(strategy),
                        "expected": expected,
                        "got": got["expected_cost"],
                        "error": error,
                        "passed": error <= 1e-8,
                    }
                )
    return checks


def validate_extreme_cases() -> list[dict]:
    checks = []
    good_case = Case(101, 0.0, 4, 2, 0.0, 18, 3, 0.0, 6, 3, 56, 6, 5)
    result = evaluate_strategy(good_case, (0, 0, 0, 0), False)
    expected = 4 + 18 + 6
    checks.append(
        {
            "check": "extreme_all_good",
            "expected_cost": expected,
            "got": result["expected_cost"],
            "passed": result["feasible"] and abs(result["expected_cost"] - expected) <= 1e-8,
        }
    )

    always_bad_case = Case(102, 1.0, 4, 2, 0.1, 18, 3, 0.1, 6, 3, 56, 6, 5)
    result = evaluate_strategy(always_bad_case, (0, 0, 0, 0), False)
    checks.append(
        {
            "check": "extreme_never_success",
            "strategy": "(0,0,0,0)",
            "feasible": result["feasible"],
            "passed": not result["feasible"],
        }
    )

    recoverable_case = Case(103, 0.9, 4, 2, 0.1, 18, 3, 0.1, 6, 3, 56, 6, 5)
    result = evaluate_strategy(recoverable_case, (1, 1, 0, 0), False)
    checks.append(
        {
            "check": "extreme_detect_recovers",
            "strategy": "(1,1,0,0)",
            "feasible": result["feasible"],
            "passed": result["feasible"] and math.isfinite(result["expected_cost"]),
        }
    )
    return checks


def simulate_strategy(case: Case, strategy: tuple[int, int, int, int], retest_known_good: bool, rng: random.Random) -> dict[str, float]:
    d1, d2, df, dr = strategy
    cost = 0.0
    exchanges = 0
    disassemblies = 0
    assemblies = 0
    market_bad = 0
    parts: list[int | None] = [None, None]
    known_good = [False, False]

    while True:
        if parts[0] is None:
            cost += case.buy1
            parts[0] = 0 if rng.random() < case.p1 else 1
            known_good[0] = False
        if parts[1] is None:
            cost += case.buy2
            parts[1] = 0 if rng.random() < case.p2 else 1
            known_good[1] = False

        for idx, should_test in enumerate((d1, d2)):
            if not should_test:
                continue
            if known_good[idx] and not retest_known_good:
                continue
            cost += case.test1 if idx == 0 else case.test2
            while parts[idx] == 0:
                cost += case.buy1 if idx == 0 else case.buy2
                cost += case.test1 if idx == 0 else case.test2
                p_bad = case.p1 if idx == 0 else case.p2
                parts[idx] = 0 if rng.random() < p_bad else 1
            known_good[idx] = True

        cost += case.assemble
        assemblies += 1
        product_good = parts[0] == 1 and parts[1] == 1 and rng.random() >= case.pf

        if df:
            cost += case.test_final
            if product_good:
                return {
                    "cost": cost,
                    "exchanges": exchanges,
                    "disassemblies": disassemblies,
                    "assemblies": assemblies,
                    "market_bad": market_bad,
                }
            if dr:
                cost += case.disassemble
                disassemblies += 1
            else:
                parts = [None, None]
                known_good = [False, False]
        else:
            if product_good:
                return {
                    "cost": cost,
                    "exchanges": exchanges,
                    "disassemblies": disassemblies,
                    "assemblies": assemblies,
                    "market_bad": market_bad,
                }
            cost += case.exchange_loss
            exchanges += 1
            market_bad += 1
            if dr:
                cost += case.disassemble
                disassemblies += 1
            else:
                parts = [None, None]
                known_good = [False, False]


def validate_monte_carlo(cases: list[Case], trials: int = 50000) -> list[dict]:
    checks = []
    examples = [
        (1, (0, 0, 0, 0)),
        (1, (1, 1, 0, 1)),
        (4, (1, 1, 1, 1)),
        (5, (0, 1, 0, 0)),
        (6, (0, 0, 0, 0)),
    ]
    case_map = {case.case_id: case for case in cases}
    for case_id, strategy in examples:
        case = case_map[case_id]
        for retest in (False, True):
            exact = evaluate_strategy(case, strategy, retest)
            if not exact["feasible"]:
                continue
            rng = random.Random(20260900 + case_id * 100 + sum(strategy) + int(retest))
            samples = [simulate_strategy(case, strategy, retest, rng)["cost"] for _ in range(trials)]
            mean = sum(samples) / trials
            variance = sum((x - mean) ** 2 for x in samples) / (trials - 1)
            stderr = math.sqrt(variance / trials)
            error = abs(mean - exact["expected_cost"])
            checks.append(
                {
                    "check": "monte_carlo_cost",
                    "inspection_policy": "retest_known_good" if retest else "no_retest_known_good",
                    "case_id": case_id,
                    "strategy": str(strategy),
                    "trials": trials,
                    "exact_cost": exact["expected_cost"],
                    "simulated_mean_cost": mean,
                    "standard_error": stderr,
                    "abs_error": error,
                    "passed": error <= max(0.20, 4.0 * stderr),
                }
            )
    return checks


def attach_prices(rows: list[dict], cases: list[Case]) -> list[dict]:
    price_map = {case.case_id: case.price for case in cases}
    result = []
    for row in rows:
        out = row.copy()
        out["price"] = price_map[int(row["case_id"])]
        result.append(out)
    return result


def write_csv(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    if not rows:
        return
    fieldnames = []
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
    cases = read_cases(root / "data" / "q2_table1.csv")
    out_dir = root / "results" / "q2"
    rows = attach_prices(strategy_rows(cases, False) + strategy_rows(cases, True), cases)

    checks = []
    checks.extend(validate_probability_normalization(cases))
    checks.extend(validate_cost_and_revenue(rows))
    checks.extend(validate_closed_form(cases))
    checks.extend(validate_extreme_cases())
    checks.extend(validate_monte_carlo(cases))

    write_csv(out_dir / "q2_validation_checks.csv", checks)
    summary = {
        "check_count": len(checks),
        "passed_count": sum(1 for row in checks if row["passed"]),
        "failed_count": sum(1 for row in checks if not row["passed"]),
        "failed_checks": [row for row in checks if not row["passed"]][:20],
        "note": "Monte Carlo checks use fixed random seeds and tolerance based on standard error.",
    }
    (out_dir / "q2_validation_summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False, indent=2))
    if summary["failed_count"]:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
