from __future__ import annotations

import csv
import itertools
import json
import math
from collections import deque
from dataclasses import dataclass
from pathlib import Path


QUALITY_STATES = ((1, 1), (1, 0), (0, 1), (0, 0))
METRICS = (
    "total_cost",
    "purchase_cost",
    "part_test_cost",
    "assembly_cost",
    "final_test_cost",
    "exchange_loss_cost",
    "disassembly_cost",
    "exchange_count",
    "disassembly_count",
    "assembly_count",
    "market_bad_count",
)


@dataclass(frozen=True)
class Case:
    case_id: int
    p1: float
    buy1: float
    test1: float
    p2: float
    buy2: float
    test2: float
    pf: float
    assemble: float
    test_final: float
    price: float
    exchange_loss: float
    disassemble: float


def read_cases(path: Path) -> list[Case]:
    with path.open("r", encoding="utf-8-sig", newline="") as f:
        rows = csv.DictReader(f)
        cases = []
        for row in rows:
            values = {k: float(v) for k, v in row.items()}
            values["case_id"] = int(values["case_id"])
            cases.append(Case(**values))
        return cases


def belief_key(belief: tuple[float, float, float, float]) -> tuple[float, float, float, float]:
    cleaned = []
    for x in belief:
        if abs(x) < 1e-13:
            x = 0.0
        cleaned.append(round(x, 12))
    s = sum(cleaned)
    if s <= 0:
        raise ValueError("invalid zero belief")
    return tuple(round(x / s, 12) for x in cleaned)


def add_metric(target: dict[str, float], name: str, value: float) -> None:
    target[name] = target.get(name, 0.0) + value
    if name.endswith("_cost"):
        target["total_cost"] = target.get("total_cost", 0.0) + value


def condition_belief(belief: tuple[float, ...], part_index: int, quality: int) -> tuple[float, ...] | None:
    weights = [prob if state[part_index] == quality else 0.0 for prob, state in zip(belief, QUALITY_STATES)]
    s = sum(weights)
    if s <= 1e-14:
        return None
    return belief_key(tuple(w / s for w in weights))


def set_part_good(belief: tuple[float, ...], part_index: int) -> tuple[float, ...]:
    other_index = 1 - part_index
    other_good = sum(prob for prob, state in zip(belief, QUALITY_STATES) if state[other_index] == 1)
    if part_index == 0:
        return belief_key((other_good, 1.0 - other_good, 0.0, 0.0))
    return belief_key((other_good, 0.0, 1.0 - other_good, 0.0))


def product_good_prob(belief: tuple[float, ...], pf: float) -> float:
    return belief[0] * (1.0 - pf)


def condition_on_bad_product(belief: tuple[float, ...], pf: float) -> tuple[float, ...]:
    weights = []
    for prob, state in zip(belief, QUALITY_STATES):
        likelihood_good = (1.0 - pf) if state == (1, 1) else 0.0
        weights.append(prob * (1.0 - likelihood_good))
    s = sum(weights)
    if s <= 1e-14:
        raise ValueError("bad product observation has zero probability")
    return belief_key(tuple(w / s for w in weights))


def initial_belief(case: Case) -> tuple[float, float, float, float]:
    q1 = 1.0 - case.p1
    q2 = 1.0 - case.p2
    return belief_key((q1 * q2, q1 * case.p2, case.p1 * q2, case.p1 * case.p2))


def replacement_cost(case: Case, part_index: int) -> dict[str, float]:
    p = case.p1 if part_index == 0 else case.p2
    buy = case.buy1 if part_index == 0 else case.buy2
    test = case.test1 if part_index == 0 else case.test2
    q = 1.0 - p
    if q <= 0:
        return {"total_cost": math.inf, "purchase_cost": math.inf, "part_test_cost": math.inf}
    return {
        "total_cost": (buy + test) / q,
        "purchase_cost": buy / q,
        "part_test_cost": test / q,
    }


