# NUMERICS

**Assumptions:** European options, no dividends, risk-neutral measure, continuous compounding.


## Pricing
- $d_1 = \frac{\ln(S/K) + \left(r + \tfrac{1}{2}\sigma^2\right)T}{\sigma \sqrt{T}},  d_2 = d_1 - \sigma \sqrt{T}$

- Call: $C = S N(d_1) - K e^{-rT} N(d_2)$

- Put: $P = K e^{-rT} N(-d_2) - S N(-d_1)$

- $N(\cdot)$ via `0.5 * erfc(-x / sqrt(2))`


## Greeks (units)
- **Delta (Δ):** call $N(d_1)$; put $N(d_1)-1$ (dimensionless)
- **Gamma (Γ):** $\frac{n(d_1)}{S \sigma \sqrt{T}}$ (per 1 unit of S)
- **Vega:** $S n(d_1)\sqrt{T}$ (**per 1.0 σ**, i.e., per 100 vol points)
- **Theta (Θ):** per **year**; per day ≈ $\Theta/365$
- **Rho (ρ):** per **1.0** change in rate $r$

## Tolerances
We test prices to 1e-4 and Greeks to 1e-6 ~ 1e-3 depending on sensitivity and FD noise.

## Edge cases
- $T \to 0$: fallback to intrinsic; Greeks are ill-behaved at expiry.
- Very small $\sigma$: avoid divide-by-zero; validated via `Params::validate`.