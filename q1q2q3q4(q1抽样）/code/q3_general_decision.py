from __future__ import annotations

import csv
import itertools
import json
import math
from dataclasses import dataclass
from functools import lru_cache
from pathlib import Path


@dataclass(frozen=True)
class PartSpec:
    id: str
    defect_rate: float
    buy_cost: float
    test_cost: float


@dataclass(frozen=True)
class ProductSpec:
    id: str
    kind: str
    children: tuple[str, ...]
    defect_rate: float
    assemble_cost: float
    test_cost: float
    disassemble_cost: float
    price: float = 0.0
    exchange_loss: float = 0.0


@dataclass(frozen=True)
class Q3Model:
    parts: dict[str, PartSpec]
    products: dict[str, ProductSpec]
    root_id: str = "final"


def load_model(path: Path) -> Q3Model:
    data = json.loads(path.read_text(encoding="utf-8"))
    parts = {row["id"]: PartSpec(**row) for row in data["parts"]}
    products = {}
    for row in data["products"]:
        item = row.copy()
        item["children"] = tuple(item["children"])
        products[item["id"]] = ProductSpec(**item)
    return Q3Model(parts=parts, products=products)


def product_order(model: Q3Model) -> list[str]:
    order = []
    seen = set()

    def visit(node_id: str) -> None:
        if node_id in model.parts:
            return
        if node_id in seen:
            return
        for child in model.products[node_id].children:
            visit(child)
        seen.add(node_id)
        order.append(node_id)

    visit(model.root_id)
    return order


def part_order(model: Q3Model) -> list[str]:
    return sorted(model.parts)


def strategy_to_dict(model: Q3Model, bits: tuple[int, ...]) -> dict[str, int]:
    keys = []
    for part_id in part_order(model):
        keys.append(f"test_{part_id}")
    for product_id in product_order(model):
        keys.append(f"test_{product_id}")
    for product_id in product_order(model):
        keys.append(f"disassemble_{product_id}")
    return dict(zip(keys, bits))


def iter_strategies(model: Q3Model):
    key_count = len(model.parts) + 2 * len(model.products)
    for bits in itertools.product([0, 1], repeat=key_count):
        yield strategy_to_dict(model, bits)


def part_expected_cost(part: PartSpec, do_test: bool) -> tuple[float, dict[str, float]]:
    if not do_test:
        return part.buy_cost, {
            "purchase_cost": part.buy_cost,
            "part_test_cost": 0.0,
            "assembly_cost": 0.0,
            "product_test_cost": 0.0,
            "disassembly_cost": 0.0,
            "exchange_loss_cost": 0.0,
        }
    q = 1.0 - part.defect_rate
    if q <= 0:
        return math.inf, {
            "purchase_cost": math.inf,
            "part_test_cost": math.inf,
            "assembly_cost": 0.0,
            "product_test_cost": 0.0,
            "disassembly_cost": 0.0,
            "exchange_loss_cost": 0.0,
        }
    return (part.buy_cost + part.test_cost) / q, {
        "purchase_cost": part.buy_cost / q,
        "part_test_cost": part.test_cost / q,
        "assembly_cost": 0.0,
        "product_test_cost": 0.0,
        "disassembly_cost": 0.0,
        "exchange_loss_cost": 0.0,
    }


def merge_costs(items: list[dict[str, float]]) -> dict[str, float]:
    keys = ["purchase_cost", "part_test_cost", "assembly_cost", "product_test_cost", "disassembly_cost", "exchange_loss_cost"]
    return {key: math.fsum(item.get(key, 0.0) for item in items) for key in keys}


def total_cost(parts: dict[str, float]) -> float:
    return math.fsum(parts.values())


def evaluate_strategy(model: Q3Model, strategy: dict[str, int]) -> dict:
    @lru_cache(None)
    def eval_node(node_id: str) -> tuple[float, float, dict[str, float], bool]:
        """Return cost, good probability, cost breakdown, feasible for one allowed output."""
        if node_id in model.parts:
            part = model.parts[node_id]
            cost, costs = part_expected_cost(part, bool(strategy[f"test_{node_id}"]))
            good_prob = 1.0 if strategy[f"test_{node_id}"] else 1.0 - part.defect_rate
            return cost, good_prob, costs, math.isfinite(cost)

        product = model.products[node_id]
        child_results = [eval_node(child) for child in product.children]
        if not all(item[3] for item in child_results):
            return math.inf, 0.0, merge_costs([item[2] for item in child_results]), False

        child_costs = [item[0] for item in child_results]
        child_good_probs = [item[1] for item in child_results]
        child_breakdowns = [item[2] for item in child_results]
        base_breakdown = merge_costs(child_breakdowns)
        base_child_cost = math.fsum(child_costs)
        p_good_once = math.prod(child_good_probs) * (1.0 - product.defect_rate)

        do_test = bool(strategy[f"test_{node_id}"])
        do_disassemble = bool(strategy[f"disassemble_{node_id}"])

        if p_good_once <= 0:
            return math.inf, 0.0, base_breakdown, False

        if product.kind == "final" and not do_test:
            # Market failures trigger exchange and a fresh production cycle.
            q = p_good_once
            expected_attempts = 1.0 / q
            fail_per_success = (1.0 - q) / q
            breakdown = {k: v * expected_attempts for k, v in base_breakdown.items()}
            breakdown["assembly_cost"] += product.assemble_cost * expected_attempts
            breakdown["exchange_loss_cost"] += product.exchange_loss * fail_per_success
            if do_disassemble:
                # Conservative feasibility rule: disassembling without full observability can trap bad upstream items.
                return math.inf, 0.0, breakdown, False
            cost = total_cost(breakdown)
            return cost, 1.0, breakdown, True

        if do_test:
            if do_disassemble:
                # Minimal Q3 baseline: tested failures are not reused recursively; disassembly benefit is
                # left for the later dynamic/belief model to avoid false precision.
                pass
            q = p_good_once
            expected_attempts = 1.0 / q
            fail_per_success = (1.0 - q) / q
            breakdown = {k: v * expected_attempts for k, v in base_breakdown.items()}
            breakdown["assembly_cost"] += product.assemble_cost * expected_attempts
            breakdown["product_test_cost"] += product.test_cost * expected_attempts
            if do_disassemble:
                breakdown["disassembly_cost"] += product.disassemble_cost * fail_per_success
            cost = total_cost(breakdown)
            return cost, 1.0, breakdown, True

        # Untested semi-finished items flow upward with their quality probability.
        breakdown = base_breakdown
        breakdown["assembly_cost"] += product.assemble_cost
        cost = total_cost(breakdown)
        return cost, p_good_once, breakdown, True

    cost, good_prob, breakdown, feasible = eval_node(model.root_id)
    root = model.products[model.root_id]
    row = {
        "feasible": feasible,
        "expected_cost": cost,
        "expected_profit": root.price - cost if feasible else -math.inf,
        "root_good_probability_before_market_or_test": good_prob,
        **breakdown,
    }
    return row


def compact_strategy(strategy: dict[str, int]) -> str:
    enabled = [key for key, value in strategy.items() if value]
    return ";".join(enabled) if enabled else "none"


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
                "strategy": compact_strategy(strategy),
                **strategy,
                **result,
            }
        )
    write_csv(out_dir / "q3_all_strategies_minimal.csv", rows)
    feasible = [row for row in rows if row["feasible"]]
    feasible.sort(key=lambda row: -row["expected_profit"])
    write_csv(out_dir / "q3_top20_strategies_minimal.csv", feasible[:20])
    summary = {
        "model_scope": "minimal general multi-stage baseline; recursive disassembly reuse is reserved for the later dynamic/belief model",
        "strategy_count": len(rows),
        "feasible_count": len(feasible),
        "best_strategy": feasible[0] if feasible else None,
    }
    (out_dir / "q3_summary_minimal.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
