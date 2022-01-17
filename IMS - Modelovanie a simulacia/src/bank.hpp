#pragma once
#include "arguments.hpp"
#include <fstream>

#define MIN(_a, _b) \
    (((_a) < (_b)) ?  (_a) : (_b))

#define MAX(_a, _b) \
    (((_a) > (_b)) ?  (_a) : (_b))

class Bank_sector {

public:
    /// π^ema - "realizovana inflacia" (vyhladeny a spriemerovany priebeh okamzitej inflacie)
    double m_realized_inflation;
    /// π(t) - okmazita inflacia
    double m_immediate_inflation;

    /// ρ^l
    double m_loan_IR;
    double m_loan_IR_avg;

    /// ρ^d
    double m_deposit_IR;
    double m_deposit_IR_avg;

    /// default constructor
    Bank_sector();
    /**
     * bankovy sektor - sklada sa z "reprezentativnej banky"
     * a centralnej banky, ktora nastavuje zakladnu urokovu sadzbu.
     * 
     * Centralna banka ma tiez mandat na cielenie inflacie.
     * Centralna banka nastavuje zakladnu urokovu sadzbu ρ_0 pomocou Tayloroveho pravidla
     *
     * ρ_0(t) = ρ* + Φ_π[π^ema-π*]
     */ 
   

    
    /// Γ = max{ α_Γ(ρ^l(t) - π^ema(t), Γ_0) }

    /// ρ^l(t) = ρ_0(t) +f*(D(t)/ε^-)
    /// ρ^d(t) = (ρ_0(t)*ε^-(t)-(1-f)*D(t)) / (S + ε^+)
    void update_averages(double ema);

     /* 
     * 
     * Φ_π - moduluje intenzitu politiky centralnej banky
     * ρ* - zakladna urokova sadzba
     * π* - inflacny ciel pre centralnu banku nastavi urokovu sadzbu vkladov domacnosti ρ^d 
     *      a poziciek firiem ρ^l
     * 
     * ε^+ = Σ(max{ε_i, 0})
     * ε^- = -Σ(min{ε_i, 0})
     * 
     * ρ^l(t) = ρ_0(t) +f*(D(t)/ε^-)
     * ρ^d(t) = (ρ_0(t)*ε^-(t)-(1-f)*D(t)) / (S + ε^+)
     * 
     * D(t) - celkove naklady vzniknute v dosledku zlyhania firiem 
     * f - urcuje ako dopad tychto nesplatneni dopada na veritelov a vkladatelov - f interpoluje medzi nimi
     * naklady plne znasaju dlznici(f = 0) alebo plne vkladatelia (f = 0).
     * Celkom mnozstvor penazi (centralnej banky) M v obehu sa udrziava konstantne 
     * a bilancia bankovych sektorov je dana:
     * 
     * M = S(t) + ε^+(t) - ε^-(t)
     * 
     */
    void print_inflation_header(std::ofstream & out_file);
    void print_inflation_info(std::ofstream  & out_file);
};