from __future__ import annotations

import csv
import itertools
import json
import math
from dataclasses import dataclass
from functools import lru_cache
from pathlib import Path

from q3_general_decision import Q3Model, load_model, part_order, product_order, strategy_to_dict


COST_KEYS = [
    "purchase_cost",
    "part_test_cost",
    "assembly_cost",
    "product_test_cost",
    "disassembly_cost",
    "exchange_loss_cost",
]


@dataclass(frozen=True)
class EvalResult:
    feasible: bool
    cost: float
    good_prob: float
    known_good: bool
    costs: tuple[float, float, float, float, float, float]
    reason: str = ""


def zero_costs() -> tuple[float, float, float, float, float, float]:
    return (0.0, 0.0, 0.0, 0.0, 0.0, 0.0)


def add_costs(*items: tuple[float, ...]) -> tuple[float, ...]:
    return tuple(math.fsum(values) for values in zip(*items))


def scale_costs(item: tuple[float, ...], factor: float) -> tuple[float, ...]:
    return tuple(value * factor for value in item)


def add_single(item: tuple[float, ...], key: str, value: float) -> tuple[float, ...]:
    values = list(item)
    values[COST_KEYS.index(key)] += value
    return tuple(values)


def total(costs: tuple[float, ...]) -> float:
    return math.fsum(costs)


def costs_to_dict(costs: tuple[float, ...]) -> dict[str, float]:
    return dict(zip(COST_KEYS, costs))


def strategy_label(strategy: dict[str, int]) -> str:
    enabled = [key for key, value in strategy.items() if value]
    return ";".join(enabled) if enabled else "none"


def evaluate_strategy(model: Q3Model, strategy: dict[str, int]) -> dict:
    def raw_good_probability(node_id: str) -> float:
        if node_id in model.parts:
            part = model.parts[node_id]
            return 1.0 if strategy[f"test_{node_id}"] else 1.0 - part.defect_rate
        product = model.products[node_id]
        if strategy[f"test_{node_id}"]:
            return 1.0
        return math.prod(raw_good_probability(child) for child in product.children) * (1.0 - product.defect_rate)

    final_single_assembly_good_probability = math.prod(
        raw_good_probability(child) for child in model.products[model.root_id].children
    ) * (1.0 - model.products[model.root_id].defect_rate)

    @lru_cache(None)
    def allowed(node_id: str) -> EvalResult:
        if node_id in model.parts:
            part = model.parts[node_id]
            if strategy[f"test_{node_id}"]:
                q = 1.0 - part.defect_rate
                if q <= 0:
                    return EvalResult(False, math.inf, 0.0, False, zero_costs(), "part cannot pass inspection")
                costs = zero_costs()
                costs = add_single(costs, "purchase_cost", part.buy_cost / q)
                costs = add_single(costs, "part_test_cost", part.test_cost / q)
                return EvalResult(True, total(costs), 1.0, True, costs)
            costs = zero_costs()
            costs = add_single(costs, "purchase_cost", part.buy_cost)
            return EvalResult(True, part.buy_cost, 1.0 - part.defect_rate, False, costs)

        product = model.products[node_id]
        if strategy[f"test_{node_id}"]:
            return known_good(node_id)

        child_results = [allowed(child) for child in product.children]
        if not all(child.feasible for child in child_results):
            return EvalResult(False, math.inf, 0.0, False, zero_costs(), "child infeasible")
        child_costs = add_costs(*(child.costs for child in child_results))
        costs = add_single(child_costs, "assembly_cost", product.assemble_cost)
        q = math.prod(child.good_prob for child in child_results) * (1.0 - product.defect_rate)
        return EvalResult(True, total(costs), q, False, costs)

    @lru_cache(None)
    def known_good(node_id: str) -> EvalResult:
        if node_id in model.parts:
            part = model.parts[node_id]
            if not strategy[f"test_{node_id}"]:
                return EvalResult(False, math.inf, 0.0, False, zero_costs(), "untested part cannot be known good")
            q = 1.0 - part.defect_rate
            if q <= 0:
                return EvalResult(False, math.inf, 0.0, False, zero_costs(), "part cannot pass inspection")
            costs = zero_costs()
            costs = add_single(costs, "purchase_cost", part.buy_cost / q)
            costs = add_single(costs, "part_test_cost", part.test_cost / q)
            return EvalResult(True, total(costs), 1.0, True, costs)

        product = model.products[node_id]
        child_results = [allowed(child) for child in product.children]
        if not all(child.feasible for child in child_results):
            return EvalResult(False, math.inf, 0.0, False, zero_costs(), "child infeasible")
        q = math.prod(child.good_prob for child in child_results) * (1.0 - product.defect_rate)
        if q <= 0:
            return EvalResult(False, math.inf, 0.0, False, zero_costs(), "product cannot be good")

        child_cost_once = add_costs(*(child.costs for child in child_results))
        attempt_cost = add_single(child_cost_once, "assembly_cost", product.assemble_cost)
        attempt_cost = add_single(attempt_cost, "product_test_cost", product.test_cost)

        if strategy[f"disassemble_{node_id}"]:
            if not all(child.known_good for child in child_results):
                return EvalResult(False, math.inf, 0.0, False, zero_costs(), "disassembly may recycle unknown bad child")
            # Children are paid once. Failed assemblies only repeat assembly and product testing.
            repeat_costs = zero_costs()
            repeat_costs = add_single(repeat_costs, "assembly_cost", product.assemble_cost / q)
            repeat_costs = add_single(repeat_costs, "product_test_cost", product.test_cost / q)
            repeat_costs = add_single(repeat_costs, "disassembly_cost", product.disassemble_cost * (1.0 - q) / q)
            costs = add_costs(child_cost_once, repeat_costs)
            return EvalResult(True, total(costs), 1.0, True, costs)

        costs = scale_costs(attempt_cost, 1.0 / q)
        return EvalResult(True, total(costs), 1.0, True, costs)

    root = model.products[model.root_id]
    if strategy[f"test_{model.root_id}"]:
        result = known_good(model.root_id)
        if not result.feasible:
            return {
                "feasible": False,
                "infeasible_reason": result.reason,
                "expected_cost": math.inf,
                "expected_profit": -math.inf,
                "single_assembly_good_probability": 0.0,
                "fulfillment_success_probability": 0.0,
                **costs_to_dict(result.costs),
            }
        return {
            "feasible": True,
            "infeasible_reason": "",
            "expected_cost": result.cost,
            "expected_profit": root.price - result.cost,
            "single_assembly_good_probability": final_single_assembly_good_probability,
            "fulfillment_success_probability": 1.0,
            **costs_to_dict(result.costs),
        }

    child_results = [allowed(child) for child in root.children]
    if not all(child.feasible for child in child_results):
        return {
            "feasible": False,
            "infeasible_reason": "child infeasible",
            "expected_cost": math.inf,
            "expected_profit": -math.inf,
            "single_assembly_good_probability": 0.0,
            "fulfillment_success_probability": 0.0,
            **costs_to_dict(zero_costs()),
        }
    q = math.prod(child.good_prob for child in child_results) * (1.0 - root.defect_rate)
    if q <= 0:
        return {
            "feasible": False,
            "infeasible_reason": "final product cannot be good",
            "expected_cost": math.inf,
            "expected_profit": -math.inf,
            "single_assembly_good_probability": q,
            "fulfillment_success_probability": 0.0,
            **costs_to_dict(zero_costs()),
        }
    child_cost_once = add_costs(*(child.costs for child in child_results))
    if strategy[f"disassemble_{model.root_id}"]:
        if not all(child.known_good for child in child_results):
            return {
                "feasible": False,
                "infeasible_reason": "final disassembly may recycle unknown bad child",
                "expected_cost": math.inf,
                "expected_profit": -math.inf,
                "single_assembly_good_probability": q,
                "fulfillment_success_probability": 0.0,
                **costs_to_dict(child_cost_once),
            }
        repeat_costs = zero_costs()
        repeat_costs = add_single(repeat_costs, "assembly_cost", root.assemble_cost / q)
        repeat_costs = add_single(repeat_costs, "exchange_loss_cost", root.exchange_loss * (1.0 - q) / q)
        repeat_costs = add_single(repeat_costs, "disassembly_cost", root.disassemble_cost * (1.0 - q) / q)
        costs = add_costs(child_cost_once, repeat_costs)
    else:
        attempt_cost = add_single(child_cost_once, "assembly_cost", root.assemble_cost)
        costs = scale_costs(attempt_cost, 1.0 / q)
        costs = add_single(costs, "exchange_loss_cost", root.exchange_loss * (1.0 - q) / q)
    cost = total(costs)
    return {
        "feasible": True,
        "infeasible_reason": "",
        "expected_cost": cost,
        "expected_profit": root.price - cost,
        "single_assembly_good_probability": q,
        "fulfillment_success_probability": 1.0,
        **costs_to_dict(costs),
    }


