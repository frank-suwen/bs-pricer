## DESIGN: When to expose `span`, when to template, when to prefer PMR

### TL;DR (checklist)
- **Public API:** keep `double`-based, stable
- **Internals:** accept `std::span<const T>` to avoid copies; provide **double shims**.
- **Templates:** only when type-polymorphism buys you real wins (perf, reuse); constrain with `Arithmetic`.
- **PMR (arena):** use `std::pmr::monotonic_buffer_resource` for **batch allocations**; pre-size to avoid upstream.

### Goals & constraints
- Keep the **external surface stable** for users/tests/benchmarks.
- Enable **zero-copy** data flow internally (no vector ownership required).
- Prepare for Stage-2 MC + Stage-3 SMID without API churn.
- Keep tests **portable** (don't assert exact allocator byte counts).

### `std::span` policy
- **Expose `span` on internals** (helpers & numerics) to accept views of contiguous data:
    - Example: `bs::accumulate_sum(std::span<const double>)` (core) with `std::vector<double>` shim.
    - Domain batch helpers (`bs::detail::mean_price_*`) take `std::span<const bs::Params>`.
- **Why:** zero-copy, no ownership, ergonomic interop (arrays, vectors, `pmr::vector`).
- **Public API stays concrete** (`double` params/returns). Add shims instead of changing signatures.

## Templates (with concepts)
- **Default:** write concrete `double` functions for the public surface (predictable ABI & errors).
- **Template** only where you benefit from type-polymorphism:
    - Numeric kernels that naturally generalize: e.g., `accumulate_sum<T: Arithmetic>`.
    - Unit-test scaffolding and micro-bench experiments.
- **Constrain** with tiny concepts (here: `Arithmetic`) to keep diagnostics clean and prevent accidental misuse.

## PMR / MBR policy
- Use **PMR** to route container allocations at runtime; plug **MBR** when you want arena semantics.
- Prefer **MBR per batch** (e.g., MC simulation run) when:
    - You can **estimate working set** once (largest `reserve()` first),
    - You want **O(1) teardown** (`arena.release()`),
    - You want to **avoid heap traffic** in the hot path.
- Keep a **logger upstream** in tests to assert "no upstream allocs" when sized correctly.
- Avoid MBR when you need **fine-grained frees** mid-batch or lifetimes overlap batches.

## Examples in this repo
- **Span core + shims:** `accumulate_sum(span)`with `vector` overload; used in bench checksum.
- **Domain batch helper:** `bs::detail::mean_price_call/put(std::span<const Params>)`.
- **PMR demo:** `pmr::vector<double>` backed by `monotonic_buffer_resource` with upstream logger; portable assertions (size >= need, alignment >= `alignof(double)`).

## Defaults we follow
- Public API: **concrete types** (doubles), stable.
- Internals: **span-based** kernels, **templated only where it pays, PMR** when batchy.