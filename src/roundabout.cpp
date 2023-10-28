#include "roundabout.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

#include "helpers.h"
#include "settings.h"

// private functions

std::string Roundabout::prepare_string() {
    std::string s, result = "\n";
    int l = 0;

    for (auto &lane : lanes) {
        s = "[l" + std::to_string(l++) + "] ";
        result += prepare_string_lane(lane, s);
    }
    for (auto &entry : entries) {
        s = "[ent" + std::to_string(entry.first) + "] ";
        result += prepare_string_lane(entry.second, s);
    }
    for (auto &exit : exits) {
        s = "[ext" + std::to_string(exit.first) + "] ";
        result += prepare_string_lane(exit.second, s);
    }
    return result;
}

void Roundabout::save() { history += prepare_string(); }

void Roundabout::delete_tails_ee(std::map<int, std::vector<Car *>> &e) {
    for (auto &pair : e) {
        for (auto &car : pair.second) {
            if (is_tail(car)) {
                delete car;
                car = nullptr;
            }
        }
    }
}

void Roundabout::delete_tails() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (is_tail(car)) {
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
            if (is_head(lane[idx])) {
                space = lane[idx]->get_space() - 1;
                tail_idx = idx - 1;
                while (space) {
                    if (tail_idx < 0) break;
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
    int tail_idx, space, curr_lane_starting_from, curr_lane;
    int outer_lane_idx = lanes.size() - 1;
    Car *car;

    for (int lane = 0; lane < (int)lanes.size(); lane++) {
        std::vector<Car *> &rbt_lane = lanes[lane];
        for (int idx = 0; idx < (int)rbt_lane.size(); idx++) {
            car = rbt_lane[idx];
            if (is_head(car)) {
                space = car->get_space() - 1;
                tail_idx = proper_idx(rbt_lane, idx - 1);
                // add tail on same lane
                while (space) {
                    curr_lane_starting_from = calculate_another_lane_idx(car->get_starting_from(), outer_lane_idx, lane, false);
                    if (tail_idx == proper_idx(rbt_lane, curr_lane_starting_from - 1))
                        break;
                    rbt_lane[tail_idx] = new Car(*car);
                    tail_idx = proper_idx(rbt_lane, tail_idx - 1);
                    space--;
                }
                curr_lane = lane;
                // add tail on lanes to entry
                while (space && curr_lane != outer_lane_idx) {
                    curr_lane++;
                    tail_idx = calculate_another_lane_idx(car->get_starting_from(), outer_lane_idx, curr_lane, false);
                    lanes[curr_lane][tail_idx] = new Car(*car);
                    space--;
                }
                // add tail on entry
                for (int t = 1; t <= space; t++) {
                    tail_idx = proper_idx(entries[car->get_starting_from()], -t);
                    entries[car->get_starting_from()][tail_idx] = new Car(*car);
                }
            }
        }
    }
    fix_tails_ee(entries);
    fix_tails_ee(exits);
}

// transition functions

void Roundabout::generate_cars() {
    int entry, v, space, destination;

    if (get_density() >= max_density) return;
    // commented loop is for trying until car is added
    // std::map<int, int> entries_chances_copy(entries_chances);

    // while (entries_chances_copy.size()) {
    entry = weighted_random_choice(entries_chances);
    if (!entries[entry][0]) {
        v = rand() % (max_velocity + 1);
        space = weighted_random_choice(cars_sizes);
        destination = weighted_random_choice(exits_chances);
        add_car(entry, v, space, destination);
        // break;
    }
    //     entries_chances_copy.erase(entry);
    // }
}

void Roundabout::accelerate_car(Car *car) {
    if (is_head(car) && car->get_v() < max_velocity) {
        car->set_v(car->get_v() + 1);
        car->set_v_used(0);
    }
}

void Roundabout::accelerate_ee(std::map<int, std::vector<Car *>> &e) {
    for (auto &lane : e) {
        for (auto &car : lane.second) accelerate_car(car);
    }
}

void Roundabout::accelerate() {
    for (auto &lane : lanes) {
        for (auto &car : lane) accelerate_car(car);
    }
    accelerate_ee(entries);
    accelerate_ee(exits);
}

int Roundabout::calculate_another_lane_idx(int car_idx, int current_lane, int destination_lane, bool forward) {
    if (current_lane == destination_lane) return car_idx;
    // forward is for returning index for changing lanes
    int lanes_difference = forward ? abs(destination_lane - current_lane) : 0;
    int new_idx = (int)round((double)lanes[destination_lane].size() / (double)lanes[current_lane].size() * ((double)car_idx + lanes_difference));
    return proper_idx(lanes[destination_lane], new_idx);
}

int Roundabout::change_lane_decision(int car_idx, int current_lane, int v, Car *car) {
    int no_lanes = (int)lanes.size();
    int outer_lane_idx = no_lanes - 1;

    int outer_pos = calculate_another_lane_idx(car_idx, current_lane, outer_lane_idx);
    int destination = car ? car->get_destination() : lanes[current_lane][car_idx]->get_destination();
    double l = proper_idx(lanes[outer_lane_idx], destination - outer_pos);

    int decision = (int)floor((l / (double)lanes[outer_lane_idx].size()) * (double)no_lanes);
    decision = outer_lane_idx - decision;
    // as changing lanes is dependant on velocity available
    if (abs(decision - current_lane) > v)
        decision = current_lane + (decision > current_lane ? v : -v);

    return decision;
}

void Roundabout::change_lanes() {
    int decision, d_to_prev, new_idx, outer_lane_idx, outer_pos;
    double l, wait_percent = 0.1;
    Car *car, *prev;
    std::set<Car *> changed;
    bool change;

    for (int lane = 0; lane < (int)lanes.size(); lane++) {
        for (int idx = 0; idx < (int)lanes[lane].size(); idx++) {
            car = lanes[lane][idx];
            if (is_head(car) && changed.find(car) == changed.end()) {
                decision = change_lane_decision(idx, lane, 1);
                if (decision != lane) {
                    new_idx = calculate_another_lane_idx(idx, lane, decision);
                    change = false;
                    while (1) {
                        if (lanes[decision][new_idx]) break;

                        d_to_prev = find_prev(lanes[decision], new_idx);
                        prev = lanes[decision][proper_idx(lanes[decision], new_idx - d_to_prev - 1)];
                        if (is_head(prev) && prev->get_v_available() > d_to_prev - (car->get_space() - 1)) break;
                        if (d_to_prev < car->get_space() - 1) break;
                        change = true;
                        break;
                    }
                    if (change) {
                        changed.insert(car);
                        car->set_v_used(car->get_v_used() + 1);
                        lanes[decision][new_idx] = car;
                        lanes[lane][idx] = nullptr;
                        delete_tails();
                        fix_tails();
                    } else {
                        outer_lane_idx = (int)lanes.size() - 1;
                        outer_pos = calculate_another_lane_idx(idx, lane, outer_lane_idx);
                        l = proper_idx(lanes[outer_lane_idx], car->get_destination() - outer_pos);
                        if (l < wait_percent * (outer_lane_idx - lane) * (int)lanes[outer_lane_idx].size()) {
                            car->set_v(0);
                            moved.insert(car);
                        }
                    }
                }
            }
        }
    }
}

void Roundabout::brake_ee(std::map<int, std::vector<Car *>> &e) {
    int d_to_next;
    Car *car;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (size_t idx = 0; idx < lane.size(); idx++) {
            car = lane[idx];
            if (is_head(car)) {
                d_to_next = find_next(lane, idx);
                // second check if not breaking at the end of the lane
                if (d_to_next < car->get_v_available() && d_to_next < (int)(lane.size() - (idx + 1)))
                    car->set_v(d_to_next + car->get_v_used());
            }
        }
    }
}

void Roundabout::brake() {
    int d_to_next;
    Car *car;

    for (auto &lane : lanes) {
        for (size_t idx = 0; idx < lane.size(); idx++) {
            car = lane[idx];
            if (is_head(car)) {
                d_to_next = find_next(lane, idx);
                if (d_to_next < car->get_v_available()) car->set_v(d_to_next + car->get_v_used());
            }
        }
    }
    brake_ee(entries);
    brake_ee(exits);
}

void Roundabout::enter() {
    int to_end, distance_to_prev, idx_prev, new_idx, decision, rbt_idx, v_needed;
    int outer_lane_idx = lanes.size() - 1;
    bool stop;
    Car *car;

    for (auto &pair : entries) {
        std::vector<Car *> &lane = pair.second;
        stop = false;
        for (int idx = lane.size() - 1; idx >= 0; idx--) {
            car = lane[idx];
            if (is_head(car) && idx != (int)lane.size() - 1) {
                to_end = lane.size() - 1 - idx;
                if (car->get_v() > to_end) car->set_v(to_end);
                break;
            }
            if (is_head(car)) {
                if (car->get_v() > TURN_VELOCITY) car->set_v(TURN_VELOCITY);
                // car can speed up to TURN_VELOCITY in 1s
                decision = change_lane_decision(pair.first, outer_lane_idx, TURN_VELOCITY - 1, car);

                // check if way to desired spot is free
                for (int current_lane = outer_lane_idx; current_lane >= decision; current_lane--) {
                    rbt_idx = calculate_another_lane_idx(pair.first, outer_lane_idx, current_lane, false);
                    // if way isn't blocked
                    if (lanes[current_lane][rbt_idx]) {
                        stop = true;
                        break;
                    }
                    // if car wouldn't hit me in the process
                    distance_to_prev = find_prev(lanes[current_lane], rbt_idx);
                    idx_prev = proper_idx(lanes[current_lane], rbt_idx - distance_to_prev - 1);
                    if (distance_to_prev != (int)lanes[current_lane].size() - 1 &&
                        lanes[current_lane][idx_prev]->get_v_available() >= distance_to_prev + 1) {
                        stop = true;
                        break;
                    }
                }
                if (stop) {
                    moved.insert(car);
                    break;
                }

                new_idx = calculate_another_lane_idx(pair.first, outer_lane_idx, decision, false);
                v_needed = outer_lane_idx - decision + 1;
                if (car->get_v() < v_needed) car->set_v(v_needed);  // accelerate to appropriate speed to enter
                car->set_v_used(v_needed);                          // entering uses one speed (action point)
                lanes[decision][new_idx] = car;
                lane[idx] = nullptr;

                // do tails correction
                delete_tails();
                fix_tails();
                break;
            }
        }
    }
}

void Roundabout::exit() {
    int d_to_next, to_entry;
    Car *car;

    std::vector<Car *> &rbt_lane = lanes[lanes.size() - 1];
    for (int idx = 0; idx < (int)rbt_lane.size(); idx++) {
        car = rbt_lane[idx];
        if (is_head(car) &&
            proper_idx(rbt_lane, car->get_destination() - idx) < car->get_v_available() &&
            moved.find(car) == moved.end()) {
            to_entry = proper_idx(rbt_lane, car->get_destination() - idx);
            car->set_v_used(car->get_v_used() + to_entry);

            // brake for next car on exit
            d_to_next = find_next(exits[car->get_destination()], -1);
            if (d_to_next < car->get_v_available()) {
                car->set_v(car->get_v_used() + d_to_next);
            }
            // brake to TURN_VELOCITY
            if (car->get_v_available() > TURN_VELOCITY) {
                car->set_v(car->get_v_used() + TURN_VELOCITY);
            }
            // exit is occupied
            if (d_to_next == 0) {
                rbt_lane[car->get_destination()] = car;
            } else {
                exits[car->get_destination()][car->get_v_available() - 1] = car;
            }
            // brake to TURN_VELOCITY
            if (car->get_v() > TURN_VELOCITY) car->set_v(TURN_VELOCITY);

            rbt_lane[idx] = nullptr;
            moved.insert(car);
        }
    }
    delete_tails();
    fix_tails();
}

void Roundabout::move_ee(std::map<int, std::vector<Car *>> &e) {
    int next_idx;
    Car *temp;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (size_t idx = 0; idx < lane.size(); idx++) {
            // check if car was moved before
            if (is_head(lane[idx]) && moved.find(lane[idx]) == moved.end()) {
                next_idx = idx + lane[idx]->get_v() - lane[idx]->get_v_used();

                if (e == exits && next_idx >= (int)lane.size()) {
                    capacity -= lane[idx]->get_space();
                    delete lane[idx];
                    lane[idx] = nullptr;
                } else {
                    temp = lane[idx];
                    lane[idx] = nullptr;
                    lane[next_idx] = temp;
                    moved.insert(lane[next_idx]);
                }
            }
        }
    }
}

