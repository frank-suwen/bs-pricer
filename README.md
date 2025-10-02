# bs-pricer

A small, production-ish **Black-Scholes** pricer in modern **C++20** with a clean API, unit tests, and CI. 

# Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBS_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure