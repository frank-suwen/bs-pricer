// Warmup loop; time N evaluations; report mean and p50. (Stage-1 is about correctness + package, not perf heroics yet.)
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "bs/bs.hpp"

using my_clock_t = std::chrono::steady_clock;

static double parse_or(const char* s, double d) { try { return s ? std::stod(s) : d; } catch (...) { return d; } }
static long parse_or_long(const char* s, long d) { try { return s ? std::stol(s) : d; } catch (...) { return d; } }

int main(int argc, char** argv) {
    // Defaults suitable for a small VM; tweak with flags
    std::string type = "call";
    long iters = 500000;                // keep modest for 1 vCPU / 1GiB
    bs::Params p{100.0, 100.0, 0.05, 0.20, 1.0};

    // Very simple flag parsing: --type, --iters, --S, --K, --r, --sigma, --T
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        auto next = [&](double& ref) { if (i + 1 < argc) ref = parse_or(argv[++i], ref); };
        if (a == "--type" && i + 1 < argc) type = argv[++i];
        else if (a == "--iters" && i + 1 < argc) iters = parse_or_long(argv[++i], iters);
        else if (a == "--S") next(p.S);
        else if (a == "--K") next(p.K);
        else if (a == "--r") next(p.r);
        else if (a == "--sigma") next(p.sigma);
        else if (a == "--T") next (p.T);
    }

    std::string why;
    if (!p.validate(&why)) { std::cerr << "Invalid params: " << why << "\n"; return 2; }

    // Warm-up (avoid cold-start skew)
    volatile double sink = 0.0;
    for (int w = 0; w < 3; ++w) {
        for (long i = 0; i < iters / 10; i++) {
            sink = sink + (type == "call" ? bs::price_call(p) : bs::price_put(p));
        }
    }

    // Timed section
    auto t0 = my_clock_t::now();
    for (long i = 0; i < iters; i++) {
        sink = sink + (type == "call" ? bs::price_call(p) : bs::price_put(p));
    }
    auto t1 = my_clock_t::now();

    const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    const double ns_per_eval = static_cast<double>(ns) / static_cast<double>(iters);
    const double evals_per_sec = 1e9 / ns_per_eval;

    std::cout << "Type=" << type << " iters=" << iters << "\n";
    std::cout << "Time: " << ns / 1e6 << " ms\n";
    std::cout << "ns/eval: " << ns_per_eval << "\n";
    std::cout << "evals/s: " << evals_per_sec << "\n";
    // prevent dead-code elimination:
    std::cout << "sink=" << sink << "\n";
    return 0;
}