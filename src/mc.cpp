#include "bs/mc.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

namespace bs {
namespace {
    
// Simple online mean/variance accumulator (Welford).
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
inline double payoff_from_st(double st, double k) noexcept {
    if constexpr (IsCall) {
        return std::max(st - k, 0.0);
    } else {
        return std::max(k - st, 0.0);
    }
}

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
        const double payoff = payoff_from_st<IsCall>(st, p.K);
        stats.push(payoff);
    }

    McResult out;
    out.price = disc * stats.mean;
    out.stderr = disc * std::sqrt(stats.variance() / static_cast<double>(paths));
    out.paths = paths;
    return out;
}

template <bool IsCall>
McResult mc_price_antithetic_impl(const Params& p, std::uint64_t paths, std::uint64_t seed) {
    if (paths == 0) {
        throw std::invalid_argument("mc_price_*_antithetic: paths must be > 0");
    }
    if ((paths % 2) != 0) {
        throw std::invalid_argument("mc_price_*_antithetic: paths must be even");
    }

    std::mt19937_64 rng(seed);
    std::normal_distribution<double> normal(0.0, 1.0);

    const double drift = (p.r - 0.5 * p.sigma * p.sigma) * p.T;
    const double vol   = p.sigma * std::sqrt(p.T);
    const double disc  = std::exp(-p.r * p.T);

    const std::uint64_t pairs = paths / 2;
    
    RunningStats stats;

    for (std::uint64_t i = 0; i < pairs; ++i) {
        const double z = normal(rng);

        const double st1 = p.S * std::exp(drift + vol * z);
        const double st2 = p.S * std::exp(drift - vol * z); // same as using -z

        const double payoff1 = payoff_from_st<IsCall>(st1, p.K);
        const double payoff2 = payoff_from_st<IsCall>(st2, p.K);

        // One antithetic pair becomes one averaged observation.
        const double paired_payoff = 0.5 * (payoff1 + payoff2);
        stats.push(paired_payoff);
    }

    McResult out;
    out.price = disc * stats.mean;
    out.stderr = disc * std::sqrt(stats.variance() / static_cast<double>(pairs));
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

McResult mc_price_call_antithetic(const Params& p, std::uint64_t paths, std::uint64_t seed) {
    return mc_price_antithetic_impl<true>(p, paths, seed);
}

McResult mc_price_put_antithetic(const Params& p, std::uint64_t paths, std::uint64_t seed) {
    return mc_price_antithetic_impl<false>(p, paths, seed);
}

} // namespace bs