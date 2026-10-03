import csv
import hashlib
import os
import platform
import shutil
import statistics
import subprocess
import sys
import tempfile
import time
from pathlib import Path


# ----------------------------------------------------------------------------
# Paths
# ----------------------------------------------------------------------------
def find_root():
    """Project root = the folder that contains tests/. Works whether runner.py
    sits in the root itself or in a subfolder (e.g. src/)."""
    here = Path(__file__).resolve().parent
    for candidate in (here, here.parent):
        if (candidate / "tests").is_dir():
            return candidate
    return here.parent


ROOT = find_root()
TESTS_DIR = ROOT / "tests"
RESULTS_DIR = ROOT / "results"

# ----------------------------------------------------------------------------
# Settings
# ----------------------------------------------------------------------------
COMPILE_TIMEOUT = 60        # seconds allowed for one compilation
RUN_TIMEOUT = 5             # seconds allowed for one program run
RUNS = 5                    # runs per binary; median is used for timing
OPT_LEVELS = ["-O0", "-O1", "-O2", "-O3"]
PERF_THRESHOLD = 0.10       # >10% slower counts as a performance regression
MIN_ABS_DIFF = 0.005        # ...but only if also >5 ms slower (ignore noise)

IS_WINDOWS = os.name == "nt"

# family = which compiler line a version belongs to. Within a family, the first
# entry is treated as the OLD version and the last as the NEW version.
if IS_WINDOWS:
    # Original local setup (one version each, so only cross-compiler checks)
    COMPILERS = {
        "GCC": {
            "command": r"C:\MinGW\bin\gcc.exe",
            "extra_args": [],
            "family": "gcc",
        },
        "Clang": {
            "command": r"C:\Program Files (x86)\LLVM\bin\clang.exe",
            "extra_args": ["--target=i686-w64-windows-gnu"],
            "family": "clang",
        },
    }
    LINK_ARGS = []
else:
    # Docker / Linux setup (matches the Dockerfile)
    COMPILERS = {
        "gcc-9": {"command": "gcc-9", "extra_args": [], "family": "gcc"},
        "gcc-12": {"command": "gcc-12", "extra_args": [], "family": "gcc"},
        "clang-11": {"command": "clang-11", "extra_args": [], "family": "clang"},
        "clang-15": {"command": "clang-15", "extra_args": [], "family": "clang"},
    }
    LINK_ARGS = ["-lm"]     # math functions need libm on Linux


# ----------------------------------------------------------------------------
# Helpers
# ----------------------------------------------------------------------------
def find_compiler(command):
    if os.path.isfile(command):
        return command
    return shutil.which(command)


def executable_name(base):
    return f"{base}.exe" if IS_WINDOWS else base


def run_command(command, timeout):
    """Run a command. Returns (returncode, stdout, stderr, error, seconds)."""
    start = time.perf_counter()
    try:
        result = subprocess.run(
            command, capture_output=True, text=True, timeout=timeout
        )
        elapsed = time.perf_counter() - start
        return result.returncode, result.stdout, result.stderr, None, elapsed
    except subprocess.TimeoutExpired:
        return None, "", "", "TIMEOUT", time.perf_counter() - start
    except Exception as exc:
        return None, "", "", str(exc), time.perf_counter() - start


def normalize_output(text):
    # Ignore trailing whitespace and final newline differences.
    return "\n".join(line.rstrip() for line in text.strip().splitlines())


def short_hash(text):
    return hashlib.sha256(text.encode("utf-8", "replace")).hexdigest()[:10]


# ----------------------------------------------------------------------------
# Compile + run one (test, compiler, opt level)
# ----------------------------------------------------------------------------
def compile_and_run(source_file, compiler_name, compiler_info, opt_level):
    result = {
        "compiler": compiler_name,
        "opt": opt_level,
        "compile_status": "",
        "run_status": "",
        "exit_code": "",
        "output": "",
        "error": "",
        "compile_time": None,
        "run_time": None,
        "binary_size": None,
    }

    build_dir = Path(tempfile.mkdtemp(prefix="compiler_regression_"))
    executable = build_dir / executable_name(source_file.stem)

    compile_cmd = [
        compiler_info["path"],
        *compiler_info["extra_args"],
        str(source_file),
        opt_level,
        "-o",
        str(executable),
        *LINK_ARGS,
    ]

    try:
        code, stdout, stderr, error, ctime = run_command(
            compile_cmd, COMPILE_TIMEOUT
        )
        result["compile_time"] = ctime

        if error == "TIMEOUT":
            result["compile_status"] = "TIMEOUT"
            result["error"] = "Compilation timed out"
            return result
        if error or code != 0:
            result["compile_status"] = "ERROR"
            result["error"] = (error or stderr.strip() or stdout.strip())[:500]
            return result

        result["compile_status"] = "OK"
        result["binary_size"] = executable.stat().st_size

        # Run several times: first run provides output/exit code,
        # all runs provide timing.
        times = []
        first = None
        for i in range(RUNS):
            rcode, rout, rerr, rerror, rtime = run_command(
                [str(executable)], RUN_TIMEOUT
            )
            if i == 0:
                first = (rcode, rout, rerr, rerror)
            if rerror:
                break
            times.append(rtime)

        rcode, rout, rerr, rerror = first
        result["output"] = rout
        result["exit_code"] = "" if rcode is None else rcode

        if rerror == "TIMEOUT":
            result["run_status"] = "TIMEOUT"
            result["error"] = "Program execution timed out"
        elif rerror:
            result["run_status"] = "RUNTIME ERROR"
            result["error"] = rerror[:500]
        elif rcode != 0:
            result["run_status"] = "RUNTIME ERROR"
            result["error"] = (
                rerr.strip() or rout.strip() or f"Exit code: {rcode}"
            )[:500]
        else:
            result["run_status"] = "PASS"
            result["run_time"] = statistics.median(times)
        return result
    finally:
        shutil.rmtree(build_dir, ignore_errors=True)