void Roundabout::move() {
    int next_idx;
    Car *temp;

    delete_tails();
    for (auto &lane : lanes) {
        for (size_t idx = 0; idx < lane.size(); idx++) {
            // check if car was moved before
            if (is_head(lane[idx]) && moved.find(lane[idx]) == moved.end()) {
                next_idx = idx + lane[idx]->get_v_available();
                next_idx = proper_idx(lane, next_idx);

                temp = lane[idx];
                lane[idx] = nullptr;
                lane[next_idx] = temp;

                moved.insert(lane[next_idx]);
            }
        }
    }
    move_ee(entries);
    move_ee(exits);
    fix_tails();
}

void Roundabout::delete_cars(std::vector<Car *> &lane) {
    for (auto &car : lane) {
        if (car) delete car;
    }
}

// Public Functions

Roundabout::Roundabout(
    double island_radius,
    std::map<int, int> &entries,
    std::map<int, int> &exits,
    int number_of_lanes,
    int max_velocity,
    double max_density,
    int exits_entries_len) {
    // add to island radius half of raod width
    double radius = island_radius + ROAD_WIDTH / 2;
    int length;

    this->island_radius = island_radius;
    this->max_velocity = max_velocity;
    this->max_density = max_density;
    this->second = 0;
    this->capacity = 0;
    this->max_capacity = 0;
    this->cumulative_densities = 0.0;
    this->history = "";

    // setup info string
    this->info += "Seed: " + std::to_string(seed);
    this->info += "\nNo.lanes,No.entries,No.exits,Lanes lengths\n" +
                  std::to_string(number_of_lanes) + "," +
                  std::to_string(entries.size()) + "," +
                  std::to_string(exits.size());

    // calculate and initialize lanes
    // from circle circuit formula assign appropiate lengths
    for (int lane = 0; lane < number_of_lanes; lane++) {
        length = 2 * M_PI * (radius + ROAD_WIDTH * lane);
        this->lanes.push_back(std::vector<Car *>(length, nullptr));
        this->info += "," + std::to_string(length);
        this->max_capacity += length;
    }

    this->info += "\nRbt radius: " + std::to_string(island_radius);
    this->info += "\nMax velocity: " + std::to_string(max_velocity);
    this->info += "\nTurn velocity: " + std::to_string(TURN_VELOCITY);

    this->info += "\nCars sizes: ";
    for (auto car : cars_sizes) {
        this->info += std::to_string(car.first) + "(c):" + std::to_string(car.second) + "(w)\t";
    }

    this->info += "\nEntries:\t";
    // initialize entries
    for (auto &entry : entries) {
        this->entries[entry.first] = std::vector<Car *>(exits_entries_len, nullptr);
        this->entries_chances[entry.first] = entry.second;
        this->info += std::to_string(entry.first) + "(e):" + std::to_string(entry.second) + "(w)\t";
        this->max_capacity += exits_entries_len;
    }
    this->info += "\nExits:\t\t";
    // initialize exits
    for (auto &exit : exits) {
        this->exits[exit.first] = std::vector<Car *>(exits_entries_len, nullptr);
        this->exits_chances[exit.first] = exit.second;
        this->info += std::to_string(exit.first) + "(e):" + std::to_string(exit.second) + "(w)\t";
        this->max_capacity += exits_entries_len;
    }

    this->info += "\nMax capacity: " + std::to_string(max_capacity);
    this->info += "\nMax density: " + std::to_string(max_density) +
                  "(" + std::to_string((int)(max_capacity * (max_density / 100))) + ")\n";

    this->saving = false;
}

