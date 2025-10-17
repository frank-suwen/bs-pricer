#include "bs/concepts.hpp"
using bs::Arithmetic;

static_assert(Arithmetic<int>);
static_assert(Arithmetic<float>);
static_assert(Arithmetic<double>);
static_assert(!Arithmetic<bool>);

int main() { return 0; } // compile-time checks are the point here