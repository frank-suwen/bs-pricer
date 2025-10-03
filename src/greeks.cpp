// Implements close-form Greeks (units per year; note vega scaling).
#include "bs/greeks.hpp"
#include "algorithm"
#include "bs/math.hpp"
#include "bs/pricing.hpp" // for payoff at T=0 if you want consistent handling
#include <cmath>

namespace bs {

static inline double d1(const Params &p) {
  const double vol_sqrtT = p.sigma * std::sqrt(p.T);
  return (std::log(p.S / p.K) + (p.r + 0.5 * p.sigma * p.sigma) * p.T) / vol_sqrtT;
}

static inline double d2_from_d1(double d1_val, const Params &p) {
  return d1_val - p.sigma * std::sqrt(p.T);
}

Greeks greeks_call(const Params &p) {
  Greeks g{};
  if (p.T == 0.0) {
    // At expiry, Greeks are ill-behaved; keep a simple, defensible convention
    g.delta = (p.S > p.K) ? 1.0 : (p.S < p.K ? 0.0 : 0.5);
    g.gamma = 0.0;
    g.vega = 0.0;
    g.theta = 0.0;
    g.rho = (p.S > p.K) ? p.K * p.T * std::exp(-p.r * p.T) : 0.0; // not meaningful; keep 0
    return g;
  }

  const double D1 = d1(p);
  const double D2 = d2_from_d1(D1, p);
  const double n1 = norm_pdf(D1);

  g.delta = std::erfc(-(D1) / std::sqrt(2.0)) * 0.5;
  g.gamma = n1 / (p.S * p.sigma * std::sqrt(p.T));
  g.vega = p.S * n1 * std::sqrt(p.T);
  g.theta = -(p.S * n1 * p.sigma) / (2.0 * std::sqrt(p.T)) -
            p.r * p.K * std::exp(-p.r * p.T) * (0.5 * std::erfc(-D2 / std::sqrt(2.0)));
  g.rho = p.K * p.T * std::exp(-p.r * p.T) * (0.5 * std::erfc(-D2 / std::sqrt(2.0)));
  return g;
}

Greeks greeks_put(const Params &p) {
  Greeks g{};
  if (p.T == 0.0) {
    g.delta = (p.S < p.K) ? -1.0 : (p.S > p.K ? 0.0 : -0.5);
    g.gamma = 0.0;
    g.vega = 0.0;
    g.theta = 0.0;
    g.rho = 0.0;
    return g;
  }

  const double D1 = d1(p);
  const double D2 = d2_from_d1(D1, p);
  const double n1 = norm_pdf(D1);

  const double Nd1 = 0.5 * std::erfc(-D1 / std::sqrt(2.0));
  const double Nd2 = 0.5 * std::erfc(-D2 / std::sqrt(2.0));

  g.delta = Nd1 - 1.0;
  g.gamma = n1 / (p.S * p.sigma * std::sqrt(p.T));
  g.vega = p.S * n1 * std::sqrt(p.T);
  g.theta = -(p.S * n1 * p.sigma) / (2.0 * std::sqrt(p.T)) +
            p.r * p.K * std::exp(-p.r * p.T) * (1.0 - Nd2);
  g.rho = -p.K * p.T * std::exp(-p.r * p.T) * (1.0 - Nd2);
  return g;
}

} // namespace bs