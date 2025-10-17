#include <catch2/catch_test_macros.hpp>
#include "bs/concepts.hpp"

using bs::Arithmetic;

// Compile-time checks (fire during compilation)
static_assert(Arithmetic<int>);
static_assert(Arithmetic<float>);
static_assert(Arithmetic<double>);
static_assert(!Arithmetic<bool>);

// Runtime "placeholder" so Catch registers a test case and ctest shows it.
TEST_CASE("Arithmetic concept sanity", "[concepts]") {
    // If it compiled, the static_asserts alrady passed.
    // Add a trivial check so Catch reports one passing test.
    REQUIRE(true);
}