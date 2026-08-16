from __future__ import annotations

import csv
import json
import math
from dataclasses import replace
from pathlib import Path

from q1_fixed_sampling import binom_accept_prob, binom_reject_prob
from q2_static_enumeration import Case, evaluate_strategy as eval_q2, read_cases
from q3_general_decision import Q3Model, load_model, part_order, product_order
from q3_reuse_decision import evaluate_strategy as eval_q3, iter_strategies as iter_q3, strategy_label as q3_label


ALPHA = 0.05
SAMPLE_SIZES = [100, 368, 400, 1000]
TIE_TOL = 1e-9


def cp_interval(n: int, x: int, alpha: float = ALPHA) -> tuple[float, float]:
    """Two-sided Clopper-Pearson exact interval for a binomial proportion."""
    if not 0 <= x <= n:
        raise ValueError("x must be in [0, n]")
    if x == 0:
        lower = 0.0
    else:
        target = alpha / 2
        lo, hi = 0.0, x / n
        for _ in range(80):
            mid = (lo + hi) / 2
            if binom_reject_prob(n, x, mid) > target:
                hi = mid
            else:
                lo = mid
        lower = (lo + hi) / 2

    if x == n:
        upper = 1.0
    else:
        target = alpha / 2
        lo, hi = x / n, 1.0
        for _ in range(80):
            mid = (lo + hi) / 2
            if binom_accept_prob(n, x, mid) > target:
                lo = mid
            else:
                hi = mid
        upper = (lo + hi) / 2
    return lower, upper


def sample_record(param_id: str, nominal_p: float, n: int) -> dict:
    x = int(round(n * nominal_p))
    p_hat = x / n
    lower, upper = cp_interval(n, x)
    return {
        "param_id": param_id,
        "sample_size": n,
        "assumed_defective_count": x,
        "nominal_rate_from_table": nominal_p,
        "point_estimate": p_hat,
        "ci_lower": lower,
        "ci_upper": upper,
        "ci_width": upper - lower,
        "assumption_note": "sample size and defective count are modeling assumptions, not data given by the problem",
    }


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


def q2_case_with_rates(case: Case, rates: dict[str, float]) -> Case:
    return replace(case, p1=rates["p1"], p2=rates["p2"], pf=rates["pf"])


def q2_strategy_label(strategy: tuple[int, int, int, int]) -> str:
    return f"({strategy[0]},{strategy[1]},{strategy[2]},{strategy[3]})"


def q2_all_strategies():
    for d1 in (0, 1):
        for d2 in (0, 1):
            for df in (0, 1):
                for dr in (0, 1):
                    yield (d1, d2, df, dr)


def q2_best(case: Case, rates: dict[str, float], retest: bool = False) -> tuple[tuple[int, int, int, int], dict]:
    c = q2_case_with_rates(case, rates)
    best = None
    for strategy in q2_all_strategies():
        result = eval_q2(c, strategy, retest)
        if result["feasible"] and (best is None or result["expected_profit"] > best[1]["expected_profit"]):
            best = (strategy, result)
    if best is None:
        raise RuntimeError("no feasible Q2 strategy")
    return best


def q2_tied_best(case: Case, rates: dict[str, float], retest: bool = False) -> tuple[tuple[int, int, int, int], dict, list[tuple[int, int, int, int]]]:
    c = q2_case_with_rates(case, rates)
    evaluated = []
    best = None
    for strategy in q2_all_strategies():
        result = eval_q2(c, strategy, retest)
        if not result["feasible"]:
            continue
        evaluated.append((strategy, result))
        if best is None or result["expected_profit"] > best[1]["expected_profit"]:
            best = (strategy, result)
    if best is None:
        raise RuntimeError("no feasible Q2 strategy")
    best_profit = best[1]["expected_profit"]
    ties = [strategy for strategy, result in evaluated if abs(result["expected_profit"] - best_profit) <= TIE_TOL]
    return best[0], best[1], ties


