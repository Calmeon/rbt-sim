#pragma once

class Agent {
    int dr;
    int a_plus;
    int a_minus;
    double wait_percent;  // how far car goes until it stops and wait to change lane
    double change_bias;   // <-1,1> how fast agent changes lanes lower is faster
                          // for example it will be faster on lane no. 2

   public:
    Agent(int dr, int a_plus, int a_minus,
          double wait_percent, double change_bias);
    ~Agent();

    int get_dr();
    int get_a_plus();
    int get_a_minus();
    double get_wait_percent();
    double get_change_bias();
};
