#include "inflation.hpp"
#include "arguments.hpp"
#include "firm.hpp"
#include "bank.hpp"
#include "household.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdlib.h> 
#include "time.h"
#include <iomanip>
#include <random>

double sum(Firms firms, Firm_prop property) {
    double sum = 0.0;
    for (auto firm : firms) {
        if (firm->m_active == 1.0) {
        switch (property) {
            case Production:
                sum += firm->m_production;
                break;
            case Cash_balance:
                sum += firm->m_cash_balance;
                break;
            case Active:
                sum += firm->m_active;
                break;
            case Price_times_production:
                sum += (firm->m_price * firm->m_production);
                break;
            case Wage_times_production:
                sum += (firm->m_wage * firm->m_production);
                break;
            }    
        }
    }
    return sum;
}

Inflation_model::Inflation_model(Arguments *args) {
    bank = new Bank_sector();
    households = new Households();

    /// create N firms
    for (int i = 0; i < args->number_of_firms; ++i)
        firms.emplace_back(new Firm(args));

    /// calc employment adn unemployment
    employment = (sum(firms, Production) / args->number_of_firms) / args->productivity_factor;
    unemployment = 1.0 - employment;
    
    /// production weighted props
    p_avg_price =     sum(firms, Price_times_production) / sum(firms, Production);
    p_avg_old_price = 1.0;
    /// max price assumption for calculating min price
    min_price =       1e20; 
    p_avg_wage =      sum(firms, Wage_times_production) /  sum(firms, Production);
    max_wage =        1.0;

    /// populate initial value of savings
    households->savings = args->number_of_firms * employment;

    /// set total amount of money -> setting to be 0
    total_money_amount = 0.0;

    /// relative count of dead firms
    rel_dead_firm_count = 0.0;
  
    /// save initial args value, for perfoming lockdowns
    init_bankrupcy_treshold = args->bancrupcy_treshold;
    init_prod_factor = args->productivity_factor;

    /// crisis param
    extra_cons_iter = false;
}

Inflation_model::~Inflation_model() {
    delete this->bank;
    delete this->households;
    for (auto firm : firms)
        delete firm;
}

