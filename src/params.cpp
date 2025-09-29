#include "bs/params.hpp"
#include <limits>

namespace bs {

    bool Params::validate(std::string* why) const {
        auto bad = [&](const char* msg) { if (why) *why = msg; return false; }
        if (!(S > 0.0))        return bad("S must be > 0");
        if (!(K > 0.0))        return bad("K must be > 0");
        if (!(sigma > 0.0))    return bad("sigma must be > 0");
        if (!(T >= 0.0))       return bad("T must be >= 0");
        if (!std::isfinite(r)) return bad ("r must be finite");
        return true;
    }

} // namespace bs