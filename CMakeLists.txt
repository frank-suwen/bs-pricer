cmake_minimum_required(VERSION 3.20)
project(bs_pricer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXXSTANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BS_BUILD_TESTS "Build tests" ON)

# library (empty sources for now are OK)
add_library(bs STATIC
    src/pricing.cpp
    src/greeks.cpp
    src/math.cpp
    src/params.cpp
)

target_include_directories(bs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(bs PUBLIC cxx_std_20)
target_compile_options(bs PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,APPleClang>:-Wall -Wextra -Wpedantic>
)

# CLI
add_executable(bs_cli apps/bs_cli.cpp)
target_link_libraries(bs_cli PRIVATE bs)

# Dependencies for tests
include(cmake/deps.cmake)

if(BS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()