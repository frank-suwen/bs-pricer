#pragma once
#include <cmath>

namespace bs {

// Standard normal PDF
inline double norm_pdf(double x) {
  static constexpr double INV_SQRT_2PI = 0.39894228040143267794; // 1/sqrt(2*pi)
  return INV_SQRT_2PI * std::exp(-0.5 * x * x);
}

// Standard normal CDF using erfc (good numeric behavior for tails)
inline double norm_cdf(double x) {
  static constexpr double INV_SQRT_2 = 0.70710678118654752440; // 1/sqrt(2)
  return 0.5 * std::erfc(-x * INV_SQRT_2);
}

} // namespace bs