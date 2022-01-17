#pragma once
#include "arguments.hpp"


#define MIN(_a, _b) \
    (((_a) < (_b)) ?  (_a) : (_b))

#define MAX(_a, _b) \
    (((_a) > (_b)) ?  (_a) : (_b))

#define ABS(_x) \
    (((_x) > 0.0) ? (_x) : -(_x))

typedef enum firm_prop {
    Production,
    Cash_balance,
    Active,
    Price_times_production,
    Wage_times_production
} Firm_prop;

class Firm { 

public:
    /// N_F - pocet firiem (asi sa nebude ukladat v instancii firmy)
    /// dopyt - D_i(t)
    double m_demand;
    /// cena - p_i(t)
    double m_price;
    /// mzda - W_i
    double m_wage;
    /// pracovna sila N_i 
    /// double m_work_force;
    /// produkcia Y_i = ξ*N_i
    double m_production;
    /// hotovostny zostatok - ε_i
    double m_cash_balance;
    /// profits and dividents P_i(t)
    double m_profits_divs;
    /// whether this firm has not bancrupted yet
    double m_active;

    void renormalize(double avg_price);

    /// Dopyt po tovare D_i - modelovany prostrednictvom modelu intenzity vyberu s parametrom β,
    /// ktory definuje zavislost dopytu od ceny
    /// D_i(t) = C_B(t)*exp(-β*p_i(t))/p_i(t)*Σexp(-β*p_i(t))
    /**
     * 
     * if (Φ_i < θ) {
     *      tok potrebnych uverov z banky nie je prilis vysoky v porovnani s velkostou spolocnosti 
     *      (merane ako celkova mzda)
     *      firma(i) moze pokracovat vo svojom konani
     * } else (Φ_i ≥ θ) {
     *      firma(i) zlyha a prislusne naklady na zlyhanie absorbuje bankovy sektor, 
     *      ktory upravi urokove sadzby poziciek a vkladov (ρ^l - pozicky firiem, ρ^d - vklady domacnosti).
     *      Zlyhavna firma je nahradena novou pri sadzbe φ, inicializovanou nahodne.
     * }
     * 
     * Parameter θ - riadi maximalnu paku v ekonomike a modeluje politiku kontroly rizika bankoveho sektoru.
     * 
     */ 

    /**
     * Aktualizacia produkcie Y_i(t)
     * 
     * If (Y_i(t) < D_i(t)) {
     *      Y_i(t + 1) = Y_i(t) + min{η+_i(D_i(t) - Y_i(t)), ξu*_i(t)}
     * }
     * 
     * If(Y_i(t) > D_i(t)) {
     *      Y_i(t + 1) = Y_i(t) - η-_i[Y_i(t) - D_i(t)]
     * }
     * 
     * 
     * u*_i(t) - maximalny pocet nezamestnanych pracovnikov v case (t) dostupnych firme(i), ktory zavisi od mzdy (pracovnik),
     *           ktoru firma vyplaca
     * 
     * u*_i = exp(β*W_i(t)/w_dash(t))/Σ(exp(β*W_i(t)/w_dash(t)))
     * 
     * w_dash - produkciou vazena priemerna mzda
     * 
     * firmy prijimaju a vyhadzuju zamestnancov podla ich urovne financnej krehkosti Φ_i
     * 
     * cim blizsie k bankrotu, tym rychlejsie vyhadzuju a pomalsie naberaju zamestnancov, analogicky v opacnom pripade
     */