def q2_eval_profit(case: Case, rates: dict[str, float], strategy: tuple[int, int, int, int], retest: bool = False) -> dict:
    return eval_q2(q2_case_with_rates(case, rates), strategy, retest)


def q2_endpoint_vectors(lower: dict[str, float], upper: dict[str, float]) -> list[dict[str, float]]:
    keys = ["p1", "p2", "pf"]
    vectors = []
    for mask in range(8):
        vectors.append({key: upper[key] if (mask >> idx) & 1 else lower[key] for idx, key in enumerate(keys)})
    return vectors


def comparable_leq(a: dict[str, float], b: dict[str, float]) -> bool:
    return all(a[key] <= b[key] + 1e-15 for key in a)


def profit_value(result: dict) -> float:
    return result["expected_profit"] if result["feasible"] else -math.inf


def q2_monotonic_check(case: Case, lower: dict[str, float], upper: dict[str, float], retest: bool = False) -> dict:
    violations = []
    comparisons = 0
    endpoint_vectors = q2_endpoint_vectors(lower, upper)
    for strategy in q2_all_strategies():
        profits = [(vec, profit_value(q2_eval_profit(case, vec, strategy, retest))) for vec in endpoint_vectors]
        for low_vec, low_profit in profits:
            for high_vec, high_profit in profits:
                if low_vec == high_vec or not comparable_leq(low_vec, high_vec):
                    continue
                comparisons += 1
                if high_profit > low_profit + 1e-9:
                    violations.append((strategy, low_vec, high_vec, low_profit, high_profit))
    return {
        "method": "all_8_interval_endpoint_comparable_pairs",
        "checked_strategies": 16,
        "checked_endpoint_vectors": 8,
        "checked_comparisons": comparisons,
        "violation_count": len(violations),
        "passed": len(violations) == 0,
    }


def change_type(nominal_strategy, robust_strategy, nominal_ties, robust_ties) -> str:
    if nominal_strategy == robust_strategy:
        return "unchanged"
    if robust_strategy in nominal_ties or nominal_strategy in robust_ties:
        return "display_change_within_tied_best"
    return "strict_switch"


def q2_analysis(cases: list[Case], out_dir: Path) -> tuple[list[dict], list[dict], list[dict]]:
    ci_rows = []
    result_rows = []
    mono_rows = []
    for n in SAMPLE_SIZES:
        for case in cases:
            recs = {
                "p1": sample_record(f"q2_case{case.case_id}_p1", case.p1, n),
                "p2": sample_record(f"q2_case{case.case_id}_p2", case.p2, n),
                "pf": sample_record(f"q2_case{case.case_id}_pf", case.pf, n),
            }
            ci_rows.extend(recs.values())
            point = {key: rec["point_estimate"] for key, rec in recs.items()}
            lower = {key: rec["ci_lower"] for key, rec in recs.items()}
            upper = {key: rec["ci_upper"] for key, rec in recs.items()}
            nominal_strategy, nominal_result, nominal_ties = q2_tied_best(case, point, retest=False)
            mono = q2_monotonic_check(case, lower, upper, retest=False)
            mono_rows.append({"scope": "q2", "sample_size": n, "case_id": case.case_id, **mono})
            if mono["passed"]:
                robust_strategy, robust_worst, robust_ties = q2_tied_best(case, upper, retest=False)
            else:
                robust_strategy, robust_worst, robust_ties = nominal_strategy, q2_eval_profit(case, upper, nominal_strategy, False), [nominal_strategy]
            robust_nominal = q2_eval_profit(case, point, robust_strategy, False)
            nominal_worst = q2_eval_profit(case, upper, nominal_strategy, False)
            result_rows.append(
                {
                    "sample_size": n,
                    "case_id": case.case_id,
                    "nominal_strategy": q2_strategy_label(nominal_strategy),
                    "robust_strategy": q2_strategy_label(robust_strategy),
                    "nominal_best_strategy": q2_strategy_label(nominal_strategy),
                    "robust_best_strategy": q2_strategy_label(robust_strategy),
                    "strategy_changed": nominal_strategy != robust_strategy,
                    "strategy_change_type": change_type(nominal_strategy, robust_strategy, nominal_ties, robust_ties),
                    "nominal_tied_best_count": len(nominal_ties),
                    "nominal_tied_best_strategies": ";".join(q2_strategy_label(s) for s in nominal_ties),
                    "robust_tied_best_count": len(robust_ties),
                    "robust_tied_best_strategies": ";".join(q2_strategy_label(s) for s in robust_ties),
                    "nominal_best_profit_at_point": nominal_result["expected_profit"],
                    "nominal_strategy_worst_profit": nominal_worst["expected_profit"],
                    "robust_strategy_profit_at_point": robust_nominal["expected_profit"],
                    "robust_strategy_worst_profit": robust_worst["expected_profit"],
                    "robust_profit_cost_at_point": nominal_result["expected_profit"] - robust_nominal["expected_profit"],
                    "nominal_market_defect_rate": nominal_result.get("market_defect_rate", ""),
                    "robust_market_defect_rate_at_point": robust_nominal.get("market_defect_rate", ""),
                    "monotonicity_passed": mono["passed"],
                }
            )
    write_csv(out_dir / "q4_q2_ci_inputs.csv", ci_rows)
    write_csv(out_dir / "q4_q2_strategy_comparison.csv", result_rows)
    return ci_rows, result_rows, mono_rows


