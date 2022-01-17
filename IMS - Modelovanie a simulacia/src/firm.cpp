#include <stdio.h>
#include <iostream>
#include "firm.hpp"
#include <iomanip>
#include <random>

Firm::Firm(Arguments *args) {
    std::random_device                      rand_dev;
    std::mt19937                            generator(rand_dev());
    std::uniform_real_distribution<double>  distr(0.0, 1.0);

    this->m_active = 1.0;
    this->m_price = 1 + 0.1 * (2.0 * distr(generator) - 1.0);
    this->m_production = (0.5 + 0.1 * (2.0 * distr(generator) - 1.0)) * args->productivity_factor;
    this->m_demand = 0.5;
    this->m_wage = args->productivity_factor; 
    this->m_profits_divs = this->m_price * MIN(this->m_demand, this->m_production) - this->m_wage * this->m_production;
    this->m_cash_balance = 0.0;
}

void Firm::print_header() {
    std::cout << "Status" << std::setw(13)
              << "Demand" << std::setw(13)
              << "Price" << std::setw(13)
              << "Wage" << std::setw(13)
              << "Production" << std::setw(13)
              << "Cash" << std::setw(13)
              << "Profit" << std::setw(13)
              << "Hire fact." << std::setw(13)
              << "Fire fact." << std::setw(13)
              << std::endl
              << "==========================================================================================" 
              << std::endl;
}

void Firm::print_cols() {
    std::cout << ((m_active == 1.0) ? "Alive" : "Dead") << std::setw(13)
              << m_demand << std::setw(13) 
              << m_price << std::setw(13)
              << m_wage << std::setw(13)
              << m_production << std::setw(13)
              << m_cash_balance << std::setw(13) 
              << m_profits_divs << std::setw(13)
              << std::endl;
}
