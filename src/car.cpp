#include "car.h"

#include <iostream>

Car::Car(int v, int space, int destination, int starting_from) {
    this->v = v;
    this->space = space;
    this->destination = destination;
    this->starting_from = starting_from;
    this->is_tail = false;
}

Car::Car(Car &head) {
    v = head.v;
    space = head.space;
    destination = head.destination;
    starting_from = head.starting_from;
    is_tail = true;
}

Car::~Car() {}

void Car::print() {
    std::cout << (is_tail ? "===Tail===" : "===Head===") << std::endl;
    std::cout << "v: " << v
              << " Space: " << space
              << " Destination: " << destination
              << " Starting from: " << starting_from << std::endl;
}