    /**
     * 
     * Koeficienty η+- ∈ <0, 1>
     *  - vyjadruju citlivost cielovej produkcie firmy vzhladom na prebytocnu ponuku/dopyt
     * 
     * η-_i = [ η-_0(1 + Γ*Φ_i(t))]
     * η+_i = [ η+_0(1 - Γ*Φ_i(t))]
     * 
     * koeficienty su zarovnane na interval <0, 1> t.j., pre ([x] < 0) => 0, pre ([x] >= 1) => 1
     * 
     * fixne koeficienty identicke napriec vsetkymi firmami
     * faktor Γ > 0, meria, ako financna nestabilita firiem ovplyvnuje ich politiku (naboru/prepustania)
     * 
     * Γ - zavisi od urokovej sadzby updravenej o inflaciu
     * 
     * Γ = max{ α_Γ(ρ^l(t) - π^ema(t), Γ_0) }
     * 
     * α_Γ - volny parameter, zachytava vplyv realny urokovej miery
     */

    /**
     * Aktualizacia ceny : ceny sa aktualizuju nahodnym multiplikacnym procesom ...
     * 
     * if (Y_i(t) < D_i(t)) {
     *      if (p_i(t) < p_dash(t)) {
     *          p_i(t + 1) = p_i(t)*(1 + γ*ξ_i(t))*(1 + π_flex(t))
     *      }
     * 
     *      /// no change
     *      if (p_i(t) >= p_dash(t)) {
     *          p_i(t + 1) = p_i(t)
     *      }
     * }
     * 
     * if (Y_i(t) > D_i(t)) {
     *      if (p_i(t) > p_dash(t)) {
     *          p_i(t + 1) = p_i(t)*(1 - γ*ξ_i(t))*(1 + π_flex(t))
     *      }
     * 
     *      /// no change
     *      if (p_i(t) <= p_dash(t)) {
     *          p_i(t + 1) = p_i(t) 
     *      }
     *  
     * }
     * 
     * ξ_i(t) ∈ U[0, 1] - nezavisla nahodna premenna s uniformnym rozlozenim 
     * γ - parameter upravy ceny (vstupny parameter)
     * faktor (1 + π_flex(t)) - modeluje ocakavanu inflaciu firiem, ked stanovuju svoje ceny a mzdy
     * 
     */

    /**
     * Aktualizacia platov - riadi sa vyberom ceny a vyroby, 
     * v kazdom casovom kroku firma aktualizuje mzdy vyplacane svojim zamestnancom nasledovne:
     * 
     * if (Y_i(t) < D_i(t) && P_i(t) > 0)
     *      W^T_i(t + 1) = W_i(t)*[1 + γ*(1 - Γ*Φ_i)(1-u(t))*ξ'_i(t)]*[1 + g*π_flex(t)]
     * 
     * 
     * if (Y_i(t) > D_it() && P_i(t) < 0)
     *      W_i(t + 1) = W_i(t)*[1 - γ*(1 + Γ*Φ_i)*u(t)*ξ'_i(t)]*[1 + g*π_flex(t)]
     * 
     * 
     * P_i - profit firmy
     * ξ'_i(t) ∈ U[0, 1] - nezavisla nahodna premenna s uniformnym rozlozenim 
     * g - moduluje, ako sa mydz indexuju podla inflacnych ocakavani firiem
     * 
     * poznamka: v ramci tohto modelu produktivia nie je priamo zavisla na vyplate
     */

    /**
     * Profity a dividenty: zisky firmy(i) sa vypocitaju ako 
     * trzby minus vyplacane mzdy s pripocitanim urokov ziskanych z ich vkladov a zaplatenych urokov
     * 
     * P_i = p_i(t) * min{Y_i(t), D_i(t)} - W_i(t)*Y_i(t) + ρ^d*max{ε_i(t), 0} - ρ^l*min{ε_i(t), 0}
     * 
     * Ak su zisky kladne a firma(i) ma kladny hotovostny zostatok (m_cash_balance), 
     * vyplaca dividenty ako zlomok δ hotovostneho zostatku ε_i
     * 
     * Δ(t) = δ*Σ(ε_i(t) * θ(P_i(t)) * θ(ε_i(t)))
     * 
     * θ(x) - Heaviside-ova funkcia - { 1 pre x > 0, inak 0 }
     */


    /// default constructor (initial parameters)
    Firm(Arguments *);

    void print_header();
    void print_cols();
};