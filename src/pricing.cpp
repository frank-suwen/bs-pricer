// Implements d1/d2 and call/put pricing using norm_cdf, std::exp, std::long, etc.
// Keep helpers inline/static in this TU if private.
#include "bs/pricing.hpp"
#include "bs/math.hpp"
#include <algorithm>
#include <cmath>

namespace bs {

static inline double d1(const Params &p) {
  const double vol_sqrtT = p.sigma * std::sqrt(p.T);
  return (std::log(p.S / p.K) + (p.r + 0.5 * p.sigma * p.sigma) * p.T) / vol_sqrtT;
}

static inline double d2_from_d1(double d1_val, const Params &p) {
  return d1_val - p.sigma * std::sqrt(p.T);
}

double price_call(const Params &p) {
  if (p.T == 0.0)
    return std::max(0.0, p.S - p.K);
  const double D1 = d1(p);
  const double D2 = d2_from_d1(D1, p);
  return p.S * norm_cdf(D1) - p.K * std::exp(-p.r * p.T) * norm_cdf(D2);
}

double price_put(const Params &p) {
  if (p.T == 0.0)
    return std::max(0.0, p.K - p.S);
  const double D1 = d1(p);
  const double D2 = d2_from_d1(D1, p);
  return p.K * std::exp(-p.r * p.T) * norm_cdf(-D2) - p.S * norm_cdf(-D1);
}

} // namespace bs