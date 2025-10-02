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

# Numerics
see `docs/NUMERICS.md` for formulas, units, and tolerances. Summary:
- $C = S N(d_1) - K e^{-rT} N(d_2), \quad P = K e^{-rT} N(-d_2) - S N(-d_1)$
- Greeks: $\Delta, \Gamma, \text{vega}, \Theta, \rho$  
  (vega per $1.0 \, \sigma$, theta per year)

# Why C++20?
Feature richness (e.g., `constexpr`, library polish) with excellent toolchain maturity on Ubuntu 22.04 CI.

# Roadmap
* Stage-2: Monte Carlo engine + variance reduction
* Stage-3: SIMD/AVX + cache-aware layouts + microbenchmarks