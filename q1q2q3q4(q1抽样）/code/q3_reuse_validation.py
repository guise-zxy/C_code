from __future__ import annotations

import csv
import json
import math
import random
from pathlib import Path

from q3_general_decision import load_model, part_order, product_order
from q3_reuse_decision import COST_KEYS, evaluate_strategy, target_strategy
from q3_general_decision import evaluate_strategy as old_evaluate_strategy


def write_csv(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = []
    for row in rows:
        for key in row:
            if key not in fieldnames:
                fieldnames.append(key)
    with path.open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def cost_sum(result: dict) -> float:
    return math.fsum(result[key] for key in COST_KEYS)


def strategy_all_tests(model, final_disassemble: bool, semi_disassemble: bool, final_test: bool = False) -> dict[str, int]:
    strategy = target_strategy(model, semi_disassemble=semi_disassemble)
    strategy[f"test_{model.root_id}"] = int(final_test)
    strategy[f"disassemble_{model.root_id}"] = int(final_disassemble)
    return strategy


def simulate_all_known_good_reuse(
    model,
    semi_disassemble: bool,
    final_test: bool,
    final_disassemble: bool = True,
    trials: int = 50000,
) -> dict:
    rng = random.Random(2026081303 + int(semi_disassemble) * 17 + int(final_test))
    parts = model.parts
    products = model.products
    root = products[model.root_id]
    semi_ids = [pid for pid in product_order(model) if pid != model.root_id]

    def produce_part(part_id: str) -> tuple[float, bool]:
        part = parts[part_id]
        cost = 0.0
        while True:
            cost += part.buy_cost + part.test_cost
            if rng.random() >= part.defect_rate:
                return cost, True

    def produce_semi(semi_id: str) -> tuple[float, bool]:
        semi = products[semi_id]
        child_cost = 0.0
        for child in semi.children:
            c, _ = produce_part(child)
            child_cost += c
        if semi_disassemble:
            attempts = 0
            cost = child_cost
            while True:
                attempts += 1
                cost += semi.assemble_cost + semi.test_cost
                if rng.random() >= semi.defect_rate:
                    return cost, True
                cost += semi.disassemble_cost
        cost = child_cost
        while True:
            # Failed semi without reuse means all children are produced again.
            cost += semi.assemble_cost + semi.test_cost
            if rng.random() >= semi.defect_rate:
                return cost, True
            for child in semi.children:
                c, _ = produce_part(child)
                cost += c

    samples = []
    for _ in range(trials):
        cost = 0.0
        for semi_id in semi_ids:
            c, _ = produce_semi(semi_id)
            cost += c
        if final_test:
            while True:
                cost += root.assemble_cost + root.test_cost
                if rng.random() >= root.defect_rate:
                    break
                if final_disassemble:
                    cost += root.disassemble_cost
                else:
                    for semi_id in semi_ids:
                        c, _ = produce_semi(semi_id)
                        cost += c
        elif final_disassemble:
            while True:
                cost += root.assemble_cost
                if rng.random() >= root.defect_rate:
                    break
                cost += root.exchange_loss + root.disassemble_cost
        else:
            while True:
                cost += root.assemble_cost
                if rng.random() >= root.defect_rate:
                    break
                cost += root.exchange_loss
                for semi_id in semi_ids:
                    c, _ = produce_semi(semi_id)
                    cost += c
        samples.append(cost)
    mean = sum(samples) / trials
    var = sum((x - mean) ** 2 for x in samples) / (trials - 1)
    return {"mean": mean, "stderr": math.sqrt(var / trials), "trials": trials}


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    model = load_model(root / "data" / "q3_table2.json")
    out_dir = root / "results" / "q3"
    checks = []

    cases = [
        ("closed_form_final_reuse_only", strategy_all_tests(model, True, False, False), 147.03703703703704),
        ("closed_form_final_and_semi_reuse", strategy_all_tests(model, True, True, False), 139.77777777777777),
    ]
    for name, strategy, expected in cases:
        result = evaluate_strategy(model, strategy)
        checks.append(
            {
                "check": name,
                "expected": expected,
                "got": result["expected_cost"],
                "abs_error": abs(result["expected_cost"] - expected),
                "passed": result["feasible"] and abs(result["expected_cost"] - expected) <= 1e-8,
            }
        )
        checks.append(
            {
                "check": name + "_cost_decomposition",
                "expected": result["expected_cost"],
                "got": cost_sum(result),
                "abs_error": abs(cost_sum(result) - result["expected_cost"]),
                "passed": abs(cost_sum(result) - result["expected_cost"]) <= 1e-8,
            }
        )
        checks.append(
            {
                "check": name + "_absorbing_probability",
                "expected": 1.0,
                "got": result["fulfillment_success_probability"],
                "abs_error": abs(result["fulfillment_success_probability"] - 1.0),
                "passed": abs(result["fulfillment_success_probability"] - 1.0) <= 1e-12,
            }
        )

    # Final disassembly with unknown semi-finished products is infeasible under fixed no-diagnosis reuse.
    unknown_strategy = {key: 0 for key in strategy_all_tests(model, False, False, False)}
    unknown_strategy["disassemble_final"] = 1
    unknown_result = evaluate_strategy(model, unknown_strategy)
    checks.append(
        {
            "check": "unknown_reuse_infeasible",
            "expected": False,
            "got": unknown_result["feasible"],
            "passed": not unknown_result["feasible"],
            "reason": unknown_result["infeasible_reason"],
        }
    )

    # Scrap/reset should remain feasible and finite.
    scrap_strategy = strategy_all_tests(model, False, True, False)
    scrap_result = evaluate_strategy(model, scrap_strategy)
    checks.append(
        {
            "check": "scrap_reset_feasible",
            "expected": True,
            "got": scrap_result["feasible"],
            "passed": scrap_result["feasible"] and math.isfinite(scrap_result["expected_cost"]),
        }
    )

    for semi_reuse, final_test, exact_strategy_name in [
        (True, False, "mc_final_and_semi_reuse"),
        (False, False, "mc_final_reuse_only"),
    ]:
        strategy = strategy_all_tests(model, True, semi_reuse, final_test)
        exact = evaluate_strategy(model, strategy)
        sim = simulate_all_known_good_reuse(model, semi_reuse, final_test)
        error = abs(sim["mean"] - exact["expected_cost"])
        checks.append(
            {
                "check": exact_strategy_name,
                "expected": exact["expected_cost"],
                "got": sim["mean"],
                "stderr": sim["stderr"],
                "abs_error": error,
                "trials": sim["trials"],
                "passed": error <= max(0.25, 4 * sim["stderr"]),
            }
        )

    # When all disassembly decisions are zero, the reuse-aware program should reduce
    # to the obsolete no-reuse baseline for all 2^12 inspection combinations.
    max_cost_error = 0.0
    mismatch_count = 0
    compared = 0
    part_ids = part_order(model)
    product_ids = product_order(model)
    for bits in range(2 ** (len(part_ids) + len(product_ids))):
        strategy = {}
        bit_values = [(bits >> i) & 1 for i in range(len(part_ids) + len(product_ids))]
        for idx, part_id in enumerate(part_ids):
            strategy[f"test_{part_id}"] = bit_values[idx]
        for idx, product_id in enumerate(product_ids, start=len(part_ids)):
            strategy[f"test_{product_id}"] = bit_values[idx]
        for product_id in product_ids:
            strategy[f"disassemble_{product_id}"] = 0
        old = old_evaluate_strategy(model, strategy)
        new = evaluate_strategy(model, strategy)
        compared += 1
        old_feasible = bool(old["feasible"])
        new_feasible = bool(new["feasible"])
        if old_feasible != new_feasible:
            mismatch_count += 1
            continue
        if old_feasible:
            error = abs(float(old["expected_cost"]) - float(new["expected_cost"]))
            max_cost_error = max(max_cost_error, error)
            if error > 1e-8:
                mismatch_count += 1
    checks.append(
        {
            "check": "no_disassembly_matches_obsolete_baseline",
            "compared": compared,
            "expected_compared": 4096,
            "mismatch_count": mismatch_count,
            "max_cost_error": max_cost_error,
            "passed": compared == 4096 and mismatch_count == 0 and max_cost_error <= 1e-8,
        }
    )

    feasible_count = 0
    total_count = 0
    key_count = len(part_ids) + 2 * len(product_ids)
    for bits in range(2 ** key_count):
        strategy = {}
        bit_values = [(bits >> i) & 1 for i in range(key_count)]
        cursor = 0
        for part_id in part_ids:
            strategy[f"test_{part_id}"] = bit_values[cursor]
            cursor += 1
        for product_id in product_ids:
            strategy[f"test_{product_id}"] = bit_values[cursor]
            cursor += 1
        for product_id in product_ids:
            strategy[f"disassemble_{product_id}"] = bit_values[cursor]
            cursor += 1
        total_count += 1
        if evaluate_strategy(model, strategy)["feasible"]:
            feasible_count += 1
    checks.append(
        {
            "check": "independent_feasible_strategy_count",
            "total_count": total_count,
            "expected_total_count": 65536,
            "feasible_count": feasible_count,
            "expected_feasible_count": 17060,
            "passed": total_count == 65536 and feasible_count == 17060,
        }
    )

    write_csv(out_dir / "q3_reuse_validation_checks.csv", checks)
    summary = {
        "check_count": len(checks),
        "passed_count": sum(1 for row in checks if row["passed"]),
        "failed_count": sum(1 for row in checks if not row["passed"]),
        "failed_checks": [row for row in checks if not row["passed"]],
    }
    (out_dir / "q3_reuse_validation_summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False, indent=2))
    if summary["failed_count"]:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
