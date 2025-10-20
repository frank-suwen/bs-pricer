#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "bs/numeric_utils.hpp"

using namespace bs;
using Catch::Approx;

TEST_CASE("accumulate_sum spans & shims", "[numeric][span]") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0};
    std::array<double, 4> a{1.0, 2.0, 3.0, 4.0};
    double raw[4] = {1.0, 2.0, 3.0, 4.0};

    REQUIRE(accumulate_sum(v) == Approx(10.0));
    REQUIRE(accumulate_sum(a) == Approx(10.0));
    REQUIRE(accumulate_sum(raw, 4) == Approx(10.0));

    // Internal templated core (experiments only; prod API stays double)
    float fraw[3] = {1.f, 2.f, 3.f};
    auto s = accumulate_sum<float>(std::span<const float>(fraw, 3));
    REQUIRE(s == Approx(6.f));
}