def prepare_distribution(
    case: Case,
    strategy: tuple[int, int, int, int],
    belief: tuple[float, ...],
    retest_known_good: bool = False,
) -> list[tuple[float, tuple[float, ...], dict[str, float]]]:
    d1, d2, _, _ = strategy
    branches = [(1.0, belief_key(belief), {metric: 0.0 for metric in METRICS})]

    for part_index, should_test in enumerate((d1, d2)):
        if not should_test:
            continue
        next_branches = []
        test_cost = case.test1 if part_index == 0 else case.test2
        for branch_prob, branch_belief, metrics in branches:
            bad_prob = sum(prob for prob, state in zip(branch_belief, QUALITY_STATES) if state[part_index] == 0)
            good_prob = 1.0 - bad_prob
            if bad_prob <= 1e-13:
                next_metrics = metrics.copy()
                if retest_known_good:
                    add_metric(next_metrics, "part_test_cost", test_cost)
                next_branches.append((branch_prob, branch_belief, next_metrics))
                continue

            if good_prob > 1e-13:
                good_belief = condition_belief(branch_belief, part_index, 1)
                good_metrics = metrics.copy()
                add_metric(good_metrics, "part_test_cost", test_cost)
                next_branches.append((branch_prob * good_prob, good_belief, good_metrics))

            bad_belief = condition_belief(branch_belief, part_index, 0)
            replaced_belief = set_part_good(bad_belief, part_index)
            bad_metrics = metrics.copy()
            add_metric(bad_metrics, "part_test_cost", test_cost)
            repl = replacement_cost(case, part_index)
            for name in ("purchase_cost", "part_test_cost"):
                add_metric(bad_metrics, name, repl[name])
            next_branches.append((branch_prob * bad_prob, replaced_belief, bad_metrics))
        branches = next_branches

    merged: dict[tuple[float, ...], tuple[float, dict[str, float]]] = {}
    for prob, b, metrics in branches:
        key = belief_key(b)
        if key not in merged:
            merged[key] = (0.0, {metric: 0.0 for metric in METRICS})
        old_prob, old_metrics = merged[key]
        for name in METRICS:
            old_metrics[name] += prob * metrics.get(name, 0.0)
        merged[key] = (old_prob + prob, old_metrics)

    result = []
    for key, (prob, weighted_metrics) in merged.items():
        result.append((prob, key, {name: weighted_metrics[name] / prob for name in METRICS}))
    return result


def expand_state(
    state: str | tuple[float, ...],
    case: Case,
    strategy: tuple[int, int, int, int],
    retest_known_good: bool = False,
) -> tuple[dict[str, float], dict[str | tuple[float, ...], float], float]:
    immediate = {metric: 0.0 for metric in METRICS}
    transitions: dict[str | tuple[float, ...], float] = {}
    success_prob = 0.0

    if state == "START":
        add_metric(immediate, "purchase_cost", case.buy1 + case.buy2)
        transitions[initial_belief(case)] = 1.0
        return immediate, transitions, success_prob

    _, _, df, dr = strategy
    for prep_prob, prepared_belief, prep_metrics in prepare_distribution(case, strategy, state, retest_known_good):
        for name in METRICS:
            immediate[name] += prep_prob * prep_metrics.get(name, 0.0)
        add_metric(immediate, "assembly_cost", prep_prob * case.assemble)
        immediate["assembly_count"] += prep_prob

        good_prob = product_good_prob(prepared_belief, case.pf)
        bad_prob = 1.0 - good_prob

        if df:
            add_metric(immediate, "final_test_cost", prep_prob * case.test_final)
            success_prob += prep_prob * good_prob
            if bad_prob > 1e-13:
                if dr:
                    add_metric(immediate, "disassembly_cost", prep_prob * bad_prob * case.disassemble)
                    immediate["disassembly_count"] += prep_prob * bad_prob
                    next_state = condition_on_bad_product(prepared_belief, case.pf)
                    transitions[next_state] = transitions.get(next_state, 0.0) + prep_prob * bad_prob
                else:
                    transitions["START"] = transitions.get("START", 0.0) + prep_prob * bad_prob
        else:
            success_prob += prep_prob * good_prob
            if bad_prob > 1e-13:
                add_metric(immediate, "exchange_loss_cost", prep_prob * bad_prob * case.exchange_loss)
                immediate["exchange_count"] += prep_prob * bad_prob
                immediate["market_bad_count"] += prep_prob * bad_prob
                if dr:
                    add_metric(immediate, "disassembly_cost", prep_prob * bad_prob * case.disassemble)
                    immediate["disassembly_count"] += prep_prob * bad_prob
                    next_state = condition_on_bad_product(prepared_belief, case.pf)
                    transitions[next_state] = transitions.get(next_state, 0.0) + prep_prob * bad_prob
                else:
                    transitions["START"] = transitions.get("START", 0.0) + prep_prob * bad_prob

    return immediate, transitions, success_prob


