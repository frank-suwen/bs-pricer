add_executable(bs_tests
  test_pricing.cpp
  test_greeks.cpp
  test_edgecases.cpp
)

target_link_libraries(bs_tests PRIVATE
  bs
  Catch2::Catch2WithMain
)

add_test(NAME bs_tests COMMAND bs_tests)