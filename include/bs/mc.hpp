#pragma once
#include <cstdint>

#include "bs/params.hpp"

namespace bs {

struct McResult {
    double price{};
    double stderr{};
    std::uint64_t paths{};
};

McResult mc_price_call_plain(const Params& p, std::uint64_t paths, std::uint64_t seed);
McResult mc_price_put_plain(const Params& p, std::uint64_t paths, std::uint64_t seed);

McResult mc_price_call_antithetic(const Params& p, std::uint64_t paths, std::uint64_t seed);
McResult mc_price_put_antithetic(const Params& p, std::uint64_t paths, std::uint64_t seed);

} // namespace bs