Roundabout::~Roundabout() {
    for (auto &lane : lanes) delete_cars(lane);
    for (auto &lane : entries) delete_cars(lane.second);
    for (auto &lane : exits) delete_cars(lane.second);
}

double Roundabout::get_density() {
    return ((double)capacity / (double)max_capacity) * 100;
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
    capacity += space;
}

void Roundabout::set_saving(bool save) { saving = save; }

void Roundabout::print() { std::cout << prepare_string(); }

void Roundabout::save_history() {
    double avg_density = cumulative_densities / second;
    info += "Average density: " + std::to_string(avg_density) + "\n";

    // create history folder
    std::string historyPath = "../history";
    if (!std::filesystem::exists(historyPath)) std::filesystem::create_directory(historyPath);

    // create seed folder
    std::string directoryPath = "../history/" + std::to_string(seed);
    if (!std::filesystem::exists(directoryPath)) std::filesystem::create_directory(directoryPath);

    std::string filePath = directoryPath + "/output.txt";
    std::ofstream history_file(filePath);
    history_file << info;
    history.pop_back();  // delete last \n
    history_file << history;
    history_file.close();
}

void Roundabout::space_time_diagram(int start, int no_steps) {
    std::cout << "Seed: " << seed << std::endl;
    simulate(start);
    set_saving(true);
    simulate(no_steps);

    std::cout << "Creating plots..." << std::endl;
    save_history();
    std::string python_script = "python3 space_time_diagram.py " + std::to_string(seed);
    system(python_script.c_str());
}

void Roundabout::simulate() {
    generate_cars();
    accelerate();
    change_lanes();
    brake();
    exit();
    enter();
    brake();
    move();

    moved.clear();
    if (saving) {
        cumulative_densities += get_density();
        second++;
        save();
    }
}

void Roundabout::simulate(int no_times) {
    for (int i = 0; i < no_times; i++) {
        simulate();
    }
}
