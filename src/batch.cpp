#include "bs/detail/batch.hpp"
#include "bs/pricing.hpp"
#include "bs/numeric_utils.hpp"
#include <vector>

namespace bs::detail {

double mean_price_call(std::span<const bs::Params> ps) {
    if (ps.empty()) return 0.0;

    std::vector<double> prices;
    prices.reserve(ps.size());
    for (const auto& p : ps) prices.push_back(bs::price_call(p));
    const double sum = bs::accumulate_sum(prices);   // vector shim → span core
    return sum / static_cast<double>(prices.size());
}

double mean_price_put(std::span<const bs::Params> ps) {
    if (ps.empty()) return 0.0;

    std::vector<double> prices;
    prices.reserve(ps.size());
    for (const auto& p : ps) prices.push_back(bs::price_put(p));
    const double sum = bs::accumulate_sum(prices);
    return sum / static_cast<double>(prices.size());
}

} // namespace bs::detail
