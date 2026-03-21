#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <cstdint>

#include "bs/mc.hpp"
#include "bs/params.hpp"
#include "bs/pricing.hpp"

using Catch::Approx;

TEST_CASE("mc plain: deterministic seed gives deterministic result", "[mc][plain]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    const std::uint64_t paths = 100000;
    const std::uint64_t seed  = 42;

    const auto r1 = bs::mc_price_call_plain(p, paths, seed);
    const auto r2 = bs::mc_price_call_plain(p, paths, seed);

    REQUIRE(r1.price  == r2.price);
    REQUIRE(r1.stderr == r2.stderr);
    REQUIRE(r1.paths  == r2.paths);
}

TEST_CASE("mc plain: call result has sane fields and bounds", "[mc][plain]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    const std::uint64_t paths = 100000;
    const std::uint64_t seed  = 123;

    const auto r = bs::mc_price_call_plain(p, paths, seed);

    REQUIRE(r.paths == paths);
    REQUIRE(r.stderr > 0.0);

    // No-arbitrage-ish sanity bounds for a European call:
    // max(S - K e^{-rT}, 0) <= C <= S
    const double lower = std::max(p.S - p.K * std::exp(-p.r * p.T), 0.0);
    const double upper = p.S;

    REQUIRE(r.price >= lower);
    REQUIRE(r.price <= upper);
}

TEST_CASE("mc plain: put result has sane fields and bounds", "[mc][plain]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    const std::uint64_t paths = 100000;
    const std::uint64_t seed  = 123;

    const auto r = bs::mc_price_put_plain(p, paths, seed);

    REQUIRE(r.paths == paths);
    REQUIRE(r.stderr > 0.0);

    // No-arbitrage-ish sanity bounds for a European put:
    // max(K e^{-rT} - S, 0) <= P <= K e^{-rT}
    const double discK = p.K * std::exp(-p.r * p.T);
    const double lower = std::max(discK - p.S, 0.0);
    const double upper = discK;

    REQUIRE(r.price >= lower);
    REQUIRE(r.price <= upper);
}

TEST_CASE("mc plain: call is reasonably close to analytics BS", "[mc][plain]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    const std::uint64_t paths = 200000;
    const std::uint64_t seed  = 7;

    const auto mc = bs::mc_price_call_plain(p, paths, seed);
    const double analytic = bs::price_call(p);

    // We do not expect exact equality.
    // Use a practical tolerance for this path count.
    REQUIRE(mc.price == Approx(analytic).margin(1.0));
}

TEST_CASE("mc plain: put is reasonably close to analytic BS", "[mc][plain]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    const std::uint64_t paths = 200000;
    const std::uint64_t seed  = 7;

    const auto mc = bs::mc_price_put_plain(p, paths, seed);
    const double analytic = bs::price_put(p);

    REQUIRE(mc.price == Approx(analytic).margin(0.10));
}