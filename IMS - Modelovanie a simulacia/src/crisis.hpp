#include <vector>
#include <string>
#include "arguments.hpp"


class wave {
public:
    int start;
    int duration;

    double zeta_factor;
    double theta_factor;
    double c_factor;
    double budget_increase;
    
    bool extra_consumption;
    double new_wave_consumption_shock;


    wave(int start, int duration, double zeta_factor, double theta_factor, 
        double c_factor, double budget_increase, bool extra_consumption, double new_wave_consumption_shock) :
        start(start), duration(duration), zeta_factor(zeta_factor), 
        theta_factor(theta_factor), c_factor(c_factor), budget_increase(budget_increase),
        extra_consumption(extra_consumption), new_wave_consumption_shock(new_wave_consumption_shock) {
        }
};

class Crisis {
public:
    std::vector<bool> crisis_time;
    std::vector<wave> waves;
    
    Crisis();
    Crisis(Arguments *args);
};
