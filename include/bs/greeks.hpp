#pragma once
#include "params.hpp"
namespace bs {
    struct Greeks { double delta, gamma, vega, theta, rho; };
    Greeks greeks_call(const Params&);
    Greeks greeks_put (const Params&);
}