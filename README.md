# bs-pricer

A small, production-ish **Black-Scholes** pricer in modern **C++20** with a clean API, unit tests, and CI. 

# Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBS_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

# CLI
```bash
./build/bs_cli --type call --S 100 --K 100 --r 0.05 --sigma 0.2 --T 1
```

# Library API
* `price_call(const Params&)`, `price_put(const Params&)`
* `greeks_call(const Params&)`, `greeks_put(const Params&)`
* `Params{S,K,r,sigma,T}` with `validate()`
* `norm_pdf`, `norm_cdf` helpers

# Internals
* Core numerics accept `std::span<const T>` (zero copy) with a tiny `Arithmetic` concept for safe templating.
* Public API remains `double` for stability; span-core is used in enchmarks and internal batch helpers (`bs::detail::mean_price_call/put`).
* This prepares the Monte Carlo engine (Stage-2) and SMID (Stage-3) without API churn. 
* **PMR demo:** A `pmr::monotonic_buffer_resource` micro-test allocates a `pmr::vector<double>` from a fixed stack buffer and asserts the spill-over to an upstream logging resource. This documents allocation sites portably (size/alignment checks).


# Numerics
see `docs/NUMERICS.md` for formulas, units, and tolerances. Summary:
- $C = S N(d_1) - K e^{-rT} N(d_2), \quad P = K e^{-rT} N(-d_2) - S N(-d_1)$
- Greeks: $\Delta, \Gamma, \text{vega}, \Theta, \rho$  
  (vega per $1.0 \, \sigma$, theta per year)

# Why C++20?
Feature richness (e.g., `constexpr`, library polish) with excellent toolchain maturity on Ubuntu 22.04 CI.

# Benchmark
Built in **Release** on an Azure Standard B1s (1 vCPU, 1 GiB), gcc (Ubuntu 11.x), CMake Ninja.

`./build/bs_bench --type call --iters 300000`

$Time: 10.34 ms \space | \space ns/eval: 34.47 \space | \space evals/s: ~29.0M$

`./build/bs_bench --type put --iters 300000`

$Time: 10.64 ms \space | \space ns/eval: 35.45 \space | \space evals/s: ~28.2M$

Numbers vary with hardware/load; CI **builds** the bench but does not **run** it.



# Roadmap
* Stage-2: Monte Carlo engine + variance reduction
* Stage-3: SIMD/AVX + cache-aware layouts + microbenchmarks