#pragma once

#include <vector>
#include "arguments.hpp"
#include "household.hpp"
#include <fstream>
#include <string>
#include "firm.hpp"
#include "bank.hpp"
#include "crisis.hpp"


#define ZERO 1.0e-10

typedef std::vector<Firm *> Firms;

class Inflation_model 
{

public:
    Firms firms;
    Households *households;

    Crisis *crisis;

    /// zamestnanost
    double employment;
    /// nezamestnanost
    double unemployment;

    /// produkciou vazena priemerna cena
    double p_avg_price;
    double p_avg_old_price;
    /// najmensia hodnota
    double min_price;

    /// produkciou vazena priemerna mzda
    double p_avg_wage;
    double max_wage;

    double total_firm_debt;
    double total_firm_cash;

    /// D - big Di
    double total_costs_of_fail;

    double total_money_amount;

    /// normovane vyplaty
    double wage_norm;

    /// -Γ*Φ_i(t)
    /// lockdown
    double rel_dead_firm_count;

    /// podiel nezamestnanosti
    double unempl_share;

    /// ρ^d - urokova sadzba vkladov
    /// naklonnost k spotrebe domacnosti
    /// c(t) = c_0[1 + α_c(π^ema(t) - ρ^d(t))]
    /// α_c - moduluje citlivost domacnosti na skut. urokovu sadzbu

    double init_prod_factor;
    double init_bankrupcy_treshold;

    bool extra_cons_iter;

    Bank_sector *bank;

    Inflation_model(Arguments *args);

    ~Inflation_model();

    void print(Firms firms, std::ofstream  & out_file);
    void print_header(std::ofstream  & out_file);

    void renormalize_model();

    void add_crisis(Crisis *crisis) {
        this->crisis = crisis;
    }

    void main_cycle(int time, Arguments *args, std::ofstream & out_model, std::ofstream & out_bank);
};