def build_chain(case: Case, strategy: tuple[int, int, int, int], retest_known_good: bool = False):
    states: list[str | tuple[float, ...]] = ["START"]
    seen = {"START"}
    queue = deque(["START"])
    expanded = {}
    while queue:
        state = queue.popleft()
        immediate, transitions, success_prob = expand_state(state, case, strategy, retest_known_good)
        expanded[state] = (immediate, transitions, success_prob)
        for next_state in transitions:
            if next_state not in seen:
                seen.add(next_state)
                states.append(next_state)
                queue.append(next_state)
    return states, expanded


def solve_linear(a: list[list[float]], b: list[float]) -> list[float]:
    n = len(b)
    aug = [row[:] + [rhs] for row, rhs in zip(a, b)]
    for col in range(n):
        pivot = max(range(col, n), key=lambda r: abs(aug[r][col]))
        if abs(aug[pivot][col]) < 1e-11:
            raise ValueError("singular linear system")
        aug[col], aug[pivot] = aug[pivot], aug[col]
        div = aug[col][col]
        for j in range(col, n + 1):
            aug[col][j] /= div
        for r in range(n):
            if r == col:
                continue
            factor = aug[r][col]
            if abs(factor) < 1e-15:
                continue
            for j in range(col, n + 1):
                aug[r][j] -= factor * aug[col][j]
    return [aug[i][n] for i in range(n)]


def evaluate_strategy(case: Case, strategy: tuple[int, int, int, int], retest_known_good: bool = False) -> dict:
    states, expanded = build_chain(case, strategy, retest_known_good)
    index = {state: i for i, state in enumerate(states)}
    n = len(states)
    matrix = [[0.0 for _ in range(n)] for _ in range(n)]
    for state in states:
        i = index[state]
        matrix[i][i] = 1.0
        _, transitions, _ = expanded[state]
        for next_state, prob in transitions.items():
            matrix[i][index[next_state]] -= prob

    try:
        success_rhs = [expanded[state][2] for state in states]
        success_prob = solve_linear(matrix, success_rhs)[index["START"]]
        if success_prob < 1.0 - 1e-7:
            raise ValueError("success is not reached with probability 1")

        solved_metrics = {}
        for metric in METRICS:
            rhs = [expanded[state][0].get(metric, 0.0) for state in states]
            solved_metrics[metric] = solve_linear(matrix, rhs)[index["START"]]
        if any(not math.isfinite(value) for value in solved_metrics.values()):
            raise ValueError("non-finite expected metric")
    except ValueError as exc:
        return {
            "feasible": False,
            "infeasible_reason": str(exc),
            "state_count": n,
            "expected_cost": math.inf,
            "expected_profit": -math.inf,
            "success_probability": 0.0,
        }

    market_bad = solved_metrics["market_bad_count"]
    market_total = market_bad + 1.0
    expected_cost = solved_metrics["total_cost"]
    return {
        "feasible": True,
        "infeasible_reason": "",
        "state_count": n,
        "success_probability": success_prob,
        "expected_cost": expected_cost,
        "expected_profit": case.price - expected_cost,
        "market_defect_rate": market_bad / market_total,
        "expected_exchange_count": solved_metrics["exchange_count"],
        "expected_disassembly_count": solved_metrics["disassembly_count"],
        "expected_assembly_count": solved_metrics["assembly_count"],
        "expected_purchase_cost": solved_metrics["purchase_cost"],
        "expected_part_test_cost": solved_metrics["part_test_cost"],
        "expected_assembly_cost": solved_metrics["assembly_cost"],
        "expected_final_test_cost": solved_metrics["final_test_cost"],
        "expected_exchange_loss_cost": solved_metrics["exchange_loss_cost"],
        "expected_disassembly_cost": solved_metrics["disassembly_cost"],
    }


def strategy_rows(cases: list[Case], retest_known_good: bool = False) -> list[dict]:
    rows = []
    policy_name = "retest_known_good" if retest_known_good else "no_retest_known_good"
    for case in cases:
        for strategy in itertools.product([0, 1], repeat=4):
            result = evaluate_strategy(case, strategy, retest_known_good)
            rows.append(
                {
                    "inspection_policy": policy_name,
                    "case_id": case.case_id,
                    "d1_test_part1": strategy[0],
                    "d2_test_part2": strategy[1],
                    "df_test_final": strategy[2],
                    "dr_disassemble": strategy[3],
                    **result,
                }
            )
    return rows


