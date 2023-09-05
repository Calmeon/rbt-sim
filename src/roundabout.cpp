#include "roundabout.h"

#include <cmath>
#include <iostream>
#include <random>

#include "car.h"
#include "helpers.h"
#include "settings.h"

Roundabout::Roundabout(
    float island_radius,
    std::map<int, float> &entries,
    std::map<int, float> &exits,
    int number_of_lanes,
    int max_velocity,
    float density,
    int exits_entries_len) {
    // add to island radius half of raod width
    float radius = island_radius + ROAD_WIDTH / 2;
    int length;

    this->island_radius = island_radius;
    this->max_velocity = max_velocity;
    this->density = density;
    this->second = 0;

    // calculate and initialize lanes
    // from circle circuit formula assign appropiate lengths
    for (int lane = 0; lane < number_of_lanes; lane++) {
        length = 2 * M_PI * (radius + ROAD_WIDTH * lane);
        this->lanes.push_back(std::vector<Car *>(length, nullptr));
    }
    // initialize entries
    for (auto &entry : entries) {
        this->entries[entry.first] = std::vector<Car *>(exits_entries_len, nullptr);
        this->entries_chances[entry.first] = entry.second;
    }
    // initialize exits
    for (auto &exit : exits) {
        this->exits[exit.first] = std::vector<Car *>(exits_entries_len, nullptr);
        this->exits_chances[exit.first] = exit.second;
    }
}

Roundabout::~Roundabout() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (car != nullptr) {
                delete car;
                car = nullptr;
            }
        }
    }

    for (auto &lane : entries) {
        for (auto &car : lane.second) {
            if (car != nullptr) {
                delete car;
                car = nullptr;
            }
        }
    }

    for (auto &lane : exits) {
        for (auto &car : lane.second) {
            if (car != nullptr) {
                delete car;
                car = nullptr;
            }
        }
    }
}

void Roundabout::print() {
    int l = 0;
    for (auto &lane : lanes) {
        std::cout << "[l" << l++ << "] ";
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

    for (auto &entry : entries) {
        std::cout << "[ent" << entry.first << "] ";
        for (auto &car : entry.second) {
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

    for (auto &exit : exits) {
        std::cout << "[ext" << exit.first << "] ";
        for (auto &car : exit.second) {
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
    std::cout << std::endl;
}

void Roundabout::add_car_rbt(int lane, int idx, int space) {
    Car *head = new Car(0, space, -1, -1);
    Car *tail;
    int tail_idx;

    idx = proper_idx(lanes[lane], idx);
    lanes[lane][idx] = head;

    for (int i = 1; i < space; i++) {
        tail = new Car(*head);
        tail_idx = proper_idx(lanes[lane], idx - i);
        lanes[lane][tail_idx] = tail;
    }
}

void Roundabout::add_car(int entry, int v, int space, int destination) {
    Car *head = new Car(v, space, destination, entry);
    entries[entry][0] = head;
}

void Roundabout::delete_tails_ee(std::map<int, std::vector<Car *>> &e) {
    for (auto &pair : e) {
        for (auto &car : pair.second) {
            if (car != nullptr && car->get_is_tail()) {
                delete car;
                car = nullptr;
            }
        }
    }
}

void Roundabout::delete_tails() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (car != nullptr && car->get_is_tail()) {
                delete car;
                car = nullptr;
            }
        }
    }
    delete_tails_ee(entries);
    delete_tails_ee(exits);
}

void Roundabout::fix_tails_ee(std::map<int, std::vector<Car *>> &e) {
    int tail_idx;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (size_t idx = 0; idx < lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                for (int t = 1; t < lane[idx]->get_space(); t++) {
                    tail_idx = idx - t;
                    if (tail_idx >= 0) {
                        lane[tail_idx] = new Car(*lane[idx]);
                    }
                }
            }
        }
    }
}

void Roundabout::fix_tails() {
    int tail_idx;

    for (auto &lane : lanes) {
        for (size_t idx = 0; idx < lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                for (int t = 1; t < lane[idx]->get_space(); t++) {
                    tail_idx = proper_idx(lane, idx - t);
                    lane[tail_idx] = new Car(*lane[idx]);
                }
            }
        }
    }
    fix_tails_ee(entries);
    fix_tails_ee(exits);
}

// transition functions

void Roundabout::generate_cars() {
    int v, space;
    for (auto &entry : entries) {
        if ((float)rand() / (float)RAND_MAX > entries_chances[entry.first] &&
            entry.second[0] == nullptr) {
            v = rand() % (max_velocity + 1);
            space = weighted_random_choice(cars_sizes);

            add_car(entry.first, v, space, -1);
        }
    }
}

void Roundabout::accelerate_ee(std::map<int, std::vector<Car *>> &e) {
    for (auto &lane : e) {
        for (auto &car : lane.second) {
            if (car != nullptr && !car->get_is_tail()) {
                if (car->get_v() < max_velocity) {
                    car->set_v(car->get_v() + 1);
                }
            }
        }
    }
}

void Roundabout::accelerate() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (car != nullptr && !car->get_is_tail()) {
                if (car->get_v() < max_velocity) {
                    car->set_v(car->get_v() + 1);
                }
            }
        }
    }
    accelerate_ee(entries);
    accelerate_ee(exits);
}

void Roundabout::brake_ee(std::map<int, std::vector<Car *>> &e) {
    int d_to_next;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (size_t idx = 0; idx < lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                d_to_next = find_next(lane, idx);
                // second check if for not breaking at the end of the lane
                if (d_to_next < lane[idx]->get_v() && d_to_next < (int)(lane.size() - (idx + 1))) {
                    lane[idx]->set_v(d_to_next);
                }
            }
        }
    }
}

void Roundabout::brake() {
    int d_to_next;

    for (auto &lane : lanes) {
        for (size_t idx = 0; idx < lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                d_to_next = find_next(lane, idx);
                if (d_to_next < lane[idx]->get_v()) {
                    lane[idx]->set_v(d_to_next);
                }
            }
        }
    }
    brake_ee(entries);
    brake_ee(exits);
}

void Roundabout::move_ee(std::map<int, std::vector<Car *>> &e) {
    int next_idx;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (size_t idx = 0; idx < lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                // check if car was moved before
                if (moved.find(lane[idx]) == moved.end()) {
                    next_idx = idx + lane[idx]->get_v();
                    next_idx = proper_idx(lane, next_idx);
                    lane[next_idx] = lane[idx];
                    lane[idx] = nullptr;
                    moved.insert(lane[next_idx]);
                }
            }
        }
    }
}

void Roundabout::move() {
    int next_idx;

    delete_tails();
    for (auto &lane : lanes) {
        for (size_t idx = 0; idx < lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                // check if car was moved before
                if (moved.find(lane[idx]) == moved.end()) {
                    next_idx = idx + lane[idx]->get_v();
                    next_idx = proper_idx(lane, next_idx);
                    lane[next_idx] = lane[idx];
                    lane[idx] = nullptr;
                    moved.insert(lane[next_idx]);
                }
            }
        }
    }
    move_ee(entries);
    move_ee(exits);
    fix_tails();
}

void Roundabout::simulate() {
    generate_cars();
    accelerate();
    brake();
    move();
    second++;
    moved.clear();
}
