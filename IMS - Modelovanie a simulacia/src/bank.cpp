#include "bank.hpp"
#include <iostream>
#include <iomanip>

Bank_sector::Bank_sector() {
    this->m_realized_inflation = 0.0;
    this->m_immediate_inflation = 0.0;
}

/// Γ = max{ α_Γ(ρ^l(t) - π^ema(t), Γ_0) }

/// ρ^l(t) = ρ_0(t) +f*(D(t)/ε^-)
/// f - divider factor

/// ρ^d(t) = (ρ_0(t)*ε^-(t)-(1-f)*D(t)) / (S + ε^+)

void Bank_sector::update_averages(double ema) {
    this->m_realized_inflation =    ema * this->m_immediate_inflation + (1.0 - ema) * this->m_realized_inflation;
    this->m_deposit_IR_avg =        ema * this->m_deposit_IR + (1.0 - ema) * this->m_deposit_IR_avg;
    this->m_loan_IR_avg =           ema * this->m_loan_IR + (1.0 - ema) * this->m_loan_IR_avg;
}


void Bank_sector::print_inflation_header(std::ofstream  & out_file) {
    out_file << "Realized inflation;Immediate inflation;Loan interest rate;Average loan interest rate;Deposit interest rate;Average deposit IR\n";
}

void Bank_sector::print_inflation_info(std::ofstream & out_file) {
    out_file << m_realized_inflation << ";"
             << m_immediate_inflation << ";"
             << m_loan_IR << ";"
             << m_loan_IR_avg << ";"
             << m_deposit_IR << ";"
             << m_deposit_IR_avg << std::endl;
}