def iter_strategies(model: Q3Model):
    key_count = len(model.parts) + 2 * len(model.products)
    for bits in itertools.product([0, 1], repeat=key_count):
        yield strategy_to_dict(model, bits)


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


def target_strategy(model: Q3Model, semi_disassemble: bool = False) -> dict[str, int]:
    strategy = {key: 0 for key in strategy_to_dict(model, (0,) * (len(model.parts) + 2 * len(model.products)))}
    for part_id in part_order(model):
        strategy[f"test_{part_id}"] = 1
    for product_id in product_order(model):
        if product_id != model.root_id:
            strategy[f"test_{product_id}"] = 1
            strategy[f"disassemble_{product_id}"] = int(semi_disassemble)
    strategy[f"test_{model.root_id}"] = 0
    strategy[f"disassemble_{model.root_id}"] = 1
    return strategy


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    model = load_model(root / "data" / "q3_table2.json")
    out_dir = root / "results" / "q3"

    rows = []
    for idx, strategy in enumerate(iter_strategies(model), 1):
        result = evaluate_strategy(model, strategy)
        rows.append(
            {
                "strategy_id": idx,
                "strategy": strategy_label(strategy),
                **strategy,
                **result,
            }
        )
    write_csv(out_dir / "q3_all_strategies_reuse.csv", rows)
    feasible = [row for row in rows if row["feasible"]]
    feasible.sort(key=lambda row: (-row["expected_profit"], row["strategy_id"]))
    write_csv(out_dir / "q3_top20_strategies_reuse.csv", feasible[:20])

    best_profit = feasible[0]["expected_profit"] if feasible else -math.inf
    tied_best = [row for row in feasible if abs(row["expected_profit"] - best_profit) <= 1e-9]
    write_csv(out_dir / "q3_tied_best_strategies_reuse.csv", tied_best)

    unit_basic = evaluate_strategy(model, target_strategy(model, semi_disassemble=False))
    unit_full = evaluate_strategy(model, target_strategy(model, semi_disassemble=True))
    summary = {
        "model_scope": "assembly tree fixed strategies with known-good recovered child reuse",
        "strategy_count": len(rows),
        "feasible_count": len(feasible),
        "best_strategy": feasible[0] if feasible else None,
        "unit_check_final_reuse_only": unit_basic,
        "unit_check_final_and_semi_reuse": unit_full,
        "unit_check_targets": {
            "final_reuse_only_expected_cost_approx": 147.0370,
            "final_and_semi_reuse_expected_cost_approx": 139.7778,
        },
    }
    (out_dir / "q3_summary_reuse.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
