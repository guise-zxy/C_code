# Obsolete Q3 Baseline Without Reuse

The following old files are retained only as obsolete baseline records and must not be used as formal Q3 results:

```text
q3_all_strategies_minimal.csv
q3_top20_strategies_minimal.csv
q3_summary_minimal.json
```

Reason:

```text
The old Q3 script counted disassembly cost but did not preserve and reuse recovered children.
Therefore the old profit 39.3416, old best strategy, and old feasible count 49152 are withdrawn.
```

Formal Q3 calculations should use the reuse-aware outputs:

```text
q3_all_strategies_reuse.csv
q3_top20_strategies_reuse.csv
q3_tied_best_strategies_reuse.csv
q3_summary_reuse.json
```

The formal entry point is:

```text
problem_b/code/run_q1_q2_q3.py
```

The formal entry point does not rerun `q3_general_decision.py`.
