#!/usr/bin/env bash
set -euo pipefail

OUTDIR=${OUTDIR:-perf_out}
FLAME=${FLAME:-$HOME/FlameGraph}

mkdir -p "$OUTDIR"

sudo perf script > "$OUTDIR/perf.script"
"$FLAME/stackcollapse-perf.pl" "$OUTDIR/perf.script" > "$OUTDIR/out.folded"
"$FLAME/flamegraph.pl" "$OUTDIR/out.folded" > "$OUTDIR/bs_bench_flame.svg"

echo "Wrote $OUTDIR/bs_bench_flame.svg"
