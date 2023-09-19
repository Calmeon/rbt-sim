#include "roundabout.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

#include "helpers.h"
#include "settings.h"

// private functions

std::string Roundabout::prepare_string() {
    std::string result = "\n";
    int l = 0;

    for (auto &lane : lanes) {
        result += "[l" + std::to_string(l++) + "] ";
        for (auto &car : lane) {
            if (car == nullptr) {
                result += ".";
            } else if (car->get_is_tail()) {
                result += ">";
            } else {
                result += std::to_string(car->get_v());
            }
        }
        result += "\n";
    }
    if (DEBUG) {
        result += "[db] ";
        for (size_t i = 0; i < lanes[lanes.size() - 1].size(); i++) {
            result += std::to_string(i % 10);
        }
        result += "\n";
    }

    for (auto &entry : entries) {
        result += "[ent" + std::to_string(entry.first) + "] ";
        for (auto &car : entry.second) {
            if (car == nullptr) {
                result += ".";
            } else if (car->get_is_tail()) {
                result += ">";
            } else {
                result += std::to_string(car->get_v());
            }
        }
        result += "\n";
    }

    for (auto &exit : exits) {
        result += "[ext" + std::to_string(exit.first) + "] ";
        for (auto &car : exit.second) {
            if (car == nullptr) {
                result += ".";
            } else if (car->get_is_tail()) {
                result += ">";
            } else {
                result += std::to_string(car->get_v());
            }
        }
        result += "\n";
    }

    return result;
}

void Roundabout::save() {
    history += prepare_string();
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
    int tail_idx, space;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (size_t idx = 0; idx < lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                space = lane[idx]->get_space() - 1;
                tail_idx = idx - 1;
                while (space) {
                    if (tail_idx < 0) {
                        break;
                    }
                    lane[tail_idx] = new Car(*lane[idx]);
                    space--;
                    tail_idx--;
                }

                if (space && e == exits) {
                    tail_idx = lane[idx]->get_destination() - space + 1;
                    tail_idx = proper_idx(lanes[lanes.size() - 1], tail_idx);
                    while (space) {
                        lanes[lanes.size() - 1][tail_idx] = new Car(*lane[idx]);
                        tail_idx = proper_idx(lanes[lanes.size() - 1], ++tail_idx);
                        space--;
                    }
                }
            }
        }
    }
}

void Roundabout::fix_tails() {
    int tail_idx, space;

    for (auto &lane : lanes) {
        for (size_t idx = 0; idx < lane.size(); idx++) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                space = lane[idx]->get_space() - 1;
                tail_idx = proper_idx(lane, idx - 1);
                while (space) {
                    if (tail_idx == proper_idx(lane, lane[idx]->get_starting_from() - 1)) {
                        break;
                    }
                    lane[tail_idx] = new Car(*lane[idx]);
                    tail_idx = proper_idx(lane, tail_idx - 1);
                    space--;
                }

                for (int t = 1; t <= space; t++) {
                    entries[lane[idx]->get_starting_from()][proper_idx(entries[lane[idx]->get_starting_from()], -t)] = new Car(*lane[idx]);
                }
            }
        }
    }
    fix_tails_ee(entries);
    fix_tails_ee(exits);
}

// transition functions

