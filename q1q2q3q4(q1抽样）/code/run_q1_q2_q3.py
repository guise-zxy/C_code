from __future__ import annotations

import subprocess
import sys
from pathlib import Path


def run(script: str) -> None:
    path = Path(__file__).resolve().parent / script
    print(f"\n===== running {script} =====")
    subprocess.run([sys.executable, str(path)], check=True)


def main() -> None:
    run("q1_fixed_sampling.py")
    run("q2_static_enumeration.py")
    run("q2_validation.py")
    run("q2_sensitivity_checks.py")
    run("q3_reuse_decision.py")
    run("q3_reuse_validation.py")


if __name__ == "__main__":
    main()
