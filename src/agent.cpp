#include "agent.h"

Agent::Agent(int dr, int a_plus, int a_minus,
             double wait_percent, double change_bias) {
    this->dr = dr;                      // badane <0,5> co 1
    this->a_plus = a_plus;              // 1-4
    this->a_minus = a_minus;            // 1-4
    this->wait_percent = wait_percent;  // osobne wartości dla linii
    this->change_bias = change_bias;    // zostawić, ale do wyłączania pasów
}

Agent::~Agent() {}

int Agent::get_dr() { return dr; }
int Agent::get_a_plus() { return a_plus; }
int Agent::get_a_minus() { return a_minus; }
double Agent::get_wait_percent() { return wait_percent; }
double Agent::get_change_bias() { return change_bias; }
