struct Params {
    // r: continuously compounded rate (per year)
    // T: years
    // sigma: annualized vol
    double S, K, r, sigma, T;
};

bool validate(std::string* why=nullptr) const;