def is_ok(r):
    return r["compile_status"] == "OK" and r["run_status"] == "PASS"


def status_text(r):
    if r["compile_status"] != "OK":
        return "COMPILE ERROR"
    return r["run_status"]


# ----------------------------------------------------------------------------
# Regression analysis
# ----------------------------------------------------------------------------
def classify(results):
    """Overall verdict for one (test, opt) across all compilers."""
    rs = list(results.values())
    if any(r["compile_status"] != "OK" for r in rs):
        return "COMPILE ERROR"
    if any(r["run_status"] != "PASS" for r in rs):
        return "RUNTIME ERROR"
    outputs = {normalize_output(r["output"]) for r in rs}
    if len(outputs) > 1:
        return "OUTPUT MISMATCH"
    return "PASS"


def version_pairs(compiler_paths):
    """[(family, old_name, new_name)] for families with >= 2 versions."""
    families = {}
    for name, info in compiler_paths.items():
        families.setdefault(info["family"], []).append(name)
    return [
        (fam, names[0], names[-1])
        for fam, names in families.items()
        if len(names) >= 2
    ]


def find_regressions(test_name, opt, results, pairs):
    found = []
    for family, old_name, new_name in pairs:
        old, new = results[old_name], results[new_name]

        base = {
            "test": test_name,
            "opt": opt,
            "family": family,
            "old_version": old_name,
            "new_version": new_name,
            "old_time": old["run_time"],
            "new_time": new["run_time"],
            "change_pct": "",
            "type": "",
            "detail": "",
        }

        # 1. Compilation regression
        if old["compile_status"] == "OK" and new["compile_status"] != "OK":
            found.append({**base, "type": "COMPILE REGRESSION",
                          "detail": new["error"][:200]})
            continue

        # 2. Run-time failure regression
        if is_ok(old) and not is_ok(new):
            found.append({**base, "type": "RUNTIME REGRESSION",
                          "detail": new["error"][:200] or new["run_status"]})
            continue

        if not (is_ok(old) and is_ok(new)):
            continue

        # 3. Correctness regression
        if normalize_output(old["output"]) != normalize_output(new["output"]):
            found.append({**base, "type": "OUTPUT REGRESSION",
                          "detail": (
                              f"old={short_hash(normalize_output(old['output']))} "
                              f"new={short_hash(normalize_output(new['output']))}"
                          )})
            continue

        # 4. Performance regression
        o, n = old["run_time"], new["run_time"]
        if o and n and n > o * (1 + PERF_THRESHOLD) and (n - o) > MIN_ABS_DIFF:
            pct = (n - o) / o * 100
            found.append({**base, "type": "PERFORMANCE REGRESSION",
                          "change_pct": f"{pct:.1f}",
                          "detail": f"{o*1000:.1f} ms -> {n*1000:.1f} ms"})
    return found


def find_opt_mismatches(test_name, per_opt_results):
    """Same compiler, different -O level, different output => optimizer bug
    (or undefined behaviour in the test program)."""
    found = []
    opts = list(per_opt_results.keys())
    base_opt = opts[0]
    for name in per_opt_results[base_opt]:
        ref = per_opt_results[base_opt][name]
        if not is_ok(ref):
            continue
        for opt in opts[1:]:
            cur = per_opt_results[opt][name]
            if is_ok(cur) and normalize_output(cur["output"]) != normalize_output(ref["output"]):
                found.append({
                    "test": test_name,
                    "opt": f"{base_opt} vs {opt}",
                    "family": "",
                    "old_version": name,
                    "new_version": name,
                    "old_time": ref["run_time"],
                    "new_time": cur["run_time"],
                    "change_pct": "",
                    "type": "OPT-LEVEL MISMATCH",
                    "detail": f"{name}: output differs between {base_opt} and {opt}",
                })
    return found


