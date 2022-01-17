#include <iostream>
#include "arguments.hpp"

Arguments::Arguments() {
    this->number_of_firms = 18000;              // Nf
    this->init_consumption_propensity = 0.5;    // C_0
    this->intensity_of_choice = 2.0;            // β
    this->price_adjustment = 0.01;              // λ
    this->firing_propensity = 0.2;              // η_0-
    this->hiring_propensity = 0.4;              // η_0+ 
    this->fraction_of_dividents = 0.02;         // δ
    this->bancrupcy_treshold = 3.0;             // Θ
    this->rate_of_firm_revival = 0.10;          // φ
    this->productivity_factor = 1.0;            // ξ
    this->EMA = 0.2;                            // ω
    this->simulation_time = 1200;               // 100 - years of simulation
    this->sensitivity_of_hh = 4.0;              // alpha_c
    this->divider_factor = 0.5;                 // f
    this->crisis_scenario = 0;                  // cs
}

void Arguments::print_help(int exit_code){
    std::cout << "Mark-0 model" << std::endl;
    std::cout << "[Usage] ./inflation [-n I] [-c D] [-i D] [-a D] [-fp D] [-hp D] [-d D] [-t D] [-fr D] [-p D] [-e D] [-cf D] [-T I] [-cs I]" << std::endl;
    std::cout << "I = integer value, D = double value" << std::endl;
    std::cout << "[-n I]  = Number of firms - default value 18000" << std::endl;
    std::cout << "[-c D]  = Initialconsuption propensity - default value 0.5" << std::endl;
    std::cout << "[-i D]  = Intensity of choice parameter - default value 2.0" << std::endl;
    std::cout << "[-a D]  = Price adjustment parameter - default value 0.01" << std::endl;
    std::cout << "[-fp D] = Firing propensity - default value 0.2" << std::endl;
    std::cout << "[-hp D] = Hiring propensity - default value 0.4" << std::endl;
    std::cout << "[-d D]  = Fraction of dividents - default value 0.02" << std::endl;
    std::cout << "[-t D]  = Bancrupcy treshold - default value 4.0" << std::endl;
    std::cout << "[-fr D] = Rate of firm revival - default value 0.1" << std::endl;
    std::cout << "[-p D]  = Productivity factor - default value 1.0" << std::endl;
    std::cout << "[-ac D] = Sensitivity of households - default value 4.0" << std::endl;
    std::cout << "[-f D]  = Divider factor - default value 0.5" << std::endl;
    std::cout << "[-e D]  = Exponentialy moving average (ema) parameter - default value 0.2" << std::endl;
    std::cout << "[-cf D] = Factor by which to reduce consumption during measures - default value 0.5" << std::endl;
    std::cout << "[-T I]  = Simulation time (in months) - default value 120 months" << std::endl;
    std::cout << "[-cs I] = Crisis scenario:\t(1-5)\n\t1 - ???\n\t2 - ???\n\t3 - ???\n\t4 - ???\n\t5 - ???\n" << std::endl;
    
    exit(exit_code);   
}

void Arguments::validate_args(std::vector<std::string> arg_str){
    for(unsigned int i = 1; i < arg_str.size(); i = i+2) {
        try {
            std::string str = arg_str[i];
            std::string::size_type check_double;
            double double_var = std::stod(str, &check_double);
            if(str.substr(check_double).length() > 0){
                throw std::invalid_argument("Error: invalid argument");
            }
            if(double_var < 0.0){
                throw std::invalid_argument("Error: invalid argument");
            }
        }
        catch(const std::invalid_argument& ia) {
            std::cout << "Error arg:" << arg_str[i] << " is not a number." << std::endl;
            print_help(1);
        }
    }
}


void Arguments::parse_args(std::vector<std::string> arg_str) {
    if(arg_str.size() == 1 && (arg_str[0] == "-h" || arg_str[0] == "--help")) {
        this->print_help(0);
    }
    this->validate_args(arg_str);
    for(unsigned int i = 0; i < arg_str.size(); i++) {
        if(arg_str[i] == "-n") {
            this->number_of_firms = std::stoi(arg_str[++i]);
        }
        else if(arg_str[i] == "-c") {
            this->init_consumption_propensity = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-i") {
            this->intensity_of_choice = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-a") {
            this->price_adjustment = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-fp") {
            this->firing_propensity = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-hp") {
            this->hiring_propensity = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-d") {
            this->fraction_of_dividents = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-t") {
            this->bancrupcy_treshold = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-fr") {
            this->rate_of_firm_revival = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-p") {
            this->productivity_factor  = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-e") {
            this->EMA = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-T") {
            this->simulation_time = std::stoi(arg_str[++i]);
        }
        else if(arg_str[i] == "-ac") {
            this->sensitivity_of_hh = std::stod(arg_str[++i]);
        }
        else if(arg_str[i] == "-f") {
            this->divider_factor = std::stod(arg_str[++i]);
        } 
        else if(arg_str[i] == "-cs") {
            this->crisis_scenario = std::stoi(arg_str[++i]);
        }
        else{
            this->print_help(1);
        }
    }
}