#pragma once
#include <string>

namespace bs {

struct Params {
  double S{};     // Spot price
  double K{};     // Strike
  double r{};     // Continuously-compounded risk-free rate (per year)
  double sigma{}; // Annualized volatility (per year, > 0)
  double T{};     // Time to maturity in years (>= 0)

  bool validate(std::string *why = nullptr) const;
};

}; // namespace bs