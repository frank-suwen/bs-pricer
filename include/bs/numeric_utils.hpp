#pragma once
#include <span>
#include <array>
#include <vector>
#include <cstddef>
#include "bs/concepts.hpp"

namespace bs {

// Core: templated for internal experiments (float/double/long double)
// Keep public API double-based; this is internal plumbing.
template <Arithmetic T>
inline T accumulate_sum(std::span<const T> xs) noexcept {
    T s{};                      // value-init to 0
    for (T v : xs) s += v;      // tight loop
    return s;
}

// --- Double-only shims (stable production surface) ---
inline double accumulate_sum(const std::vector<double>& xs) noexcept {
    return accumulate_sum<double>(std::span<const double>(xs.data(), xs.size()));
}

template <std::size_t N>
inline double accumulate_sum(const std::array<double, N>& xs) noexcept {
    return accumulate_sum<double>(std::span<const double>(xs.data(), xs.size()));
}

inline double accumulate_sum(const double* ptr, std::size_t n) noexcept {
    return accumulate_sum<double>(std::span<const double>(ptr, n));
}

} // namespace