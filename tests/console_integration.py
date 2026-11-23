"""Run with: python tests/console_integration.py path/to/FreshCartConsole[.exe]."""
from pathlib import Path
import subprocess
import sys

exe = str(Path(sys.argv[1]).resolve())
output = Path(sys.argv[2]) if len(sys.argv) > 2 else None
if output:
    output.mkdir(parents=True, exist_ok=True)

cases = [
    ("01_standard", "2\n107\n2\n4\n76\n5\n6\n", ["TOTAL:           76.00", "Change:          0.00", "Revenue: EGP 76.00"]),
    ("02_member", "2\n101\n2\n2\n111\n6\n3\n1\n4\n700\n6\n", ["TOTAL:           621.30", "Change:          78.70", "Discount (5%):  32.70"]),
    ("03_gold_validation", "hello\n9\n2\n111\n-2\n29\n11\n3\n1\n4\n900\n1000.001\nNaN\n1000\n5\n6\n", ["TOTAL:           940.50", "Change:          59.50", "Discount (10%):  104.50", "Invalid input.", "Invalid cash.", "Revenue: EGP 940.50"]),
]
for name, inputs, expected in cases:
    run = subprocess.run([exe], input=inputs, text=True, capture_output=True, timeout=10)
    if run.returncode != 0 or any(text not in run.stdout for text in expected):
        raise AssertionError(f"Failed {name}: {run.stdout}\n{run.stderr}")
    if output:
        (output / (name + ".txt")).write_text("INPUT LINES\n" + inputs + "\nACTUAL PROGRAM OUTPUT\n" + run.stdout)
    print(name + ": PASS")
# EOF should terminate cleanly rather than trapping the user in a validation loop.
eof = subprocess.run([exe], input="bad\n", text=True, capture_output=True, timeout=5)
assert eof.returncode == 0
print("EOF handling: PASS")