def q3_model_with_rate_map(model: Q3Model, rates: dict[str, float]) -> Q3Model:
    parts = {
        pid: replace(part, defect_rate=rates[pid])
        for pid, part in model.parts.items()
    }
    products = {
        pid: replace(product, defect_rate=rates[pid])
        for pid, product in model.products.items()
    }
    return Q3Model(parts=parts, products=products, root_id=model.root_id)


def q3_best(model: Q3Model) -> tuple[dict[str, int], dict]:
    best = None
    for strategy in iter_q3(model):
        result = eval_q3(model, strategy)
        if result["feasible"] and (best is None or result["expected_profit"] > best[1]["expected_profit"]):
            best = (strategy, result)
    if best is None:
        raise RuntimeError("no feasible Q3 strategy")
    return best


def q3_tied_best(model: Q3Model) -> tuple[dict[str, int], dict, list[dict[str, int]]]:
    evaluated = []
    best = None
    for strategy in iter_q3(model):
        result = eval_q3(model, strategy)
        if not result["feasible"]:
            continue
        evaluated.append((strategy, result))
        if best is None or result["expected_profit"] > best[1]["expected_profit"]:
            best = (strategy, result)
    if best is None:
        raise RuntimeError("no feasible Q3 strategy")
    best_profit = best[1]["expected_profit"]
    ties = [strategy for strategy, result in evaluated if abs(result["expected_profit"] - best_profit) <= TIE_TOL]
    return best[0], best[1], ties


def q3_strategy_id(model: Q3Model, target: dict[str, int]) -> int:
    for idx, strategy in enumerate(iter_q3(model), 1):
        if strategy == target:
            return idx
    raise ValueError("strategy not found in formal Q3 enumeration order")


def q3_monotonic_check(point_model: Q3Model, upper_rates: dict[str, float]) -> dict:
    param_ids = list(point_model.parts) + list(point_model.products)
    total = 0
    comparisons = 0
    violations = 0
    raised_models = {
        pid: q3_model_with_rate_map(
            point_model,
            {
                **{part_id: part.defect_rate for part_id, part in point_model.parts.items()},
                **{prod_id: prod.defect_rate for prod_id, prod in point_model.products.items()},
                pid: upper_rates[pid],
            },
        )
        for pid in param_ids
    }
    for strategy in iter_q3(point_model):
        total += 1
        nominal = eval_q3(point_model, strategy)
        nominal_profit = profit_value(nominal)
        for pid, raised_model in raised_models.items():
            raised = eval_q3(raised_model, strategy)
            comparisons += 1
            if profit_value(raised) > nominal_profit + 1e-9:
                violations += 1
    return {
        "method": "one_parameter_at_a_time_raise_to_ci_upper_from_point_estimate",
        "checked_strategies": total,
        "checked_parameters": len(param_ids),
        "checked_comparisons": comparisons,
        "violation_count": violations,
        "passed": violations == 0,
    }


