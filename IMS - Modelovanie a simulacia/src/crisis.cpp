#include "crisis.hpp"


Crisis::Crisis(Arguments *args) {
    int start = 780;//args->simulation_time - (60 * 12);

    for (int i = 0; i < args->simulation_time; ++i)    
        this->crisis_time.emplace_back(false);
    /**
     *  wave(int start, int duration, double zeta_factor, double theta_factor, 
        double c_factor, double budget_increase, bool extra_consumption, double new_wave_consumption_shock)
     * 
     */

    switch(args->crisis_scenario) {
        case 1:
            this->waves.emplace_back(wave(start, 3, 0.55, 1.0, 1.0, 0.0, false, 1.0));
            break;
        case 2:
            this->waves.emplace_back(wave(start, 6, 0.55, 25.0, 0.3, 0.45, false, 1.0));
            break;
        case 3:
            this->waves.emplace_back(wave(start, 3, 0.5, 75.0, 0.5, 1.25, false, 0.15));
            this->waves.emplace_back(wave(start+12, 3, 0.5, 75.0, 0.5, 1.25, false, 0.0));
            break;
        case 4:
            this->waves.emplace_back(wave(start, 2, 0.4, 100.0, 0.5, 1.2, false, 0.3));
            this->waves.emplace_back(wave(start+6, 6, 0.75, 15.0, 0.5, 1.1, false, 0.2));
            this->waves.emplace_back(wave(start+18, 2, 1.2, 5.0, 0.5, 1.0, false, 0.05));
        default:
            break;
    }      
    for (auto wave : waves) 
        std::fill(this->crisis_time.begin()+wave.start, this->crisis_time.begin()+wave.start+wave.duration, true);
}