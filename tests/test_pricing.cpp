// Golden cases (e.g., S=100, K=100, r=0.05, sigma=0.2, T=1) with tight tolerances
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "bs/pricing.hpp"
#include "bs/params.hpp"

TEST_CASE("Black-Scholes call price: S=100,K=100,r=5%,sigma-20%,T=1") {
    bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    REQUIRE(p.validate());         // sanity
    // Known result ≈ 10.4506 (annualized vol/rate, no dividends, tolerance 1e-4 is fine for our math impl)
    // NOTE: We'll implement price_call next; for now this will fail, proving the test is live.
    REQUIRE(bs::price_call(p) == Catch::Approx(10.4506).margin(1e-4));
}