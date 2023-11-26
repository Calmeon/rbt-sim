#include "agent.h"

Agent::Agent(int max_v, int dr, int a_plus, int a_minus,
             int force_lane_change, double change_bias, double wait_percent) {
    this->max_v = max_v;
    this->dr = dr;
    this->a_plus = a_plus;
    this->a_minus = a_minus;
    this->force_lane_change = force_lane_change;
    this->change_bias = change_bias;
    this->wait_percent = wait_percent;
}

Agent::~Agent() {}

int Agent::get_max_v() { return max_v; }
int Agent::get_dr() { return dr; }
int Agent::get_a_plus() { return a_plus; }
int Agent::get_a_minus() { return a_minus; }
int Agent::get_force_lane_change() { return force_lane_change; }
double Agent::get_change_bias() { return change_bias; }
double Agent::get_wait_percent() { return wait_percent; }
