#include "car.h"

Car::Car(int v, int space, int destination, int starting_from) {
    this->v = v;
    this->v_old = v;
    this->space = space;
    this->destination = destination;
    this->starting_from = starting_from;
    this->head = nullptr;
    this->waiting = 0;
}
Car::Car(Car *head) { this->head = head; }
Car::~Car() {}

int Car::get_v() { return v; }
void Car::set_v(int v) { this->v = v; }
void Car::save_v_old() { v_old = v; }
int Car::get_v_old() {
    if (head) return head->get_v_old();
    return v_old;
}
int Car::get_space() { return space; }
bool Car::get_is_tail() { return head; }
int Car::get_starting_from() { return starting_from; }
int Car::get_destination() { return destination; }
void Car::set_waiting(int waiting) { this->waiting = waiting; }
int Car::get_waiting() { return waiting; }