def write_csv(path, rows, fieldnames=None):
    if not rows and not fieldnames:
        return
    with path.open("w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames or rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)


def fmt_ms(seconds):
    return "-" if seconds is None else f"{seconds * 1000:.1f}"


# ----------------------------------------------------------------------------
# Main
# ----------------------------------------------------------------------------
def main():
    print("=" * 78)
    print("        AUTOMATED COMPILER REGRESSION TESTING FRAMEWORK")
    print("=" * 78)
    print(f"Operating system : {platform.system()}")
    print(f"Optimization lvls: {', '.join(OPT_LEVELS)}")
    print(f"Runs per binary  : {RUNS} (median used)")
    print()

    compiler_paths = {}
    for name, info in COMPILERS.items():
        path = find_compiler(info["command"])
        if path:
            compiler_paths[name] = {
                "path": path,
                "extra_args": info["extra_args"],
                "family": info["family"],
            }
            print(f"[OK] {name}: {path}")
        else:
            print(f"[MISSING] {name}: {info['command']}")
    print()

    if len(compiler_paths) < 2:
        print("ERROR: At least two compilers are required.")
        print("Check the compiler settings at the top of runner.py.")
        sys.exit(1)

    pairs = version_pairs(compiler_paths)
    for fam, old, new in pairs:
        print(f"Version comparison: {fam}: {old} (old) -> {new} (new)")
    if not pairs:
        print("No multi-version family found: only cross-compiler checks run.")
    print()

    test_files = sorted(TESTS_DIR.glob("*.c"))
    if not test_files:
        print(f"No .c test files found in: {TESTS_DIR}")
        sys.exit(1)

    RESULTS_DIR.mkdir(exist_ok=True)

    detail_rows = []       # one row per test/opt/compiler
    summary_rows = []      # one row per test/opt
    regression_rows = []   # regressions found

    names = list(compiler_paths.keys())
    print("-" * 78)
    print(f"{'Test':24} {'Opt':4} {'Result':16} Median runtime (ms) per compiler")
    print("-" * 78)

    for source_file in test_files:
        per_opt = {}

        for opt in OPT_LEVELS:
            results = {
                name: compile_and_run(source_file, name, info, opt)
                for name, info in compiler_paths.items()
            }
            per_opt[opt] = results

            overall = classify(results)
            timings = "  ".join(
                f"{n}={fmt_ms(results[n]['run_time'])}" for n in names
            )
            print(f"{source_file.name:24} {opt:4} {overall:16} {timings}")

            summary_rows.append({
                "test": source_file.name,
                "opt": opt,
                "result": overall,
            })

            for name in names:
                r = results[name]
                detail_rows.append({
                    "test": source_file.name,
                    "opt": opt,
                    "compiler": name,
                    "compile_status": r["compile_status"],
                    "run_status": r["run_status"],
                    "exit_code": r["exit_code"],
                    "compile_time_s": "" if r["compile_time"] is None else f"{r['compile_time']:.4f}",
                    "median_run_time_s": "" if r["run_time"] is None else f"{r['run_time']:.6f}",
                    "binary_size_bytes": "" if r["binary_size"] is None else r["binary_size"],
                    "output": normalize_output(r["output"]),
                    "error": r["error"],
                })

            regression_rows.extend(
                find_regressions(source_file.name, opt, results, pairs)
            )

        regression_rows.extend(find_opt_mismatches(source_file.name, per_opt))

    # ---- write files ----
    write_csv(RESULTS_DIR / "results.csv", detail_rows)
    write_csv(RESULTS_DIR / "summary.csv", summary_rows)
    reg_fields = ["test", "opt", "family", "old_version", "new_version",
                  "type", "change_pct", "old_time", "new_time", "detail"]
    write_csv(RESULTS_DIR / "regressions.csv", regression_rows, reg_fields)

    # ---- final summary ----
    print("-" * 78)
    counts = {}
    for row in summary_rows:
        counts[row["result"]] = counts.get(row["result"], 0) + 1
    print(f"Total test runs (tests x opt levels): {len(summary_rows)}")
    for status, count in counts.items():
        print(f"  {status:20}: {count}")

    print()
    print(f"Regressions found: {len(regression_rows)}")
    by_type = {}
    for row in regression_rows:
        by_type[row["type"]] = by_type.get(row["type"], 0) + 1
    for kind, count in by_type.items():
        print(f"  {kind:24}: {count}")
    for row in regression_rows[:15]:
        print(f"  - {row['test']} {row['opt']} [{row['type']}] "
              f"{row['old_version']} -> {row['new_version']}: {row['detail']}")
    if len(regression_rows) > 15:
        print(f"  ... and {len(regression_rows) - 15} more (see regressions.csv)")

    print()
    print(f"Detailed results : {RESULTS_DIR / 'results.csv'}")
    print(f"Per-test summary : {RESULTS_DIR / 'summary.csv'}")
    print(f"Regression list  : {RESULTS_DIR / 'regressions.csv'}")
    print("=" * 78)


if __name__ == "__main__":
    main()