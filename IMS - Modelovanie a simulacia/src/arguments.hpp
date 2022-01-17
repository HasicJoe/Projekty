#pragma once
#include <vector>
#include <string>

class Arguments
{
public:
    int number_of_firms; // Nf
    double init_consumption_propensity; // C_0
    double intensity_of_choice; // β
    double price_adjustment; // λ
    double firing_propensity; // η_0-
    double hiring_propensity; // η_0+ -- default value: h_f_ratio * firing_propensity
    double fraction_of_dividents; // δ
    double bancrupcy_treshold; // Θ
    double rate_of_firm_revival; // φ
    double productivity_factor; // ξ
    double EMA; // ω
    int simulation_time;
    double sensitivity_of_hh;
    double divider_factor; // f
    int crisis_scenario; // choose crisis simulation scenario

    Arguments();

    void print_help(int exit_code);

    void validate_args(std::vector<std::string> arg_str);

    void parse_args(std::vector<std::string> arg_str);
};