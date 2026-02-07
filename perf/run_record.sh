#!/usr/bin/env bash
set -euo pipefail

BIN=${1:-./build-perf/bs_bench}
ITERS=${ITERS:-5000000}

sudo perf record -F 999 -g --call-graph dwarf \
  "$BIN" --iters "$ITERS"