def q3_analysis(model: Q3Model, out_dir: Path) -> tuple[list[dict], list[dict], list[dict]]:
    ci_rows = []
    result_rows = []
    mono_rows = []
    for n in SAMPLE_SIZES:
        nominal_rates = {}
        for pid, part in model.parts.items():
            rec = sample_record(f"q3_{pid}", part.defect_rate, n)
            ci_rows.append(rec)
            nominal_rates[pid] = rec
        for pid, product in model.products.items():
            rec = sample_record(f"q3_{pid}", product.defect_rate, n)
            ci_rows.append(rec)
            nominal_rates[pid] = rec

        point = {pid: rec["point_estimate"] for pid, rec in nominal_rates.items()}
        upper = {pid: rec["ci_upper"] for pid, rec in nominal_rates.items()}
        point_model = q3_model_with_rate_map(model, point)
        upper_model = q3_model_with_rate_map(model, upper)
        nominal_strategy, nominal_result, nominal_ties = q3_tied_best(point_model)
        mono = q3_monotonic_check(point_model, upper)
        mono_rows.append({"scope": "q3", "sample_size": n, "case_id": "q3", **mono})
        if mono["passed"]:
            robust_strategy, robust_worst, robust_ties = q3_tied_best(upper_model)
        else:
            robust_strategy, robust_worst, robust_ties = nominal_strategy, eval_q3(upper_model, nominal_strategy), [nominal_strategy]
        robust_nominal = eval_q3(point_model, robust_strategy)
        nominal_worst = eval_q3(upper_model, nominal_strategy)
        nominal_strategy_id = q3_strategy_id(model, nominal_strategy)
        robust_strategy_id = q3_strategy_id(model, robust_strategy)
        result_rows.append(
            {
                "sample_size": n,
                "nominal_strategy": q3_label(nominal_strategy),
                "robust_strategy": q3_label(robust_strategy),
                "nominal_best_strategy": q3_label(nominal_strategy),
                "robust_best_strategy": q3_label(robust_strategy),
                "nominal_best_strategy_id": nominal_strategy_id,
                "robust_best_strategy_id": robust_strategy_id,
                "strategy_changed": q3_label(nominal_strategy) != q3_label(robust_strategy),
                "strategy_change_type": change_type(q3_label(nominal_strategy), q3_label(robust_strategy), [q3_label(s) for s in nominal_ties], [q3_label(s) for s in robust_ties]),
                "nominal_tied_best_count": len(nominal_ties),
                "nominal_tied_best_strategies": " | ".join(q3_label(s) for s in nominal_ties),
                "robust_tied_best_count": len(robust_ties),
                "robust_tied_best_strategies": " | ".join(q3_label(s) for s in robust_ties),
                "nominal_best_profit_at_point": nominal_result["expected_profit"],
                "nominal_strategy_worst_profit": nominal_worst["expected_profit"],
                "robust_strategy_profit_at_point": robust_nominal["expected_profit"],
                "robust_strategy_worst_profit": robust_worst["expected_profit"],
                "robust_profit_cost_at_point": nominal_result["expected_profit"] - robust_nominal["expected_profit"],
                "nominal_single_assembly_good_probability": nominal_result["single_assembly_good_probability"],
                "robust_single_assembly_good_probability_at_point": robust_nominal["single_assembly_good_probability"],
                "fulfillment_success_probability": robust_nominal["fulfillment_success_probability"],
                "monotonicity_passed": mono["passed"],
            }
        )
    write_csv(out_dir / "q4_q3_ci_inputs.csv", ci_rows)
    write_csv(out_dir / "q4_q3_strategy_comparison.csv", result_rows)
    return ci_rows, result_rows, mono_rows


