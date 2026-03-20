#include "bs/mc.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

namespace bs {
namespace {
    
// Simple online mean/variance accumulator (Welford).
// We use this so we do not need to store all path payoffs.
struct RunningStats {
    std::uint64_t n{0};
    double mean{0.0};
    double m2{0.0};

    void push(double x) noexcept {
        ++n;
        const double delta = x - mean;
        mean += delta / static_cast<double>(n);
        const double delta2 = x - mean;
        m2 += delta * delta2;
    }

    double variance() const noexcept {
        if (n < 2) return 0.0;
        return m2 / static_cast<double>(n - 1); // sample variance
    }
};

template <bool IsCall>
McResult mc_price_plain_impl(const Params& p, std::uint64_t paths, std::uint64_t seed) {
    if (paths == 0) {
        throw std::invalid_argument("mc_price_*_plain: paths must be > 0");
    }

    // Deterministic RNG for a given seed on this toolchain/build.
    std::mt19937_64 rng(seed);
    std::normal_distribution<double> normal(0.0, 1.0);

    const double drift = (p.r - 0.5 * p.sigma * p.sigma) * p.T;
    const double vol   = p.sigma * std::sqrt(p.T);
    const double disc  = std::exp(-p.r * p.T);

    RunningStats stats;

    for (std::uint64_t i = 0; i < paths; ++i) {
        const double z  = normal(rng);
        const double st = p.S * std::exp(drift + vol * z);

        const double payoff = IsCall
            ? std::max(st - p.K, 0.0)
            : std::max(p.K - st, 0.0);
        
        stats.push(payoff);
    }

    McResult out;
    out.price = disc * stats.mean;
    out.stderr = disc * std::sqrt(stats.variance() / static_cast<double>(paths));
    out.paths = paths;
    return out;
}

} // namespace

McResult mc_price_call_plain(const Params& p, std::uint64_t paths, std::uint64_t seed) {
    return mc_price_plain_impl<true>(p, paths, seed);
}

McResult mc_price_put_plain(const Params& p, std::uint64_t paths, std::uint64_t seed) {
    return mc_price_plain_impl<false>(p, paths, seed);
}

} // namespace bs