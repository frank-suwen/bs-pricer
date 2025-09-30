#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "bs/greeks.hpp"
#include "bs/pricing.hpp"
#include "bs/params.hpp"

using Catch::Approx;

TEST_CASE("Call Greeks (S=100,K=100,r=5%,sigma=20%,T=1)") {
  bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
  REQUIRE(p.validate());

  const auto g = bs::greeks_call(p);
  CHECK(g.delta == Approx(0.6368306512).margin(1e-6));
  CHECK(g.gamma == Approx(0.01876201735).margin(1e-6));
  CHECK(g.vega  == Approx(37.52403469).margin(1e-4));
  CHECK(g.theta == Approx(-6.41402755).margin(1e-4));
  CHECK(g.rho   == Approx(53.23248155).margin(1e-4));
}

TEST_CASE("Put Greeks (S=100,K=100,r=5%,sigma=20%,T=1)") {
  bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
  REQUIRE(p.validate());

  const auto g = bs::greeks_put(p);
  CHECK(g.delta == Approx(-0.3631693488).margin(1e-6));
  CHECK(g.gamma == Approx(0.01876201735).margin(1e-6));      // same as call
  CHECK(g.vega  == Approx(37.52403469).margin(1e-4));        // same as call
  CHECK(g.theta == Approx(-1.65788042).margin(1e-4));
  CHECK(g.rho   == Approx(-41.89046090).margin(1e-4));
}

TEST_CASE("Finite-difference sanity (delta & gamma)") {
  bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
  const double eps = 1e-4; // small relative bump
  const double S0  = p.S;

  const auto g = bs::greeks_call(p);

  p.S = S0 * (1.0 + eps);
  const double up = bs::price_call(p);
  p.S = S0 * (1.0 - eps);
  const double dn = bs::price_call(p);
  p.S = S0;

  const double fd_delta = (up - dn) / (2.0 * S0 * eps);
  const double fd_gamma = (up - 2.0*bs::price_call(p) + dn) / (S0*S0*eps*eps);

  CHECK(fd_delta == Approx(g.delta).margin(5e-4));
  CHECK(fd_gamma == Approx(g.gamma).margin(5e-3)); // looser: FD is noisy
}