def regression_checks(cases: list[Case], q3_model: Q3Model, out_dir: Path) -> list[dict]:
    checks = []
    frozen_q2 = {
        1: ("(1,1,0,1)", 18.111111111111107),
        2: ("(1,1,0,1)", 12.0),
        3: ("(1,1,0,1)", 15.444444444444443),
        4: ("(1,1,1,1)", 14.75),
        5: ("(0,1,0,0)", 11.987654320987659),
        6: ("(0,0,0,0)", 21.6786703601108),
    }
    for case in cases:
        rates = {"p1": case.p1, "p2": case.p2, "pf": case.pf}
        strategy, result = q2_best(case, rates, retest=False)
        expected_strategy, expected_profit = frozen_q2[case.case_id]
        checks.append(
            {
                "scope": "q2_point_input_regression",
                "case_id": case.case_id,
                "expected_strategy": expected_strategy,
                "got_strategy": q2_strategy_label(strategy),
                "actual_strategy": q2_strategy_label(strategy),
                "expected_profit": expected_profit,
                "got_profit": result["expected_profit"],
                "actual_profit": result["expected_profit"],
                "abs_error": abs(result["expected_profit"] - expected_profit),
                "passed": expected_strategy == q2_strategy_label(strategy) and abs(result["expected_profit"] - expected_profit) <= 1e-8,
            }
        )
    strategy, result = q3_best(q3_model)
    expected_profit = 60.22222222222223
    expected_strategy = "test_p1;test_p2;test_p3;test_p4;test_p5;test_p6;test_p7;test_p8;test_s1;test_s2;test_s3;disassemble_s1;disassemble_s2;disassemble_s3;disassemble_final"
    checks.append(
        {
            "scope": "q3_point_input_regression",
            "case_id": "q3",
            "expected_strategy": expected_strategy,
            "got_strategy": q3_label(strategy),
            "actual_strategy": q3_label(strategy),
            "expected_profit": expected_profit,
            "got_profit": result["expected_profit"],
            "actual_profit": result["expected_profit"],
            "abs_error": abs(result["expected_profit"] - expected_profit),
            "passed": q3_label(strategy) == expected_strategy and abs(result["expected_profit"] - expected_profit) <= 1e-8,
        }
    )
    write_csv(out_dir / "q4_regression_checks.csv", checks)
    return checks


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    out_dir = root / "results" / "q4"
    cases = read_cases(root / "data" / "q2_table1.csv")
    q3_model = load_model(root / "data" / "q3_table2.json")

    q2_ci, q2_results, q2_mono = q2_analysis(cases, out_dir)
    q3_ci, q3_results, q3_mono = q3_analysis(q3_model, out_dir)
    regression = regression_checks(cases, q3_model, out_dir)
    monotonicity = q2_mono + q3_mono
    write_csv(out_dir / "q4_monotonicity_checks.csv", monotonicity)

    summary = {
        "model_scope": "minimal Q4 baseline: Clopper-Pearson exact intervals plus robust expected-profit maximization",
        "sample_size_scenarios": SAMPLE_SIZES,
        "sample_data_assumption": "defective count is round(sample_size * nominal table defect rate); this is a modeling assumption, not problem-provided data",
        "q2_result_rows": len(q2_results),
        "q3_result_rows": len(q3_results),
        "monotonicity_checks_passed": all(row["passed"] for row in monotonicity),
        "point_input_regression_checks_passed": all(row["passed"] for row in regression),
        "monotonicity_failed_rows": [row for row in monotonicity if not row["passed"]],
        "point_input_regression_failed_rows": [row for row in regression if not row["passed"]],
        "note": "Point-estimate runs use x/n after integer rounding and therefore may differ across sample sizes for reasons beyond interval width; point-input regression uses original table rates directly and is not produced by natural zero-width sample intervals.",
    }
    (out_dir / "q4_summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