void Roundabout::generate_cars() {
    int v, space, destination;
    for (auto &entry : entries) {
        if ((float)rand() / (float)RAND_MAX > entries_chances[entry.first] &&
            entry.second[0] == nullptr) {
            v = rand() % (max_velocity + 1);
            space = weighted_random_choice(cars_sizes);
            destination = weighted_random_choice(exits_chances);
            add_car(entry.first, v, space, destination);
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
                // second check if not breaking at the end of the lane
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

void Roundabout::enter() {
    int to_end, distance_to_prev, idx_prev, velocity_left, distance_to_next, new_idx;

    for (auto &pair : entries) {
        std::vector<Car *> &lane = pair.second;
        for (int idx = lane.size() - 1; idx >= 0; idx--) {
            if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
                to_end = lane.size() - 1 - idx;
                // if its velocity is lesser than length of entry line
                if (lane[idx]->get_v() <= to_end) {
                    break;
                }
                std::vector<Car *> &rbt_lane = lanes[lanes.size() - 1];
                // if some car is blocking entry then brake
                if (rbt_lane[pair.first] != nullptr) {
                    lane[idx]->set_v(to_end);
                    break;
                }
                // get distance to prev car on rbt and its velocity
                distance_to_prev = find_prev(rbt_lane, proper_idx(rbt_lane, pair.first + 1));
                idx_prev = proper_idx(rbt_lane, pair.first - distance_to_prev);
                if (distance_to_prev != ((int)rbt_lane.size() - 1) && rbt_lane[idx_prev]->get_v() >= distance_to_prev) {
                    lane[idx]->set_v(to_end);
                    break;
                }

                velocity_left = lane[idx]->get_v() - to_end;
                // adjust velocity to turn manuver
                if (velocity_left > TURN_VELOCITY) {
                    velocity_left = TURN_VELOCITY;
                }
                if (lane[idx]->get_v() > TURN_VELOCITY) {
                    lane[idx]->set_v(TURN_VELOCITY);
                }
                // adjust velocity to next car
                distance_to_next = find_next(rbt_lane, pair.first - 1);
                if (distance_to_next < velocity_left) {
                    lane[idx]->set_v(distance_to_next + to_end);
                    velocity_left = distance_to_next;
                }

                new_idx = lane[idx]->get_starting_from() + velocity_left - 1;
                new_idx = proper_idx(rbt_lane, new_idx);
                rbt_lane[new_idx] = lane[idx];
                lane[idx] = nullptr;
                moved.insert(rbt_lane[new_idx]);
                // do tails correction
                delete_tails();
                fix_tails();

                break;
            }
        }
    }
}

void Roundabout::exit() {
    // consider doing smth in enter to prevent from missing exit
    /*
     * v = 9 v_left = 9
     * drive to entry v = 9 v_left = 9-to_entry ex. 5
     * check if v is appropriate for turn(turnv = 3)
     * v = 3 v_left = 3
     * check if needs to brake to next (ex. d = 2)
     * v = 2 v_left = 2
     */
    int velocity_left, d_to_next, to_entry;

    std::vector<Car *> &rbt_lane = lanes[lanes.size() - 1];
    for (int idx = 0; idx < (int)rbt_lane.size(); idx++) {
        Car *car = rbt_lane[idx];
        if (car != nullptr && !car->get_is_tail() &&
            proper_idx(rbt_lane, car->get_destination() - idx) < car->get_v() &&
            moved.find(car) == moved.end()) {
            to_entry = proper_idx(rbt_lane, car->get_destination() - idx);
            velocity_left = car->get_v() - to_entry;

            // brake to match turn velocity
            if (velocity_left > TURN_VELOCITY) {
                velocity_left = TURN_VELOCITY;
            }
            if (car->get_v() > TURN_VELOCITY) {
                car->set_v(TURN_VELOCITY);
            }
            // brake for next car on exit
            d_to_next = find_next(exits[car->get_destination()], -1);
            if (d_to_next < velocity_left) {
                car->set_v(d_to_next + to_entry);
                velocity_left = d_to_next;
            }
            // exit is occupied
            if (velocity_left == 0) {
                rbt_lane[car->get_destination()] = car;
            } else {
                exits[car->get_destination()][velocity_left - 1] = car;
            }
            rbt_lane[idx] = nullptr;

            moved.insert(car);
        }
    }

    delete_tails();
    fix_tails();
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

                    if (e == exits && next_idx >= (int)lane.size()) {
                        delete lane[idx];
                        lane[idx] = nullptr;
                    } else {
                        Car *temp = lane[idx];
                        lane[idx] = nullptr;
                        lane[next_idx] = temp;
                        moved.insert(lane[next_idx]);
                    }
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

                    Car *temp = lane[idx];
                    lane[idx] = nullptr;
                    lane[next_idx] = temp;

                    moved.insert(lane[next_idx]);
                }
            }
        }
    }
    move_ee(entries);
    move_ee(exits);
    fix_tails();
}

// Public Functions

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
    // setup history string
    this->history += "No. lanes,\tNo. entries,\tNo.exits\n" +
                     std::to_string(number_of_lanes) + "," +
                     std::to_string(entries.size()) + "," +
                     std::to_string(exits.size()) + "\n";
    this->saving = false;
}

Roundabout::~Roundabout() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (car != nullptr) {
                delete car;
            }
        }
    }

    for (auto &lane : entries) {
        for (auto &car : lane.second) {
            if (car != nullptr) {
                delete car;
            }
        }
    }

    for (auto &lane : exits) {
        for (auto &car : lane.second) {
            if (car != nullptr) {
                delete car;
            }
        }
    }
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
    entries[entry][0] = new Car(v, space, destination, entry);
}

void Roundabout::set_saving(bool save) {
    saving = save;
}

void Roundabout::print() {
    std::cout << prepare_string();
}

void Roundabout::save_history() {
    // create history folder
    std::string historyPath = "../history";
    if (!std::filesystem::exists(historyPath)) {
        std::filesystem::create_directory(historyPath);
    }
    // create seed folder
    std::string directoryPath = "../history/" + std::to_string(SEED);
    if (!std::filesystem::exists(directoryPath)) {
        std::filesystem::create_directory(directoryPath);
    }

    std::string filePath = directoryPath + "/output.txt";
    std::ofstream history_file(filePath);
    history.pop_back();  // delete last \n
    history_file << history;
    history_file.close();
}

void Roundabout::plot() {
    std::string python_script = "python3 plot.py " + std::to_string(SEED);
    system(python_script.c_str());
}

void Roundabout::simulate() {
    if (saving) save();

    generate_cars();
    accelerate();
    brake();
    exit();
    enter();
    move();

    second++;
    moved.clear();
}

void Roundabout::simulate(int no_times) {
    for (int i = 0; i < no_times; i++) {
        simulate();
    }
}
