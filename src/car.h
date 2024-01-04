#pragma once

#include "agent.h"

class Car {
    int v;              // velocity in m/s
    int v_old;          // velocity in previous step
    int v_used;
    int space;          // space ocuppied
    int destination;    // which exit will car use
    int starting_from;  // at which entry car appeared
    Car *head;          // tails have head pointer
    int tail_number;    // tail property which segment it is head = 1
    Agent *agent;       // Agent profile
    int exited_from;    // From which lane car exited

   public:
    // head constructor
    Car(int v, int space, int destination, int starting_from, Agent *agent);
    // tail constructor (copy constructor)
    Car(Car *head, int tail_number);
    ~Car();

    int get_v();
    void set_v(int v);
    void save_v_old();
    void set_v_old(int v_old);
    int get_v_used();
    void set_v_used(int v_used);
    int get_v_old();
    int get_v_available();
    int get_space();
    int get_tail_number();
    bool get_is_tail();
    int get_starting_from();
    int get_destination();
    void set_exited_from(int exited_from);
    int get_exited_from();
    void set_waiting(int waiting);
    int get_waiting();
    Car *get_head();

    int get_dr();
    double get_g();
    int get_a_plus();
    int get_a_minus();
    double get_wait_percent();
    double get_change_bias();
};
