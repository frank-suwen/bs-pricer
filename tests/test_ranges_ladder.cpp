#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <ranges>
#include "bs/params.hpp"
#include "bs/pricing.hpp"

// --- C++20 helper: collect a range into std::vector<T>
namespace {
template <std::ranges::input_range R>
auto to_vector(R&& r) {
    using T = std::ranges::range_value_t<R>;
    std::vector<T> out;
    if constexpr (std::ranges::sized_range<R>)
        out.reserve(std::ranges::size(r));
    for (auto&& x : r) out.emplace_back(x);
    return out;
}
} // namespace

TEST_CASE("ranges: strike ladder -> call prices", "[ranges]") {
    using namespace std;

    // Base params
    bs::Params base{100.0, /*K=*/100.0, 0.05, 0.20, 1.0};

    // K ladder: 80, 85, 90, ..., 120 (step 5)
    auto ladder_view = 
        views::iota(0, 9)              // 0..8
      | views::transform([&](int i) {
            bs::Params p = base;
            p.K = 80 + 5 * i;
            return p;
        });
    
    auto ladder = to_vector(ladder_view);

    REQUIRE(ladder.front().K == 80.0);
    REQUIRE(ladder.back().K == 120.0);

    // Map to call prices
    auto prices_view =
        ladder 
    | views::transform([](const bs::Params& p) { return bs::price_call(p); });

    auto prices = to_vector(prices_view);

    REQUIRE(prices.size() == ladder.size());
    // Spot sanity: higher strike -> lower call price (monotone nonincreasing in K)
    for (size_t i = 1; i < prices.size(); ++i) {
        REQUIRE(prices[i] <= prices[i - 1]);
    }
}