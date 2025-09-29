#pragma once
#include "params.hpp"

namespace bs {
    double price_call(const Params& p);
    double price_put(const Params& p);
} // namespace bs