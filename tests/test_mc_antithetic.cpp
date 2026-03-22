#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <stdexcept>

#include "bs/mc.hpp"
#include "bs/params.hpp"
#include "bs/pricing.hpp"

using Catch::Approx;

TEST_CASE("mc antithetic: deterministic seed gives deterministic result", "[mc][antithetic]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    const std::uint64_t paths = 100000;
    const std::uint64_t seed  = 42;

    const auto r1 = bs::mc_price_call_antithetic(p, paths, seed);
    const auto r2 = bs::mc_price_call_antithetic(p, paths, seed);

    REQUIRE(r1.price  == r2.price);
    REQUIRE(r1.stderr == r2.stderr);
    REQUIRE(r1.paths  == r2.paths);
}

TEST_CASE("mc antithetic: rejects odd path counts", "[mc][antithetic]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};

    REQUIRE_THROWS_AS(
        bs::mc_price_call_antithetic(p, 99999, 1),
        std::invalid_argument
    );
}

TEST_CASE("mc antithetic: call is reasonably close to analytic BS", "[mc][antithetic]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    const std::uint64_t paths = 200000;
    const std::uint64_t seed  = 7;

    const auto mc = bs::mc_price_call_antithetic(p, paths, seed);
    const double analytic = bs::price_call(p);

    REQUIRE(mc.price == Approx(analytic).margin(0.10));
    REQUIRE(mc.stderr > 0.0);
}

TEST_CASE("mc antithetic: put is reasonably close to analytic BS", "[mc][antithetic]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    const std::uint64_t paths = 200000;
    const std::uint64_t seed  = 7;

    const auto mc = bs::mc_price_put_antithetic(p, paths, seed);
    const double analytic = bs::price_put(p);

    REQUIRE(mc.price == Approx(analytic).margin(0.10));
    REQUIRE(mc.stderr > 0.0);
}

TEST_CASE("mc antithetic: stderr is lower than plain for some total paths", "[mc][antithetic]") {
    const bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    const std::uint64_t paths = 200000;
    const std::uint64_t seed  = 12345;

    const auto plain = bs::mc_price_call_plain(p, paths, seed);
    const auto anti  = bs::mc_price_call_antithetic(p, paths, seed);

    REQUIRE(anti.stderr < plain.stderr);
}