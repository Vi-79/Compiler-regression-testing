import csv
import os
import platform
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TESTS_DIR = ROOT / "tests"
RESULTS_DIR = ROOT / "results"
TIMEOUT_SECONDS = 5

# Your installed 32-bit compiler setup
COMPILERS = {
    "GCC": {
        "command": r"C:\MinGW\bin\gcc.exe",
        "extra_args": [],
    },
    "Clang": {
        "command": r"C:\Program Files (x86)\LLVM\bin\clang.exe",
        "extra_args": ["--target=i686-w64-windows-gnu"],
    },
}


def find_compiler(command):
    # Use the configured full path if it exists.
    if os.path.isfile(command):
        return command

    # Otherwise allow Windows PATH lookup.
    return shutil.which(command)


def executable_name(base):
    return f"{base}.exe" if os.name == "nt" else base


def run_command(command, timeout=TIMEOUT_SECONDS):
    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        return result.returncode, result.stdout, result.stderr, None
    except subprocess.TimeoutExpired:
        return None, "", "", "TIMEOUT"
    except Exception as exc:
        return None, "", "", str(exc)


def normalize_output(text):
    # Ignore trailing whitespace and final newline differences.
    return "\n".join(line.rstrip() for line in text.strip().splitlines())


def compile_and_run(source_file, compiler_name, compiler_info):
    compiler_path = compiler_info["path"]
    extra_args = compiler_info["extra_args"]

    build_dir = Path(tempfile.mkdtemp(prefix="compiler_regression_"))
    executable = build_dir / executable_name(source_file.stem)

    compile_cmd = [
        compiler_path,
        *extra_args,
        str(source_file),
        "-O0",
        "-o",
        str(executable),
    ]

    code, stdout, stderr, error = run_command(compile_cmd)

    if error == "TIMEOUT":
        shutil.rmtree(build_dir, ignore_errors=True)
        return {
            "compiler": compiler_name,
            "compile_status": "TIMEOUT",
            "run_status": "",
            "output": "",
            "error": "Compilation timed out",
        }

    if code != 0:
        shutil.rmtree(build_dir, ignore_errors=True)
        return {
            "compiler": compiler_name,
            "compile_status": "ERROR",
            "run_status": "",
            "output": "",
            "error": stderr.strip() or stdout.strip(),
        }

    run_code, run_stdout, run_stderr, run_error = run_command([str(executable)])

    if run_error == "TIMEOUT":
        status = "TIMEOUT"
        error_text = "Program execution timed out"
    elif run_code != 0:
        status = "RUNTIME ERROR"
        error_text = (
            run_stderr.strip()
            or run_stdout.strip()
            or f"Exit code: {run_code}"
        )
    else:
        status = "PASS"
        error_text = ""

    result = {
        "compiler": compiler_name,
        "compile_status": "OK",
        "run_status": status,
        "output": run_stdout,
        "error": error_text,
    }

    shutil.rmtree(build_dir, ignore_errors=True)
    return result


def test_one(source_file, compiler_paths):
    results = {}

    for compiler_name, compiler_info in compiler_paths.items():
        results[compiler_name] = compile_and_run(
            source_file, compiler_name, compiler_info
        )

    gcc = results["GCC"]
    clang = results["Clang"]

    if gcc["compile_status"] != "OK" or clang["compile_status"] != "OK":
        overall = "COMPILE ERROR"
    elif gcc["run_status"] != "PASS" or clang["run_status"] != "PASS":
        overall = "RUNTIME ERROR"
    elif normalize_output(gcc["output"]) != normalize_output(clang["output"]):
        overall = "OUTPUT MISMATCH"
    else:
        overall = "PASS"

    return results, overall


def main():
    print("=" * 72)
    print("        AUTOMATED COMPILER REGRESSION TESTING FRAMEWORK")
    print("=" * 72)
    print(f"Operating system: {platform.system()}")
    print()

    compiler_paths = {}

    for name, info in COMPILERS.items():
        path = find_compiler(info["command"])

        if path:
            compiler_paths[name] = {
                "path": path,
                "extra_args": info["extra_args"],
            }
            print(f"[OK] {name}: {path}")
        else:
            print(f"[MISSING] {name}: {info['command']}")

    print()

    if len(compiler_paths) < 2:
        print("ERROR: Both GCC and 32-bit Clang are required.")
        print("Check the compiler installation paths in runner.py.")
        sys.exit(1)

    test_files = sorted(TESTS_DIR.glob("*.c"))

    if not test_files:
        print(f"No .c test files found in: {TESTS_DIR}")
        sys.exit(1)

    RESULTS_DIR.mkdir(exist_ok=True)
    csv_file = RESULTS_DIR / "results.csv"

    rows = []

    print("-" * 72)
    print(f"{'Test':28} {'GCC':16} {'Clang':16} Result")
    print("-" * 72)

    for source_file in test_files:
        compiler_results, overall = test_one(
            source_file, compiler_paths
        )

        gcc_status = compiler_results["GCC"]["run_status"]
        if compiler_results["GCC"]["compile_status"] != "OK":
            gcc_status = "COMPILE ERROR"

        clang_status = compiler_results["Clang"]["run_status"]
        if compiler_results["Clang"]["compile_status"] != "OK":
            clang_status = "COMPILE ERROR"

        print(
            f"{source_file.name:28} "
            f"{gcc_status:16} "
            f"{clang_status:16} "
            f"{overall}"
        )

        rows.append({
            "test": source_file.name,
            "gcc_status": gcc_status,
            "clang_status": clang_status,
            "gcc_output": normalize_output(
                compiler_results["GCC"]["output"]
            ),
            "clang_output": normalize_output(
                compiler_results["Clang"]["output"]
            ),
            "result": overall,
            "gcc_error": compiler_results["GCC"]["error"],
            "clang_error": compiler_results["Clang"]["error"],
        })

    with csv_file.open("w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)

    print("-" * 72)

    counts = {}
    for row in rows:
        counts[row["result"]] = counts.get(row["result"], 0) + 1

    print(f"Total tests: {len(rows)}")
    for status, count in counts.items():
        print(f"{status:20}: {count}")

    print()
    print(f"Detailed results saved to: {csv_file}")
    print("=" * 72)


if __name__ == "__main__":
    main()