void Inflation_model::main_cycle(int time, Arguments *args, std::ofstream  & out_model, std::ofstream & out_bank) {
    std::random_device                      rand_dev;
    std::mt19937                            generator(rand_dev());
    std::uniform_real_distribution<double>  distr(0.0, 1.0);
    double exp_arg = 0.0;

    /// update of cyclin' values
    bank->update_averages(args->EMA);

    wage_norm = 0.0;
    double max_wage = 0.0;
    for (auto firm : firms) 
        max_wage = MAX(firm->m_wage, max_wage);

    /// boltzmann averaging
    if (args->intensity_of_choice > 0.0) 
        for (auto firm : firms) 
            if (firm->m_active == 1.0) 
                wage_norm += exp(args->intensity_of_choice*(firm->m_wage - max_wage) / p_avg_wage);

    /// reset
    total_costs_of_fail = 0.0;
    total_firm_cash = 0.0;
    total_firm_debt = 0.0;

    double temp_value = 0.0;

    for (auto firm : firms) {
        if (firm->m_active == 1.0) {
            temp_value = firm->m_production * firm->m_wage / args->productivity_factor;

            if (firm->m_cash_balance > (- (args->bancrupcy_treshold+1.0) * temp_value)) {    
                if (args->intensity_of_choice > 0.0) {
                    exp_arg = args->intensity_of_choice * (firm->m_wage-max_wage)/p_avg_wage;
                    unempl_share = unemployment * args->number_of_firms * (1.0 - rel_dead_firm_count) * exp(exp_arg) / wage_norm;
                } else {
                    unempl_share = unemployment;
                }

                if (firm->m_demand > firm->m_production) {
                    firm->m_production += MIN(args->hiring_propensity*(firm->m_demand - firm->m_production), unempl_share*args->productivity_factor);
                    
                    /// increase price
                    if (firm->m_price < p_avg_price)
                        firm->m_price *= (1.0 + args->price_adjustment * distr(generator));
                    /// increase wage
                    // if (Y_i(t) < D_i(t) && P_i(t) > 0)
                    // W^T_i(t + 1) = W_i(t)*[1 + γ*(1 - Γ*Φ_i)(1-u(t))*ξ'_i(t)]*[1 + g*π_flex(t)]
                    if (firm->m_profits_divs > 0.0 && (args->price_adjustment * 1.0) > ZERO) 
                        firm->m_wage = MAX(firm->m_wage * (1.0 + args->price_adjustment * distr(generator) * employment), 0.0);
                ///excess production
                } else {
                    firm->m_production += args->firing_propensity * (firm->m_demand - firm->m_production);

                    /// decrease price
                    if (firm->m_price > p_avg_price)
                        firm->m_price = firm->m_price * (1.0 - args->price_adjustment * distr(generator));

                    /// decrease wage
                    if (firm->m_profits_divs < 0.0) 
                        firm->m_wage = MAX(firm->m_wage * (1.0 - args->price_adjustment * distr(generator) * unemployment), 0.0);
                }

                /// clip production <0, X>
                firm->m_production = MAX(firm->m_production, 0.0); 

                /// update track of total values
                total_firm_cash += MAX(firm->m_cash_balance, 0.0);
                total_firm_debt -= MIN(firm->m_cash_balance, 0.0);

                /// update min price
                min_price = MIN(min_price, firm->m_price);

            /// serve bankrupt firms and shut them down
            } else {
                total_costs_of_fail -= firm->m_cash_balance;
                firm->m_cash_balance = 0.0;
                firm->m_production = 0.0;
                firm->m_active = 0.0;
            }
        } 
    }
    p_avg_price = sum(firms, Price_times_production) / sum(firms, Production);
    p_avg_wage =  sum(firms, Wage_times_production)  / sum(firms, Production);

    employment = sum(firms, Production) / args->number_of_firms / args->productivity_factor;
    unemployment = 1.0 - employment;

    /// calc IR
    if (ABS(households->savings + total_firm_cash - total_costs_of_fail - total_firm_debt - total_money_amount) > 0.0) 
        households->savings -= households->savings + total_firm_cash - total_costs_of_fail - total_firm_debt - total_money_amount;
    
    bank->m_loan_IR = 0.0;
    if (total_firm_debt > 0.0)
        bank->m_loan_IR = (1.0 - args->divider_factor) * total_costs_of_fail / total_firm_debt;

    bank->m_deposit_IR = 0.0;
    if (households->savings + total_firm_cash > 0.0)
        bank->m_deposit_IR = ((bank->m_loan_IR * total_firm_debt) - total_costs_of_fail) / (households->savings + total_firm_cash);

    households->savings += bank->m_deposit_IR * households->savings;

    households->consumption_prop = args->init_consumption_propensity * (1.0 + args->sensitivity_of_hh * (bank->m_realized_inflation - bank->m_deposit_IR_avg));
    ///std::cout << bank->m_deposit_IR << "_____" << bank->m_loan_IR << std::endl;
    households->consumption_prop = MAX(MIN(households->consumption_prop, 1.0), 0.0);

    /// apply pandemic measures
    if (this->crisis->crisis_time[time]) {
        if (!extra_cons_iter) {
            households->consumption_prop = MIN(households->consumption_prop + this->crisis->waves[0].new_wave_consumption_shock, 1.0);
            extra_cons_iter = true;
        } else {
            this->households->consumption_prop *= this->crisis->waves[0].c_factor;
        }
        args->productivity_factor = this->crisis->waves[0].zeta_factor * this->init_prod_factor;
        for (auto firm : firms)
            firm->m_production = firm->m_production * this->crisis->waves[0].zeta_factor;

        /// increase theta a lot
        args->bancrupcy_treshold = init_bankrupcy_treshold * this->crisis->waves[0].theta_factor;
        if (!this->crisis->crisis_time[time+1]) {
            this->total_money_amount +=  this->crisis->waves[0].budget_increase * households->savings;
            this->households->savings += this->crisis->waves[0].budget_increase * households->savings;
            this->crisis->waves.erase(this->crisis->waves.begin());
            extra_cons_iter = false;
        }
    } else { 
        for (auto firm : firms)
            firm->m_production = firm->m_production * (this->init_prod_factor/args->productivity_factor);
        /// reset prod factor
        args->productivity_factor = this->init_prod_factor;
        args->bancrupcy_treshold = this->init_bankrupcy_treshold;
    }

    /// consumption
    households->consumption_budget = households->consumption_prop * ( sum(firms, Wage_times_production)/args->productivity_factor + MAX(households->savings, 0.0));
   
    /// boltzmann averaged price
    double Pnorm = 0.0;
    for (auto firm : firms) {
        if (firm->m_active == 1.0) {
            exp_arg = args->intensity_of_choice*(min_price-firm->m_price) / p_avg_price;
            Pnorm += exp(exp_arg);
        }
    }

    total_firm_cash = 0.0;
    for (auto firm : firms) {
        if (firm->m_active == 1.0) {
            exp_arg = args->intensity_of_choice * (min_price-firm->m_price)/p_avg_price;
            firm->m_demand = households->consumption_budget * exp(exp_arg) / Pnorm / firm->m_price;
       
            firm->m_profits_divs = firm->m_price*MIN(firm->m_production, firm->m_demand) 
                - (firm->m_production*firm->m_wage/args->productivity_factor) 
                - ABS(bank->m_loan_IR*MIN(firm->m_cash_balance, 0.0))
                + bank->m_deposit_IR*MAX(firm->m_cash_balance, 0.0);
            

            households->savings -= firm->m_price * MIN(firm->m_demand, firm->m_production) - (firm->m_production*firm->m_wage/args->productivity_factor);
            firm->m_cash_balance += firm->m_profits_divs;
         
            if (firm->m_cash_balance > 0.0 && firm->m_profits_divs > 0.0) {
                households->savings +=  args->fraction_of_dividents * firm->m_cash_balance;
                firm->m_cash_balance -= args->fraction_of_dividents * firm->m_cash_balance;
            }
            total_firm_cash += MAX(firm->m_cash_balance, 0.0);
        }
    }
    /// firm revival
    for (auto dead_firm : firms) {

        if (dead_firm->m_active == 0.0 && distr(generator) < args->rate_of_firm_revival) {
            dead_firm->m_production =       MAX(unemployment, 0.0) * distr(generator);

            dead_firm->m_price =            p_avg_price;
            dead_firm->m_wage =             p_avg_wage;
            dead_firm->m_cash_balance =     dead_firm->m_production*dead_firm->m_wage;
            dead_firm->m_profits_divs =     0.0;

            dead_firm->m_active =           1.0;

            total_costs_of_fail +=          dead_firm->m_cash_balance;
            total_firm_cash +=              dead_firm->m_cash_balance;

        }
    }
    /// end of firm revival
    /// reset values
    rel_dead_firm_count = 0.0;
    max_wage = 0.0;
    total_firm_debt = 0.0;

    for (auto firm : firms) {
        if (firm->m_active == 1.0) {
            if (total_firm_cash > 0.0 && firm->m_cash_balance > 0.0)
                firm->m_cash_balance -= total_costs_of_fail * firm->m_cash_balance / total_firm_cash;

            max_wage = MAX(firm->m_wage, max_wage);
            total_firm_debt -= MIN(firm->m_cash_balance, 0.0);
        } else
            rel_dead_firm_count += 1./args->number_of_firms;
    }

    p_avg_price = sum(firms, Price_times_production) / sum(firms, Production);
    p_avg_wage =  sum(firms, Wage_times_production)  / sum(firms, Production);

    /// calculate inflation
    bank->m_immediate_inflation = (p_avg_price - p_avg_old_price) / p_avg_old_price;
    p_avg_old_price = p_avg_price;

    employment = MAX(MIN((sum(firms, Production) / args->number_of_firms) / args->productivity_factor, 1.0), 0.0);
    unemployment = 1.0 - employment;

    this->print(firms, out_model);
    bank->print_inflation_info(out_bank);
}   

