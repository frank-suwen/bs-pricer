#!/usr/bin/env bash
set -euo pipefail

BIN=${1:-./build-perf/bs_bench}
ITERS=${ITERS:-3000000}

# NOTE: This VM does not expose HW PMU counters; use software counters.
sudo perf stat -r 5 -e task-clock,context-switches,cpu-migrations,page-faults \
  "$BIN" --iters "$ITERS"
