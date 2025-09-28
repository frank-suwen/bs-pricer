// Manual argv parsing (Stage-1, minimal):
// read --type, --S, --K, --r, --sigma, --T.
// On success, print price + Greeks in a neat block;
// On bad input, return non-zero with a short usage string.
#include <iostream>
int main(int, char**) {
    std::cout << "bs_cli: Black-Scholes pricer (Stage-1 skeleton)\n";
    std::cout << "Usage (coming soon): bs --type call --S 100 --K 100 -- r 0.05 --sigma 0.2 --T 1\n";
    return 0;
}