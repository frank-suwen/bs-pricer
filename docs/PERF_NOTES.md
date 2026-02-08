# PERF_NOTES

## Setup
- Build: `RelWithDebInfo`
    - `cmake -S .-B build-perf - DCMAKE_BUILD_TYPE=RelWithDebInfo`
    - `cmake --build build-perf -j` (use `-j1` if RAM is not enough)
- Workload: `./build-perf/bs_bench --iters 3_000_000` (pricing: call)

## perf stat
### Hardware counters (cycles/branches/cache misses)
- **Unavailable on this VM**: `perf stat` shows `<not supported>` for `cycles`, `instructions`, `branches`, `cache-misses`, etc.
- Interpretation: the guest VM/kernel does not expose HW PMU counters (common in cloud VMs).

### Software counters (available)
Command:
- `sudo perf stat -r 5 -e task-clock,context-switches,cpu-migrations,page-faults ./build-perf/bs_bench --iters 3000000`

Observed (example run):
- task-clock ~156 ms, ~0,96 CPUs utilized
- context-switches ~0(10-30)
- cpu-migrations ~0
- page-faults ~133 (startup/loader + libs; stable)

## perf record/report (top frames)
Command:
- `sudo perf record -F 999 -g --call-graph dwarf ./build-perf/bs_bench --iters 5000000`
- `sudo perf report --stdio`

Top stacks (from `perf report` exerpt):
- `bs::price_call(bs::Params const&)` dominates
- heavy time inside `bs::norm_cdf(double)` (inlined) --> libm `erfc*`
- additional time in `exp`
- noticeable bench overhead in `std::string` compare (`type == "call"` inside hot loop)

Conclusion
- pricing kernel is **compute-bound** and largely dominated by libm math (`erfc`/`exp`) and CDF plumbing.

## Flamegraph
- SVG:: `perf_out/bs_bench_flame.svg` (committed)