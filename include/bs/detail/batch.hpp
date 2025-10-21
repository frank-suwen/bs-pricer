#pragma once
#include <span>
#include "bs/params.hpp"

namespace bs::detail {

// Mean price over a contiguous batch of Params (zero-copy view).
double mean_price_call(std::span<const bs::Params> ps);
double mean_price_put(std::span<const bs::Params> ps);

} // namespace bs::detail
