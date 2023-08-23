#include "roundabout.h"

#include <math.h>

#include <iostream>

#define ROAD_WIDTH 3.0

int proper_idx(int idx, int vector_size) {
    return (idx + vector_size) % vector_size;
}

Rounbabout::Rounbabout(
    float island_radius,
    int number_of_lanes,
    int max_velocity,
    float density) {
    this->island_radius = island_radius;
    this->max_velocity = max_velocity;
    this->density = density;
    this->second = 0;

    // calculate and initialize lanes
    // add to island radius half of raod width
    float radius = island_radius + ROAD_WIDTH / 2;
    int length;
    // from circle circuit formula assign appropiate lengths
    for (int lane = 0; lane < number_of_lanes; lane++) {
        length = 2 * M_PI * (radius + ROAD_WIDTH * lane);
        lanes.push_back(std::vector<Car *>(length, nullptr));
    }
}

Rounbabout::~Rounbabout() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (car != nullptr) {
                delete car;
                car = nullptr;
            }
        }
    }
}

void Rounbabout::print() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (car == nullptr) {
                std::cout << ".";
            } else if (car->get_is_tail()) {
                std::cout << ">";
            } else {
                std::cout << car->get_v();
            }
        }
        std::cout << std::endl;
    }
}

void Rounbabout::add_car(int lane, int idx, int space) {
    Car *head = new Car(0, space, -1, -1);
    Car *tail;
    idx = proper_idx(idx, lanes[lane].size());
    lanes[lane][idx] = head;

    int tail_idx;
    for (int i = 1; i < space; i++) {
        tail = new Car(*head);
        tail_idx = proper_idx(idx - i, lanes[lane].size());
        lanes[lane][tail_idx] = tail;
    }
}

void Rounbabout::delete_tails() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (car != nullptr && car->get_is_tail()) {
                delete car;
                car = nullptr;
            }
        }
    }
}

void Rounbabout::fix_tails() {
    int tail_idx;
    for (auto &lane : lanes) {
        for (int idx = 0; idx < (int)lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                for (int t = 1; t < lane[idx]->get_space(); t++) {
                    tail_idx = proper_idx(idx - t, lane.size());
                    lane[tail_idx] = new Car(*lane[idx]);
                }
            }
        }
    }
}

// transition functions
void Rounbabout::accelerate() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (car != nullptr && !car->get_is_tail()) {
                if (car->get_v() < max_velocity) {
                    car->set_v(car->get_v() + 1);
                }
            }
        }
    }
}

void Rounbabout::move() {
    delete_tails();

    int next_idx;
    for (auto &lane : lanes) {
        for (int idx = 0; idx < (int)lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                if (moved.find(lane[idx]) == moved.end()) {
                    next_idx = idx + lane[idx]->get_v();
                    next_idx = proper_idx(next_idx, lane.size());
                    lane[next_idx] = lane[idx];
                    lane[idx] = nullptr;
                    moved.insert(lane[next_idx]);
                }
            }
        }
    }

    fix_tails();
}

void Rounbabout::simulate() {
    accelerate();
    move();
    moved.clear();
    second++;
    print();
}