void Inflation_model::print_header(std::ofstream  & out_file) {
    out_file <<"Employment;Unemployment;Average price;Price min;Wage average;Wage max;Savings;Total debt;Total firm cash;Deficit;Total money;Wage normalizes;Relative dead count;Unemployment share;Consumption propensity;Consumption budget;Sum production" << std::endl;
}

void Inflation_model::print(Firms firms, std::ofstream  & out_file) {
    out_file <<employment<< ";"
    << unemployment<< ";"
    << p_avg_price<< ";"
    << min_price<< ";"
    << p_avg_wage<< ";"
    << max_wage<< ";"
    << households->savings<< ";"
    << total_firm_debt<< ";"
    << total_firm_cash<< ";"
    << total_costs_of_fail<< ";"
    << total_money_amount<< ";"
    << wage_norm<< ";"
    << rel_dead_firm_count<< ";"
    << unempl_share<< ";"
    << households->consumption_prop<< ";"
    << households->consumption_budget<< ";"
    << sum(firms, Production)
    << std::endl;
}

int main(int argc, char* argv[]) {
    Arguments* args = new Arguments();
    std::vector<std::string> arg_str(argv+1, argv + argc);
    args->parse_args(arg_str);

    /// print input args
    std::cout
    << std::endl << "Firm count: " << args->number_of_firms
    << std::endl << "Consumption propensity: " << args->init_consumption_propensity
    << std::endl << "Intensity of choice: " << args->intensity_of_choice
    << std::endl << "Price adj.: " << args->price_adjustment
    << std::endl << "Firing prop: " << args->firing_propensity
    << std::endl << "Hiring prop: " << args->hiring_propensity
    << std::endl << "Fraction of dividents: " << args->fraction_of_dividents
    << std::endl << "Bankrupcy treshold: " << args->bancrupcy_treshold
    << std::endl << "Rate of firm revival: " << args->rate_of_firm_revival
    << std::endl << "Prod factor: " << args->productivity_factor
    << std::endl << "EMA: " << args->EMA
    << std::endl << "Sensitivity of HH: " << args->sensitivity_of_hh
    << std::endl << "Deposits/Loans divider: " << args->divider_factor
    << std::endl << "Simulation time: " << args->simulation_time
    << std::endl;

    /// model initialization
    Inflation_model model(args);

    /// init crisis
    Crisis *crisis = new Crisis(args);
    model.add_crisis(crisis);

    std::ofstream out_model;
    std::ofstream out_bank;

    out_model.open("model.data");
    out_bank.open("bank.data");

    model.print_header(out_model);
    model.bank->print_inflation_header(out_bank);
    
    /// simulation of the model
    for (int t = 0; t < args->simulation_time; ++t) {
        model.main_cycle(t, args, out_model, out_bank);
        if (t % 120 == 0)
            std::cout << "month: " << t << " active firms: " << sum(model.firms, Active) << std::endl;

    }
    
    out_model.close();
    out_bank.close();

    /// clear memory
    delete args;
    delete crisis;

    return 0;
}