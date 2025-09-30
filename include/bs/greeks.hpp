#pragma once
#include "params.hpp"

namespace bs {
struct Greeks {
  double delta{};
  double gamma{};
  double vega{};   // per 1.0 change in sigma (i.e., 100 vol points)
  double theta{};  // per year
  double rho{};    // per 1.0 change in r (per year)
};

Greeks greeks_call(const Params& p);
Greeks greeks_put (const Params& p);
} // namespace bs
