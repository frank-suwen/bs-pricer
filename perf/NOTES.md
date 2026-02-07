# Perf notes

- Build: `RelWithDebInfo` (`cmake -S . -B build-perf -DCMAKE_BUILD_TYPE=RelWithDebInfo`)
- Workload: `./build-perf/bs_bench --iters 3_000_000` (pricing, call)
- perf stat: VM does not expose HW PMU counters; used software counters: `task-clock, context-switches, cpu-migrations, page-faults`
- perf report (top stacks): majority in `bs::price_call`, with heavy time in `bs::norm_cdf` (inlined) and libm `erfc*`, plus `exp`
- Bench overhead: noticeable time in `std::string` compare (`type == "call"` inside hot loop)
- Next actions: hoist branch out of loop (preselect function pointer); later consider faster CDF approximation if needed (while keeping accuracy constraints)