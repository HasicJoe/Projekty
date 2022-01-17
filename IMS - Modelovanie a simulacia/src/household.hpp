#pragma once

class Households { 

public:    
    /// celkovy rozpocet spotreby domacnosti
    /// Cb(t) =  c[S(t)+W(t)+ρ^d(t)S(t)]
    double consumption_budget;
    /// C(t) < C_B(t) = aktualna spotreba domacnosti, urcena zosuladenim produkcie a dopytu
    /// C(t) = Σ(i, N_F) (P_i min{Y_i, D_i}) ≤ (C_B(t) = Σ(i, N_F) (P_i(t)D_i(t)))
    double consumption_prop;
    /// S(t) - uspory   
    /// W(t) = Σ Wi(t)Ni(t) -> netusim

    /// Vyvoj domacich uspor
    /// S(t + 1) = S(t) + W(t) + ρ^d(t)S(t)-C(t)+Δ(t)
    double savings;
    
    Households() : consumption_budget(0.0), consumption_prop(0.0), savings(0.0) {
    }
};
