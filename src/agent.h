#pragma once

class Agent {
    int max_v;
    int dr;
    int a_plus;
    int a_minus;
    int force_lane_change;  // how long will agent wait before force lane change
    double change_bias;     // <-1,1> how fast agent changes lanes lower is faster
                            // for example it will be faster on lane no. 2
    double wait_percent;    // how far car goes until it stops and wait to change lane

   public:
    Agent(int max_v, int dr, int a_plus, int a_minus,
          int force_lane_change, double change_bias, double wait_percent);
    ~Agent();

    int get_max_v();
    int get_dr();
    int get_a_plus();
    int get_a_minus();
    int get_force_lane_change();
    double get_change_bias();
    double get_wait_percent();
};
