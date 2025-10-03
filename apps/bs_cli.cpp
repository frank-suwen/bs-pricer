// Manual argv parsing (Stage-1, minimal):
// read --type, --S, --K, --r, --sigma, --T.
// On success, print price + Greeks in a neat block;
// On bad input, return non-zero with a short usage string.
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "bs/bs.hpp" // pulls in params, pricing, greeks

namespace {

void print_usage(const char *prog) {
  std::cerr << "Usage:\n  " << prog << " -- type {call|put}"
            << " -- S <spot> --K <strike> --r <rate> --sgima <vol> --T <years>\n"
            << "Example:\n  " << prog
            << " --type call --S 100 --K 100 --r 0.05 --sigma 0.2 --T 1\n";
}

bool parse_double(const std::vector<std::string> &args, size_t &i, double &out) {
  if (i + 1 >= args.size())
    return false;
  try {
    out = std::stod(args[++i]);
  } catch (...) {
    return false;
  }
  return true;
}

} // namespace

int main(int argc, char **argv) {
  std::vector<std::string> args(argv + 1, argv + argc);

  std::string type;
  bs::Params p{};
  for (size_t i = 0; i < args.size(); ++i) {
    const std::string &a = args[i];
    if (a == "--type") {
      if (i + 1 >= args.size()) {
        print_usage(argv[0]);
        return 2;
      }
      type = args[++i];
    } else if (a == "--S") {
      if (!parse_double(args, i, p.S)) {
        print_usage(argv[0]);
        return 2;
      }
    } else if (a == "--K") {
      if (!parse_double(args, i, p.K)) {
        print_usage(argv[0]);
        return 2;
      }
    } else if (a == "--r") {
      if (!parse_double(args, i, p.r)) {
        print_usage(argv[0]);
        return 2;
      }
    } else if (a == "--sigma") {
      if (!parse_double(args, i, p.sigma)) {
        print_usage(argv[0]);
        return 2;
      }
    } else if (a == "--T") {
      if (!parse_double(args, i, p.T)) {
        print_usage(argv[0]);
        return 2;
      }
    } else if (a == "--help" || a == "-h") {
      print_usage(argv[0]);
      return 0;
    } else {
      std::cerr << "Unknown flag: " << a << "\n";
      print_usage(argv[0]);
      return 2;
    }
  }

  if (type.empty()) {
    std::cerr << "--type is required\n";
    print_usage(argv[0]);
    return 2;
  }
  std::string why;
  if (!p.validate(&why)) {
    std::cerr << "Invalid params: " << why << "\n";
    return 2;
  }

  std::cout.setf(std::ios::fixed);
  std::cout << std::setprecision(6);

  double price = 0.0;
  bs::Greeks g{};
  if (type == "call") {
    price = bs::price_call(p);
    g = bs::greeks_call(p);
  } else if (type == "put") {
    price = bs::price_put(p);
    g = bs::greeks_put(p);
  } else {
    std::cerr << "Unknown option type: " << type << " (use call|put)\n";
    return 2;
  }

  std::cout << "Type: " << type << "\n"
            << "S=" << p.S << ", K=" << p.K << ", r=" << p.r << ", sigma=" << p.sigma
            << ", T=" << p.T << "\n\n";

  std::cout << "Price: " << price << "\n"
            << "Delta: " << g.delta << "\n"
            << "Gamma: " << g.gamma << "\n"
            << "Vega : " << g.vega << "  (per 1.0 sigma; per 1% = vega/100)\n"
            << "Theta: " << g.theta << "  (per year; per day ≈ theta/365)\n"
            << "Rho  : " << g.rho << " (per 1.0 rate)\n";

  return 0;
}