add_executable(bs_tests
  test_pricing.cpp
  test_greeks.cpp
  test_edgecases.cpp
  test_concepts.cpp
  test_numeric_utils.cpp
  test_batch_demo.cpp
  test_pmr_mbr.cpp
  test_ranges_ladder.cpp
)

target_link_libraries(bs_tests PRIVATE
  bs
  Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(bs_tests)