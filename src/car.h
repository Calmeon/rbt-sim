#pragma once

#include "agent.h"

class Car {
    int v;              // velocity in m/s
    int v_old;          // velocity in previous step
    int space;          // space ocuppied
    int destination;    // which exit will car use
    int starting_from;  // at which entry car appeared
    Car *head;          // tails have head pointer
    int waiting;        // how many steps is car waiting for lane change
    Agent *agent;       // Agent profile

   public:
    // head constructor
    Car(int v, int space, int destination, int starting_from, Agent *agent);
    // tail constructor (copy constructor)
    Car(Car *head);
    ~Car();

    int get_v();
    void set_v(int v);
    void save_v_old();
    int get_v_old();
    int get_space();
    bool get_is_tail();
    int get_starting_from();
    int get_destination();
    void set_waiting(int waiting);
    int get_waiting();

    int get_max_v();
    int get_dr();
    int get_a_plus();
    int get_a_minus();
    int get_force_lane_change();
    double get_change_bias();
    double get_wait_percent();
};
