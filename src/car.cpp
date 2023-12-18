#include "car.h"

Car::Car(int v, int space, int destination, int starting_from, Agent *agent) {
    this->v = v;
    this->v_old = 0;
    this->v_used = 0;
    this->space = space;
    this->destination = destination;
    this->starting_from = starting_from;
    this->head = nullptr;
    this->waiting = 0;
    this->tail_number = 1;
    this->agent = agent;
}
Car::Car(Car *head, int tail_number) {
    this->head = head;
    this->tail_number = tail_number;
}
Car::~Car() {}

int Car::get_v() { return head ? head->get_v() : v; }
void Car::set_v(int v) { this->v = v; }
void Car::save_v_old() { v_old = v; }
void Car::set_v_old(int v_old) { this->v_old = v_old; }
int Car::get_v_old() { return head ? head->get_v_old() : v_old; }
int Car::get_v_used() { return head ? head->get_v_used() : v_used; }
int Car::get_v_available() { return this->get_v() - this->get_v_used(); }
void Car::set_v_used(int v_used) { this->v_used = v_used; }
int Car::get_space() { return head ? head->get_space() : space; }
int Car::get_tail_number() { return tail_number; }
bool Car::get_is_tail() { return head; }
int Car::get_starting_from() { return starting_from; }
int Car::get_destination() { return destination; }
void Car::set_waiting(int waiting) { this->waiting = waiting; }
int Car::get_waiting() { return waiting; }
Car *Car::get_head() { return head ? head : this; }

int Car::get_max_v() { return agent->get_max_v(); }
int Car::get_dr() { return agent->get_dr(); }
double Car::get_g() { return (double)get_dr() * ((double)v / (double)get_max_v()); }
int Car::get_a_plus() { return agent->get_a_plus(); }
int Car::get_a_minus() { return agent->get_a_minus(); }
int Car::get_force_lane_change() { return agent->get_force_lane_change(); }
double Car::get_change_bias() { return agent->get_change_bias(); }
double Car::get_wait_percent() { return agent->get_wait_percent(); }
