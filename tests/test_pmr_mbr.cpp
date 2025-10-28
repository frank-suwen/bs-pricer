#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <array>
#include <cstddef>
#include <memory_resource>
#include <span>
#include <vector>

#include "helpers/pmr_logging_resource.hpp"    // the logger I just added
#include "bs/params.hpp"                       // just to keep includes consistent with repo style

TEST_CASE("pmr::monotonic_buffer_resource: in-buffer vs upstream allocations", "[pmr][mbr]") {
    // --- Upstream logger to record *only* fallbacks beyond the local buffer ---
    test_pmr::logging_resource logger{ std::pmr::get_default_resource() };

    // --- Small stack buffer for MBR ---
    // We want first two reserves (8 and 16 doubles) to fit locally,
    // but the 3rd reserve (32 doubles = 256 bytes) to exceed and go upstream.
    constexpr std::size_t bytes_8   = 8  * sizeof(double); //  64
    constexpr std::size_t bytes_16  = 16 * sizeof(double); // 128
    constexpr std::size_t bytes_32  = 32 * sizeof(double); // 256
    static_assert(bytes_8 == 64 && bytes_16 == 128 && bytes_32 == 256);

    // Choose a buffer just big enough for 64 + 128 = 192 bytes, leaving a little slack.
    std::array<std::byte, 200> local_buf{}; // fewer than 256 so the 32-capacity spills upstream

    std::pmr::monotonic_buffer_resource mbr{
        local_buf.data(),
        local_buf.size(),
        &logger            // <- upstream: our logging resource
    };

    // --- pmr vector bound to our MBR ---
    std::pmr::polymorphic_allocator<double> alloc{&mbr};
    std::pmr::vector<double> v{alloc};

    // Start clean
    logger.clear_logs();

    // 1) First allocation: preserve 8 double (64 bytes) -> should be served by MBR (no upstream)
    v.reserve(8);
    REQUIRE(logger.allocs().empty());

    // 2) Second allocaton: reserve 16 doubles (128 bytes) -> still fits in remaining local buffer
    v.reserve(16);
    REQUIRE(logger.allocs().empty());

    // 3) Third allocation: reserve 32 doubles (256 bytes) -> exceeds local buffer, must go upstream
    v.reserve(32);
    REQUIRE(!logger.allocs().empty());
    // Expect the first upstream allocation to be for 256 bytes with alignment of double
    const auto& e = logger.allocs().front();
    REQUIRE(e.size == bytes_32);
    REQUIRE(e.alignment == alignof(double));

    // Optional: sanity that pushes now use the already-allocated capacity (no new upstream alloc)
    const auto upstream_before = logger.allocs().size();
    for (int i = 0; i < 32; ++i) v.push_back(static_cast<double>(i));
    REQUIRE(logger.allocs().size() == upstream_before);
}