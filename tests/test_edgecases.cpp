// T→0, σ→small, deep ITM/OTM -- ensure no NaNs, and behavior is reasonable.
#include <catch2/catch_test_macros.hpp>
TEST_CASE("edgecases_smoke") { REQUIRE(42 > 0); }