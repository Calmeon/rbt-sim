#include "roundabout.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

#include "helpers.h"
#include "settings.h"

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
    this->cars_left = 0;
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

    this->info += "\nMax capacity: " + std::to_string(max_capacity) + "\n";

    this->saving = false;
}

Roundabout::~Roundabout() {
    for (auto &lane : lanes) delete_cars(lane);
    for (auto &lane : entries) delete_cars(lane.second);
    for (auto &lane : exits) delete_cars(lane.second);
}

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
    Car *car;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (size_t idx = 0; idx < lane.size(); idx++) {
            car = lane[idx];
            if (is_head(car)) {
                space = car->get_space() - 1;
                tail_idx = idx - 1;
                while (space) {
                    if (tail_idx < 0) break;
                    lane[tail_idx] = new Car(car);
                    space--;
                    tail_idx--;
                }

                if (space && e == exits) {
                    tail_idx = car->get_destination() - space + 1;
                    tail_idx = proper_idx(lanes[lanes.size() - 1], tail_idx);
                    while (space) {
                        lanes[lanes.size() - 1][tail_idx] = new Car(car);
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
                    rbt_lane[tail_idx] = new Car(car);
                    tail_idx = proper_idx(rbt_lane, tail_idx - 1);
                    space--;
                }
                curr_lane = lane;
                // add tail on lanes to entry
                while (space && curr_lane != outer_lane_idx) {
                    curr_lane++;
                    tail_idx = calculate_another_lane_idx(car->get_starting_from(), outer_lane_idx, curr_lane, false);
                    lanes[curr_lane][tail_idx] = new Car(car);
                    space--;
                }
                // add tail on entry
                for (int t = 1; t <= space; t++) {
                    tail_idx = proper_idx(entries[car->get_starting_from()], -t);
                    entries[car->get_starting_from()][tail_idx] = new Car(car);
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
    std::map<int, int> entries_chances_copy(entries_chances);

    while (entries_chances_copy.size()) {
        entry = weighted_random_choice(entries_chances);
        if (!entries[entry][0]) {
            v = rand() % (max_velocity + 1);
            space = weighted_random_choice(cars_sizes);
            destination = weighted_random_choice(exits_chances);
            add_car(entry, v, space, destination);
            break;
        }
        entries_chances_copy.erase(entry);
    }
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

    if ((int)lanes.size() < 1) return;

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
                        if (is_head(prev) && prev->get_v() > d_to_prev - (car->get_space() - 1)) break;
                        if (d_to_prev < car->get_space() - 1) break;
                        change = true;
                        break;
                    }
                    if (change) {
                        changed.insert(car);
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

void Roundabout::enter() {
    int to_end, distance_to_prev, idx_prev, new_idx, decision, rbt_idx;
    int no_lanes = (int)lanes.size();
    int outer_lane_idx = no_lanes - 1;
    bool stop;
    Car *car, *car_prev;

    for (auto &pair : entries) {
        std::vector<Car *> &lane = pair.second;
        stop = false;
        for (int idx = lane.size() - 1; idx >= 0; idx--) {
            car = lane[idx];
            if (is_tail(car)) break;

            // car needs to drive to end of entry to decide further
            if (is_head(car) && idx != (int)lane.size() - 1) {
                to_end = lane.size() - 1 - idx;
                if (car->get_v() > to_end) {
                    car->set_v(to_end);
                    lane[idx + car->get_v()] = car;
                    lane[idx] = nullptr;
                    moved.insert(car);
                    delete_tails();
                    fix_tails();
                }
                break;
            }
            if (is_head(car)) {
                // accelerate car
                car->set_v(car->get_v() + A_PLUS);
                // adjust speed to turn velocity
                if (car->get_v() > TURN_VELOCITY) car->set_v(TURN_VELOCITY);
                decision = change_lane_decision(pair.first, outer_lane_idx, no_lanes, car);
                // set speed so that car can get to desired lane
                if (no_lanes - decision > car->get_v()) car->set_v(no_lanes - decision);

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
                    car_prev = lanes[current_lane][idx_prev];
                    if (distance_to_prev != (int)lanes[current_lane].size() - 1 && is_head(car_prev) &&
                        car_prev->get_v() >= distance_to_prev + 1) {
                        stop = true;
                        break;
                    }
                }
                if (stop) {
                    car->set_v(0);
                    moved.insert(car);
                    break;
                }

                // move car onto roundabout
                new_idx = calculate_another_lane_idx(pair.first, outer_lane_idx, decision, false);
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
    int to_entry;
    Car *car;

    std::vector<Car *> &rbt_lane = lanes[lanes.size() - 1];
    for (int idx = 0; idx < (int)rbt_lane.size(); idx++) {
        car = rbt_lane[idx];

        // car is in front of exit
        if (is_head(car) && idx == car->get_destination() && car->get_v() > 0) {
            exits[car->get_destination()][car->get_v() - 1] = car;
            rbt_lane[idx] = nullptr;
            moved.insert(car);
            delete_tails();
            fix_tails();
        }
        // car needs to drive to exit and brake accordingly
        if (is_head(car) && proper_idx(rbt_lane, car->get_destination() - idx) < car->get_v()) {
            to_entry = proper_idx(rbt_lane, car->get_destination() - idx);
            car->set_v(to_entry);
        }
    }
    delete_tails();
    fix_tails();
}

void Roundabout::move_ee(std::map<int, std::vector<Car *>> &e) {
    int next_idx;
    Car *car;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (int idx = 0; idx < (int)lane.size(); idx++) {
            car = lane[idx];
            // check if car was moved before
            if (is_head(car) && !is_moved(car)) {
                next_idx = idx + car->get_v();

                if (e == exits && next_idx >= (int)lane.size()) {
                    capacity -= car->get_space();
                    if (saving) cars_left++;
                    delete car;
                    lane[idx] = nullptr;
                } else {
                    if (next_idx != idx) {
                        lane[next_idx] = car;
                        lane[idx] = nullptr;
                    }
                    moved.insert(car);
                }
            }
        }
    }
}

bool Roundabout::is_moved(Car *car) {
    return moved.find(car) != moved.end();
}

void Roundabout::move() {
    int next_idx;
    Car *temp;

    delete_tails();
    for (auto &lane : lanes) {
        for (int idx = 0; idx < (int)lane.size(); idx++) {
            // check if car was moved before
            if (is_head(lane[idx]) && !is_moved(lane[idx])) {
                next_idx = idx + lane[idx]->get_v();
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

std::string Roundabout::get_info() { return info; }

double Roundabout::get_density() {
    return ((double)capacity / (double)max_capacity) * 100;
}

double Roundabout::get_flow() {
    return ((double)cars_left / (double)second) * 3600.0;
}

double Roundabout::get_avg_density() {
    return cumulative_densities / second;
}

void Roundabout::add_car_rbt(int lane, int idx, int space) {
    Car *head = new Car(0, space, -1, -1);
    Car *tail;
    int tail_idx;

    idx = proper_idx(lanes[lane], idx);
    lanes[lane][idx] = head;

    for (int i = 1; i < space; i++) {
        tail = new Car(head);
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
    info += "Max density: " + std::to_string(max_density) +
            "(" + std::to_string((int)(max_capacity * (max_density / 100))) + ")\n";
    info += "Average density: " + std::to_string(get_avg_density()) + "\n";
    info += "Cars left: " + std::to_string(get_flow()) + "\n";

    std::ofstream history_file(get_output_file_path());
    history_file << info;
    history.pop_back();  // delete last \n
    history_file << history;
    history_file.close();
}

void Roundabout::space_time_diagram(int start, int no_steps) {
    simulate(start);
    set_saving(true);
    simulate(no_steps);

    save_history();
    std::string python_script = "python3 space_time_diagram.py " + std::to_string(seed);
    std::cout << "Creating diagrams..." << std::endl;
    system(python_script.c_str());
}

void Roundabout::adjust_velocity(std::vector<Car *> &lane, bool ee, bool entry, bool outer_lane) {
    int car_d_acc, car_d_keep, car_d_dec;
    int d_to_next, idx_next, next_car_v_old, to_end;
    int v_new;
    Car *car, *next_car;

    for (int idx = 0; idx < (int)lane.size(); idx++) {
        car = lane[idx];
        if (is_head(car) && !is_moved(car)) {
            // caluculate distances
            d_to_next = find_next(lane, idx);
            if (!ee && idx == car->get_destination() && outer_lane) {
                // car is standing in front of the exit
                d_to_next = find_next(exits[car->get_destination()], -1);
                idx_next = proper_idx(exits[car->get_destination()], d_to_next);
                next_car = exits[car->get_destination()][idx_next];
            } else if (ee && d_to_next >= (int)lane.size() - (idx + 1)) {
                // if it is end of the ee lane just set max velocity to not brake
                d_to_next = max_velocity;
                if (entry) {
                    to_end = (int)(lane.size() - (idx + 1));
                    d_to_next = to_end;
                }
                next_car = nullptr;
            } else {
                idx_next = proper_idx(lane, idx + d_to_next + 1);
                next_car = lane[idx_next];
            }
            // if next car doesn't exist set max_velocity as speed
            next_car_v_old = next_car ? next_car->get_v_old() : max_velocity;

            car_d_acc = d_acc(car->get_v_old(), next_car_v_old);
            car_d_keep = d_keep(car->get_v_old(), next_car_v_old);
            car_d_dec = d_dec(car->get_v_old(), next_car_v_old);

            v_new = car->get_v();
            // accelerate
            if (d_to_next >= car_d_acc)
                v_new = std::min(v_new + A_PLUS, max_velocity);
            // random accelerate
            if ((d_to_next >= car_d_keep && d_to_next < car_d_acc) ||
                v_new == max_velocity) {
                if (((double)rand() / (RAND_MAX)) < R_S) {
                    v_new = std::max(v_new + A_MINUS, 0);
                }
            }
            // brake
            if (d_to_next >= car_d_dec && d_to_next < car_d_keep)
                v_new = std::max(v_new + A_MINUS, 0);
            // emergancy brake
            if (d_to_next < car_d_dec)
                v_new = std::max(v_new - V_M, 0);

            car->set_v(v_new);

            // adjust car velocity on exit
            if (!ee && idx == car->get_destination() && car->get_v() > TURN_VELOCITY)
                car->set_v(TURN_VELOCITY);
        }
    }
}

void Roundabout::adjust_velocities_ee(std::map<int, std::vector<Car *>> &e) {
    bool entry = e == entries ? true : false;
    for (auto &pair : e)
        adjust_velocity(pair.second, true, entry);
}

void Roundabout::adjust_velocities() {
    bool outer_lane;
    for (int l = 0; l < (int)lanes.size(); l++) {
        outer_lane = l == (int)lanes.size() - 1 ? true : false;
        adjust_velocity(lanes[l], false, false, outer_lane);
    }
    adjust_velocities_ee(exits);
    adjust_velocities_ee(entries);
}

void Roundabout::save_velocities() {
    for (auto &lane : lanes) {
        for (auto &car : lane)
            if (is_head(car)) car->save_v_old();
    }
    for (auto &lane : entries) {
        for (auto &car : lane.second)
            if (is_head(car)) car->save_v_old();
    }
    for (auto &lane : exits) {
        for (auto &car : lane.second)
            if (is_head(car)) car->save_v_old();
    }
}

void Roundabout::simulate() {
    generate_cars();
    change_lanes();
    enter();
    save_velocities();
    adjust_velocities();
    exit();
    move();

    moved.clear();
    if (saving) {
        cumulative_densities += get_density();
        second++;
        save();
    }
}

void Roundabout::simulate(int no_times) {
    for (int i = 0; i < no_times; i++)
        simulate();
}
