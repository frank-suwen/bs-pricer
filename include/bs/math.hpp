#pragma once
#include <cmath>
namespace bs {
    inline double norm_pdf(double x) { static constexpr double inv_sqrt_2pi = 0.3989422804014327; return inv_sqrt_2pi*std::exp(-0.5*x*x); }
    inline double norm_cdf(double x) { static constexpr double inv_sqrt_2 = 0.7071067811865476; return 0.5 * std::erfc(-x*inv_sqrt_2); }
}