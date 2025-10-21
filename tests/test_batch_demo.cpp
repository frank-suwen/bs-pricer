#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <span>                         // <-- needed for std::span in the test
#include "bs/params.hpp"
#include "bs/pricing.hpp"
#include "bs/detail/batch.hpp"

using Catch::Approx;

TEST_CASE("mean_price_call uses span plumbing over Params", "[batch][span]") {
    bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    bs::Params arr[3] = {p, p, p};

    const double single = bs::price_call(p);
    const double mean3 =
        bs::detail::mean_price_call(std::span<const bs::Params>(arr, 3));

    REQUIRE(mean3 == Approx(single).margin(1e-12));
}

TEST_CASE("mean_price_put uses span plumbing over Params", "[batch][span]") {
    bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};
    std::vector<bs::Params> v(5, p);

    const double single = bs::price_put(p);
    const double mean5 =
        bs::detail::mean_price_put(std::span<const bs::Params>(v.data(), v.size()));

    REQUIRE(mean5 == Approx(single).margin(1e-12));
}

TEST_CASE("mean_price_* handles empty spans", "[batch][span]") {
    std::span<const bs::Params> empty{};
    REQUIRE(bs::detail::mean_price_call(empty) == Approx(0.0));
    REQUIRE(bs::detail::mean_price_put(empty)  == Approx(0.0));
}