def strategy_label(row: dict) -> str:
    return f"({row['d1_test_part1']},{row['d2_test_part2']},{row['df_test_final']},{row['dr_disassemble']})"


def ranked_rows(rows: list[dict], top_n: int = 3, tie_tol: float = 1e-9) -> tuple[list[dict], list[dict], list[dict]]:
    top_rows = []
    tie_rows = []
    compare_rows = []
    cases = sorted({int(r["case_id"]) for r in rows})
    policies = sorted({r["inspection_policy"] for r in rows})
    for policy in policies:
        for case_id in cases:
            candidates = [r for r in rows if r["inspection_policy"] == policy and int(r["case_id"]) == case_id and r["feasible"]]
            candidates.sort(key=lambda r: (-float(r["expected_profit"]), strategy_label(r)))
            if not candidates:
                continue
            best_profit = float(candidates[0]["expected_profit"])
            for rank, row in enumerate(candidates[:top_n], 1):
                out = row.copy()
                out["rank"] = rank
                out["strategy"] = strategy_label(row)
                out["profit_gap_to_best"] = best_profit - float(row["expected_profit"])
                top_rows.append(out)
            tied = [r for r in candidates if abs(float(r["expected_profit"]) - best_profit) <= tie_tol]
            for row in tied:
                out = row.copy()
                out["strategy"] = strategy_label(row)
                tie_rows.append(out)
            gap_1_2 = None
            gap_1_3 = None
            if len(candidates) >= 2:
                gap_1_2 = best_profit - float(candidates[1]["expected_profit"])
            if len(candidates) >= 3:
                gap_1_3 = best_profit - float(candidates[2]["expected_profit"])
            compare_rows.append(
                {
                    "inspection_policy": policy,
                    "case_id": case_id,
                    "best_strategy": strategy_label(candidates[0]),
                    "best_profit": best_profit,
                    "second_strategy": strategy_label(candidates[1]) if len(candidates) >= 2 else "",
                    "second_profit": candidates[1]["expected_profit"] if len(candidates) >= 2 else "",
                    "gap_best_second": gap_1_2 if gap_1_2 is not None else "",
                    "third_strategy": strategy_label(candidates[2]) if len(candidates) >= 3 else "",
                    "third_profit": candidates[2]["expected_profit"] if len(candidates) >= 3 else "",
                    "gap_best_third": gap_1_3 if gap_1_3 is not None else "",
                    "tie_count": len(tied),
                    "all_best_strategies": ";".join(strategy_label(r) for r in tied),
                    "note": "check tie/small gap" if case_id in (3, 5, 6) else "",
                }
            )
    return top_rows, tie_rows, compare_rows


def write_csv(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = list(rows[0])
    with path.open("w", newline="", encoding="utf-8-sig") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    cases = read_cases(root / "data" / "q2_table1.csv")
    out_dir = root / "results" / "q2"
    rows = strategy_rows(cases, retest_known_good=False) + strategy_rows(cases, retest_known_good=True)
    write_csv(out_dir / "q2_static_all_strategies.csv", rows)

    best_rows = []
    for policy in sorted({r["inspection_policy"] for r in rows}):
        for case in cases:
            candidates = [r for r in rows if r["inspection_policy"] == policy and r["case_id"] == case.case_id and r["feasible"]]
            best = max(candidates, key=lambda r: r["expected_profit"])
            best_rows.append(best)
    write_csv(out_dir / "q2_static_best_strategies.csv", best_rows)

    top_rows, tie_rows, compare_rows = ranked_rows(rows)
    write_csv(out_dir / "q2_static_top3_strategies.csv", top_rows)
    write_csv(out_dir / "q2_static_tied_best_strategies.csv", tie_rows)
    write_csv(out_dir / "q2_static_policy_comparison.csv", compare_rows)

    summary = {
        "strategy_count": len(rows),
        "strategy_count_per_policy": len(rows) // 2,
        "best_by_case": best_rows,
        "assumptions": {
            "inspection_accuracy": "perfect",
            "objective": "maximize expected profit for one finally delivered qualified product",
            "revenue": "price counted once; exchanges do not create additional revenue",
            "rework": "scrapped products restart from purchase; disassembled parts keep posterior quality state",
            "known_good_retest": "both no-retest and every-round retest policies are exported",
            "market_defect_rate": "reported as an indicator, not used as a hard constraint",
        },
    }
    (out_dir / "q2_static_summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
