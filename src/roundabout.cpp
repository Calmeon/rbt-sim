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
    std::vector<std::vector<std::vector<int>>> &entries,
    std::vector<std::vector<int>> &exits,
    int number_of_lanes,
    double max_density,
    int exits_entries_len) {
    // add to island radius half of raod width
    double radius = island_radius + ROAD_WIDTH / 2;
    double length;
    int no_cells;

    this->island_radius = island_radius;
    this->max_density = max_density;
    this->second = 0;
    this->capacity = 0;
    this->capacity_rbt = 0;
    this->max_capacity = 0;
    this->max_capacity_rbt = 0;
    this->cumulative_densities = 0.0;
    this->cumulative_densities_rbt = 0.0;
    this->cars_left = 0;
    this->history = "";
    this->only_rbt = true;

    // setup info string
    this->info += "Seed: " + std::to_string(seed) +
                  "\nNo.lanes,No.entries,No.exits,Lanes lengths\n" +
                  std::to_string(number_of_lanes) + "," +
                  std::to_string(entries.size()) + "," +
                  std::to_string(exits.size());

    // calculate and initialize lanes
    // from circle circuit formula assign appropiate lengths
    for (int lane = 0; lane < number_of_lanes; lane++) {
        length = 2 * M_PI * (radius + ROAD_WIDTH * (double)lane);
        no_cells = (int)floor(length / SEGMENT_LENGTH);
        this->lanes.push_back(std::vector<Car *>(no_cells, nullptr));
        this->lanes_next.push_back(std::vector<Car *>(no_cells, nullptr));
        this->info += "," + std::to_string(no_cells);
        this->max_capacity += no_cells;
        this->max_capacity_rbt += no_cells;
    }

    this->info += "\nRbt radius: " + std::to_string(island_radius) +
                  "\nMax braking velocity: " + std::to_string(V_M) +
                  "\nCars sizes: ";
    for (auto car : cars_sizes) {
        this->info += std::to_string(car.first) + "(c):" + std::to_string(car.second) + "(w)\t";
    }

    this->info += "\nEntries:\t";
    // initialize entries
    int entry_cell;
    for (auto &entry : entries) {
        auto entry_params = entry[0];
        entry_cell = round(entry_params[0] / SEGMENT_LENGTH);
        this->entries_lanes[entry_cell] = entry[1];
        this->entries[entry_cell] = std::vector<Car *>(exits_entries_len, nullptr);
        this->entries_next[entry_cell] = std::vector<Car *>(exits_entries_len, nullptr);
        this->entries_chances[entry_cell] = entry_params[1];
        this->info += std::to_string(entry_cell) + "(e):" + std::to_string(entry_params[1]) + "(w)\t";
        this->max_capacity += exits_entries_len;
    }
    this->info += "\nExits:\t\t";
    // initialize exits
    int exit_cell;
    for (auto &exit : exits) {
        exit_cell = round(exit[0] / SEGMENT_LENGTH);
        this->exits[exit_cell] = std::vector<Car *>(exits_entries_len, nullptr);
        this->exits_next[exit_cell] = std::vector<Car *>(exits_entries_len, nullptr);
        this->exits_chances[exit_cell] = exit[1];
        this->info += std::to_string(exit_cell) + "(e):" + std::to_string(exit[1]) + "(w)\t";
        this->max_capacity += exits_entries_len;
    }

    this->info += "\nMax capacity: " + std::to_string(max_capacity) + "\n";
    this->saving = false;
}

Roundabout::~Roundabout() {
    delete_all_cars();
    for (auto agent : agents) delete agent;
}

// private functions

std::string Roundabout::prepare_string() {
    std::string s, result = "\n";
    int l = 0;

    if (DEBUG) {
        result += "second: " + std::to_string(second) + "\n";
    }
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

void Roundabout::clear_next() {
    for (auto &lane : lanes_next) {
        for (auto &car : lane) {
            car = nullptr;
        }
    }
    for (auto &exit : exits_next) {
        for (auto &car : exit.second) {
            car = nullptr;
        }
    }
    for (auto &entry : entries_next) {
        for (auto &car : entry.second) {
            car = nullptr;
        }
    }
}

void Roundabout::fix_tails_ee(std::map<int, std::vector<Car *>> &e) {
    int tail_idx, space;
    Car *car;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (int idx = 0; idx < (int)lane.size(); idx++) {
            car = lane[idx];
            if (is_head(car)) {
                space = car->get_space() - 1;
                tail_idx = idx - 1;
                while (space) {
                    if (tail_idx < 0) break;
                    if (lane[tail_idx]) {
                        history += "==========ERROR==========\n";
                        print_error("fix_tails_ee", "exit/entry", pair.first, tail_idx, second);
                    }
                    lane[tail_idx] = new Car(car, car->get_space() - space + 1);
                    space--;
                    tail_idx--;
                }

                if (space && e == exits) {
                    if (car->get_exited_from() != (int)lanes.size() - 1) {
                        lanes[lanes.size() - 1][car->get_destination()] = new Car(car, car->get_space() - space + 1);
                        space--;
                    }
                    tail_idx = calculate_another_lane_idx(car->get_destination(), lanes.size() - 1, car->get_exited_from());
                    while (space) {
                        if (is_head(lanes[car->get_exited_from()][tail_idx])) {
                            history += "==========ERROR==========\n";
                            print_error("fix_tails_ee", "exit", pair.first, tail_idx, second);
                        } else {
                            delete lanes[car->get_exited_from()][tail_idx];
                        }
                        lanes[car->get_exited_from()][tail_idx] = new Car(car, car->get_space() - space + 1);
                        tail_idx = proper_idx(lanes[car->get_exited_from()], --tail_idx);
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
                    curr_lane_starting_from = calculate_another_lane_idx(car->get_starting_from(), outer_lane_idx, lane);
                    if (tail_idx == proper_idx(rbt_lane, curr_lane_starting_from - 1))
                        break;
                    if (rbt_lane[tail_idx]) {
                        history += "==========ERROR==========\n";
                        print_error("fix_tails", "lane", lane, tail_idx, second);
                    }
                    rbt_lane[tail_idx] = new Car(car, car->get_space() - space + 1);
                    tail_idx = proper_idx(rbt_lane, tail_idx - 1);
                    space--;
                }
                curr_lane = lane;
                // add tail on lanes to entry
                while (space && curr_lane != outer_lane_idx) {
                    curr_lane++;
                    tail_idx = calculate_another_lane_idx(car->get_starting_from(), outer_lane_idx, curr_lane);
                    if (lanes[curr_lane][tail_idx]) {
                        history += "==========ERROR==========\n";
                        print_error("fix_tails", "lane", curr_lane, tail_idx, second);
                    }
                    lanes[curr_lane][tail_idx] = new Car(car, car->get_space() - space + 1);
                    space--;
                }
                // add tail on entry
                for (int t = 1; t <= space; t++) {
                    tail_idx = proper_idx(entries[car->get_starting_from()], -t);
                    if (entries[car->get_starting_from()][tail_idx]) {
                        history += "==========ERROR==========\n";
                        print_error("fix_tails", "entry", car->get_starting_from(), tail_idx, second);
                        std::cout << "Starting: " << car->get_starting_from() << " Dest: " << car->get_destination() << "\n";
                    }
                    entries[car->get_starting_from()][tail_idx] = new Car(car, car->get_space() - space + 1);
                }
            }
        }
    }
    fix_tails_ee(entries);
    fix_tails_ee(exits);
}

// transition functions

void Roundabout::generate_cars() {
    int entry, v, space, destination, lane_decision, agent_idx;
    int no_lanes = lanes.size();
    int outer_lane_idx = no_lanes - 1;
    double given_density = only_rbt ? get_density_rbt() : get_density();
    if (given_density >= max_density) return;
    // loop is for trying until car is added
    std::map<int, int> entries_chances_copy(entries_chances);
    std::vector<int> available_lanes;
    std::vector<int>::const_iterator it;

    while (entries_chances_copy.size()) {
        entry = weighted_random_choice(entries_chances_copy);
        if (!entries[entry][entries[entry].size() - 1]) {
            agent_idx = weighted_random_choice(agents_chances);
            destination = weighted_random_choice(exits_chances);
            lane_decision = change_lane_decision(entry, outer_lane_idx, no_lanes, destination, agents[agent_idx]->get_change_bias());

            available_lanes = entries_lanes[entry];
            it = std::find(available_lanes.begin(), available_lanes.end(), lane_decision);
            if (it != available_lanes.end()) {
                space = weighted_random_choice(cars_sizes);
                v = rand() % (V_M + 1);
                add_car(entry, v, space, destination, agents[agent_idx]);
                // break; // with it every entry is filled every time
            }
        }
        entries_chances_copy.erase(entry);
    }
}

int Roundabout::calculate_another_lane_idx(int car_idx, int current_lane, int destination_lane) {
    if (current_lane == destination_lane) return car_idx;
    int new_idx = (int)round(((double)lanes[destination_lane].size() / (double)lanes[current_lane].size()) * (double)car_idx);
    return proper_idx(lanes[destination_lane], new_idx);
}

int Roundabout::change_lane_decision(int car_idx, int current_lane, int v, int destination, double change_bias) {
    int no_lanes = (int)lanes.size();
    int outer_lane_idx = no_lanes - 1;
    int outer_pos = calculate_another_lane_idx(car_idx, current_lane, outer_lane_idx);

    double d_to_exit = proper_idx(lanes[outer_lane_idx], destination - outer_pos);
    int decision = (double)outer_lane_idx -
                   std::min(
                       std::max(floor((d_to_exit / (double)lanes[outer_lane_idx].size() + change_bias) * (double)no_lanes),
                                0.0),
                       (double)outer_lane_idx);
    // as changing lanes is dependant on velocity available
    if (abs(decision - current_lane) > v) {
        decision = current_lane + (decision > current_lane ? v : -v);
    }

    return decision;
}

void Roundabout::change_lanes() {
    int decision, new_idx, outer_pos, d_to_exit, car_d_keep;
    int d_to_prev, prev_d_dec;
    int next_car_idx, d_to_next, v_next;
    int idx_giving_way, d_to_giving_way;
    int outer_lane_number = lanes.size() - 1;
    Car *car, *prev_car, *next_car, *car_giving_way;
    std::set<Car *> changed;
    bool change;

    for (int lane = 0; lane < (int)lanes.size() - 1; lane++) {
        for (int idx = 0; idx < (int)lanes[lane].size(); idx++) {
            car = lanes[lane][idx];
            if (is_head(car) && !contains(changed, car)) {
                decision = change_lane_decision(idx, lane, 1, car->get_destination(), car->get_change_bias());
                if (decision > lane) {
                    new_idx = calculate_another_lane_idx(idx, lane, decision);
                    change = false;

                    while (true) {
                        // if field occupied
                        if (lanes[decision][new_idx]) break;

                        // not enough space
                        d_to_prev = find_prev(lanes[decision], new_idx);
                        if (d_to_prev < car->get_space() - 1) break;

                        d_to_next = find_next(lanes[decision], new_idx);
                        next_car_idx = proper_idx(lanes[decision], new_idx + d_to_next + 1);
                        next_car = lanes[decision][next_car_idx];
                        v_next = car->get_max_v();
                        if (next_car) {
                            d_to_next -= (next_car->get_space() - next_car->get_tail_number());
                            v_next = next_car->get_v_old();
                        }
                        // incentive criterion if I can brake to next car in time
                        car_d_keep = d_keep(car->get_v_old(), car->get_g(), v_next);
                        if (d_to_next < car_d_keep) break;

                        // safety criterion if car behind me can brake in time
                        prev_car = lanes[decision][proper_idx(lanes[decision], new_idx - d_to_prev - 1)];
                        d_to_prev -= (car->get_space() - 1);
                        if (is_head(prev_car)) {
                            prev_d_dec = d_dec(prev_car->get_v_old(), prev_car->get_a_minus(), prev_car->get_g(), car->get_v_old());
                            if (d_to_prev < prev_d_dec) break;
                        }

                        change = true;
                        break;
                    }
                    if (change) {
                        car->set_waiting(0);
                        changed.insert(car);
                        delete_tails();
                        lanes[decision][new_idx] = car;
                        lanes[lane][idx] = nullptr;
                        fix_tails();

                        // outer_pos = calculate_another_lane_idx(idx, decision, outer_lane_number);
                        // d_to_exit = proper_idx(lanes[outer_lane_number], car->get_destination() - outer_pos);
                        // if (decision < outer_lane_idx - 1 &&
                        //     d_to_exit < car->get_wait_percent() * (outer_lane_idx - decision) * (int)lanes[outer_lane_idx].size()) {
                        //     car->set_v(std::max(car->get_v_old() - V_M, 0));
                        //     car->save_v_old();
                        //     adjusted.insert(car);
                        // }
                    } else if (lane < outer_lane_number - 1) {
                        outer_pos = calculate_another_lane_idx(idx, lane, outer_lane_number);
                        d_to_exit = proper_idx(lanes[outer_lane_number], car->get_destination() - outer_pos);
                        if (d_to_exit < car->get_wait_percent() * (outer_lane_number - lane) * (int)lanes[outer_lane_number].size()) {
                            car->set_waiting(car->get_waiting() + 1);
                            car->set_v(std::max(car->get_v_old() - V_M, 0));
                            car->save_v_old();
                            adjusted.insert(car);
                        }
                        // if (car->get_waiting() > car->get_force_lane_change()) {
                        //     idx_giving_way = proper_idx(lanes[decision], new_idx - car->get_space());
                        //     d_to_giving_way = find_prev(lanes[decision], idx_giving_way);
                        //     idx_giving_way = proper_idx(lanes[decision], idx_giving_way - d_to_giving_way - 1);
                        //     car_giving_way = lanes[decision][idx_giving_way];
                        //     if (is_head(car_giving_way)) {
                        //         // car_giving_way->set_v(std::max(0, car_giving_way->get_v() + car_giving_way->get_a_minus()));
                        //         // adjusted.insert(car_giving_way);
                        //         // car->get_v() - V_M
                        //         car_giving_way->set_v(std::max(car->get_v_old() - V_M, 0));
                        //         car->save_v_old();
                        //         adjusted.insert(car_giving_way);
                        //     }
                        // }
                        // outer_lane_idx = (int)lanes.size() - 1;
                        // outer_pos = calculate_another_lane_idx(idx, lane, outer_lane_idx);
                        // d_to_exit = proper_idx(lanes[outer_lane_idx], car->get_destination() - outer_pos);
                        // if (d_to_exit < car->get_wait_percent() * (outer_lane_idx - lane) * (int)lanes[outer_lane_idx].size()) {
                        //     car->set_waiting(car->get_waiting() + 1);
                        //     car->set_v(std::max(car->get_v_old() - V_M, 0));
                        //     car->save_v_old();
                        //     adjusted.insert(car);
                        // }
                    }
                }
            }
        }
    }
}

void Roundabout::enter() {
    bool stop;
    Car *car, *next_car, *prev_car;
    int to_end, car_entry, decision, rbt_idx, d_to_exit;
    int d_to_next, idx_next, next_car_v_old;
    int d_to_prev, idx_prev;
    int d_to_prev_from_entry, d_to_prev_from_his_exit;
    int no_lanes = (int)lanes.size();
    int outer_lane_idx = no_lanes - 1;

    for (auto &pair : entries) {
        std::vector<Car *> &lane = pair.second;
        stop = false;

        for (int idx = lane.size() - 1; idx >= 0; idx--) {
            car = lane[idx];
            if (is_tail(car)) break;

            if (is_head(car)) {
                to_end = (int)lane.size() - (idx + 1);
                car_entry = car->get_starting_from();
                decision = change_lane_decision(car_entry, outer_lane_idx, no_lanes, car->get_destination(), car->get_change_bias());
                rbt_idx = calculate_another_lane_idx(car_entry, outer_lane_idx, decision);

                if (!lanes[decision][rbt_idx]) {
                    // assume next car on decision lane to adjust velocity to check further
                    d_to_next = find_next(lanes[decision], rbt_idx - 1);
                    idx_next = proper_idx(lanes[decision], rbt_idx + d_to_next);
                    next_car = lanes[decision][idx_next];

                    next_car_v_old = car->get_max_v();
                    // check if car would exit immediately
                    if (decision == outer_lane_idx) {
                        d_to_exit = proper_idx(lanes[decision], car->get_destination() - (car_entry - 1));
                        if (d_to_exit <= d_to_next) {
                            d_to_next = d_to_exit + find_next(exits[car->get_destination()], -1);
                            next_car = exits[car->get_destination()][find_next(exits[car->get_destination()], -1)];
                        }
                    }

                    if (next_car) {
                        next_car_v_old = next_car->get_v_old();
                        d_to_next -= (next_car->get_space() - next_car->get_tail_number());
                    }
                    d_to_next += to_end;

                    adjust_velocity_car(car, d_to_next, next_car_v_old);
                    if (car->get_v() <= to_end) stop = true;
                } else {
                    stop = true;
                }

                // check if way to desired spot is free
                for (int current_lane = outer_lane_idx; current_lane >= decision && !stop; current_lane--) {
                    rbt_idx = calculate_another_lane_idx(car_entry, outer_lane_idx, current_lane);
                    // if way isn't blocked
                    if (lanes[current_lane][rbt_idx]) {
                        stop = true;
                        break;
                    }

                    // if car wouldn't hit me in the process
                    d_to_prev = find_prev(lanes[current_lane], rbt_idx);
                    idx_prev = proper_idx(lanes[current_lane], rbt_idx - d_to_prev - 1);
                    prev_car = lanes[current_lane][idx_prev];

                    if (d_to_prev != (int)lanes[current_lane].size() - 1 && is_head(prev_car)) {
                        // safety criterion
                        d_to_prev -= (car->get_space() - 1);
                        if (d_to_prev < d_dec(prev_car->get_v_old(), prev_car->get_a_minus(), prev_car->get_g(), car->get_v() - to_end - 1)) {
                            if (current_lane == outer_lane_idx) {
                                // blinker
                                d_to_prev_from_entry = proper_idx(lanes[current_lane], car_entry - idx_prev);
                                d_to_prev_from_his_exit = proper_idx(lanes[current_lane], prev_car->get_destination() - idx_prev);
                                if (d_to_prev_from_entry < d_to_prev_from_his_exit) {
                                    stop = true;
                                    break;
                                }
                            } else {
                                stop = true;
                                break;
                            }
                        }
                    }
                }

                if (stop) {
                    // If car can't enter assume next car to be end of entry
                    d_to_next = to_end;
                    next_car_v_old = 0;
                    adjust_velocity_car(car, d_to_next, next_car_v_old);
                    car->save_v_old();
                }

                if (car->get_v() > to_end) {
                    rbt_idx = calculate_another_lane_idx(car_entry, outer_lane_idx, decision);
                    delete_tails();
                    lanes[decision][rbt_idx] = car;
                    lane[idx] = nullptr;
                    car->set_v_used(to_end + 1);
                    capacity_rbt += car->get_space();
                    car->set_v_old(car->get_v_available());
                    fix_tails();
                }

                adjusted.insert(car);
                break;
            }
        }
    }
}

void Roundabout::exit() {
    int d_to_exit, d_to_next, next_v, idx_next, new_v;
    int d_to_prev, idx_prev;
    int destination;
    Car *car, *next_car, *prev_car;
    bool stop;
    int outer_lane_number = lanes.size() - 1;

    // exit only from 2 most outer lanes
    for (int lane_idx = outer_lane_number - 1; lane_idx <= outer_lane_number; lane_idx++) {
        std::vector<Car *> &lane = lanes[lane_idx];
        for (int idx = 0; idx < (int)lane.size(); idx++) {
            car = lane[idx];
            if (is_head(car) && !contains(adjusted, car)) {
                destination = calculate_another_lane_idx(car->get_destination(), outer_lane_number, lane_idx);
                d_to_exit = proper_idx(lane, destination - idx);
                d_to_next = find_next(lane, idx);
                // get next car and adjust v to it
                idx_next = proper_idx(lane, idx + d_to_next + 1);
                next_car = lane[idx_next];
                d_to_next -= (next_car->get_space() - next_car->get_tail_number());

                next_v = next_car->get_v_old();
                adjust_velocity_car(car, d_to_next, next_v);

                new_v = car->get_v();
                car->set_v(car->get_v_old());

                stop = true;
                if (d_to_exit <= d_to_next || new_v > d_to_exit) {
                    while (true) {
                        if (exits[car->get_destination()][0]) {
                            // exit occupied
                            break;
                        }

                        d_to_next = d_to_exit + find_next(exits[car->get_destination()], -1);
                        next_car = exits[car->get_destination()][find_next(exits[car->get_destination()], -1)];
                        next_v = next_car ? next_car->get_v_old() : car->get_max_v();

                        adjust_velocity_car(car, d_to_next, next_v);

                        if (car->get_v() <= d_to_exit && lane_idx == outer_lane_number - 1) {
                            break;
                        }

                        if (lane_idx == outer_lane_number - 1 && car->get_v() > d_to_exit) {
                            destination = car->get_destination();
                            if (lanes[outer_lane_number][destination]) {
                                // way to exit is blocked
                                break;
                            }

                            d_to_prev = find_prev(lanes[outer_lane_number], destination);
                            idx_prev = proper_idx(lanes[outer_lane_number], destination - d_to_prev - 1);
                            prev_car = lanes[outer_lane_number][idx_prev];

                            if (d_to_prev != (int)lanes[outer_lane_number].size() - 1 && is_head(prev_car)) {
                                d_to_prev -= (car->get_space() - 2);
                                if (d_to_prev < d_dec(prev_car->get_v_old(), prev_car->get_a_minus(), prev_car->get_g(), car->get_v() - d_to_exit - 1)) {
                                    break;
                                }
                            }
                        }
                        stop = false;
                        break;
                    }
                    if (stop) {
                        adjust_velocity_car(car, d_to_exit, 0);
                        car->save_v_old();
                    } else if (car->get_v() > d_to_exit) {
                        delete_tails();
                        exits[car->get_destination()][0] = car;
                        car->set_v_used(d_to_exit + 1);
                        car->set_v_old(car->get_v_available());
                        car->set_exited_from(lane_idx);
                        lane[idx] = nullptr;
                        fix_tails();
                        capacity_rbt -= car->get_space();
                    }
                    adjusted.insert(car);
                } else if (car->get_max_v() * 2 > d_to_exit) {
                    adjust_velocity_car(car, d_to_exit, 0);
                    car->set_v(std::min(car->get_v(), new_v));
                    adjusted.insert(car);
                }
            }
        }
    }
}

void Roundabout::move_ee(std::map<int, std::vector<Car *>> &e, bool exit) {
    int new_idx;
    Car *car;

    for (auto &pair : e) {
        std::vector<Car *> &lane = pair.second;
        for (int idx = 0; idx < (int)lane.size(); idx++) {
            car = lane[idx];

            if (is_head(car)) {
                new_idx = idx + car->get_v_available();
                car->set_v_used(0);
                if (exit && new_idx >= (int)lane.size()) {
                    // delete car beyond end of exit
                    capacity -= car->get_space();
                    if (saving) cars_left++;
                    delete car;
                } else if (exit) {
                    // normal move on exit
                    if (exits_next[pair.first][new_idx]) {
                        history += "==========ERROR==========\n";
                        print_error("move_ee", "exit", pair.first, new_idx, second);
                    }
                    exits_next[pair.first][new_idx] = car;
                } else {
                    // normal move on entry
                    if (entries_next[pair.first][new_idx]) {
                        history += "==========ERROR==========\n";
                        print_error("move_ee", "entry", pair.first, new_idx, second);
                    }
                    entries_next[pair.first][new_idx] = car;
                }
            }
        }
    }
}

void Roundabout::move() {
    int new_idx, d_to_exit;
    Car *car;
    int no_lanes = lanes.size();

    delete_tails();
    for (int lane_idx = 0; lane_idx < no_lanes; lane_idx++) {
        std::vector<Car *> &lane = lanes[lane_idx];
        for (int idx = 0; idx < (int)lane.size(); idx++) {
            car = lane[idx];

            if (is_head(car)) {
                d_to_exit = proper_idx(lane, car->get_destination() - idx);
                if (lane_idx == no_lanes - 1 && car->get_v_available() > d_to_exit) {
                    // move to exit
                    new_idx = car->get_v_available() - d_to_exit - 1;
                    if (exits_next[car->get_destination()][new_idx]) {
                        history += "==========ERROR==========\n";
                        print_error("move", "exit", car->get_destination(), new_idx, second);
                    }
                    exits_next[car->get_destination()][new_idx] = car;
                    car->set_exited_from(lane_idx);
                    capacity_rbt -= car->get_space();
                } else {
                    // move on lane
                    new_idx = idx + car->get_v_available();
                    new_idx = proper_idx(lane, new_idx);
                    if (lanes_next[lane_idx][new_idx]) {
                        history += "==========ERROR==========\n";
                        print_error("move", "lane", lane_idx, new_idx, second);
                    }
                    lanes_next[lane_idx][new_idx] = car;
                }
                car->set_v_used(0);
            }
        }
    }
    move_ee(entries);
    move_ee(exits, true);
    lanes = lanes_next;
    exits = exits_next;
    entries = entries_next;
    clear_next();
    fix_tails();
}

void Roundabout::delete_cars(std::vector<Car *> &lane) {
    for (auto &car : lane) {
        if (car) {
            delete car;
            car = nullptr;
        }
    }
}

void Roundabout::delete_all_cars() {
    for (auto &lane : lanes) delete_cars(lane);
    for (auto &lane : entries) delete_cars(lane.second);
    for (auto &lane : exits) delete_cars(lane.second);
}

// Public Functions

void Roundabout::set_max_density(double density) {
    max_density = density;
}

void Roundabout::set_only_rbt(bool only_rbt) {
    this->only_rbt = only_rbt;
}

std::string Roundabout::get_info() { return info; }

double Roundabout::get_density() {
    return ((double)capacity / (double)max_capacity) * 100;
}

double Roundabout::get_density_rbt() {
    return ((double)capacity_rbt / (double)max_capacity_rbt) * 100;
}

double Roundabout::get_flow() {
    return ((double)cars_left / (double)second) * 3600.0;
}

double Roundabout::get_avg_density() {
    return cumulative_densities / second;
}

double Roundabout::get_avg_density_rbt() {
    return cumulative_densities_rbt / second;
}

void Roundabout::add_car(int entry, int v, int space, int destination, Agent *agent) {
    entries[entry][entries[entry].size() - 1] = new Car(v, space, destination, entry, agent);
    capacity += space;
}

void Roundabout::add_agent(int max_v, int dr, int a_plus, int a_minus, int force_lane_change,
                           double change_bias, double wait_percent, int chance) {
    agents.push_back(new Agent(max_v, dr, a_plus, a_minus, force_lane_change, change_bias, wait_percent));
    agents_chances[agents.size() - 1] = chance;
    info += "(max_v, dr, a_plus, a_minus, force_lane_change, change_bias, wait_percent)=(" +
            std::to_string(agents[0]->get_max_v()) + ", " +
            std::to_string(agents[0]->get_dr()) + ", " +
            std::to_string(agents[0]->get_a_plus()) + ", " +
            std::to_string(agents[0]->get_a_minus()) + ", " +
            std::to_string(agents[0]->get_force_lane_change()) + ", " +
            std::to_string(agents[0]->get_change_bias()) + ", " +
            std::to_string(agents[0]->get_wait_percent()) + ")\n";
}

void Roundabout::set_saving(bool save) {
    saving = save;
    second = 0;
}

void Roundabout::print() { std::cout << prepare_string(); }

void Roundabout::save_history() {
    int used_capacity = only_rbt ? max_capacity_rbt : max_capacity;
    info += "Max density: " + std::to_string(max_density) +
            "(" + std::to_string((int)(used_capacity * (max_density / 100))) + ")\n";
    info += "Average density: " + std::to_string(get_avg_density()) + " | " +
            std::to_string(get_avg_density_rbt()) + "\n";
    info += "Flow veh/h: " + std::to_string(cars_left) + "\n";

    std::ofstream history_file(get_output_file_path());
    history_file << info;
    history.pop_back();  // delete last \n
    history_file << history;
    history_file.close();
}

void Roundabout::space_time_diagram(int no_steps, int start, bool only_rbt) {
    this->only_rbt = only_rbt;
    simulate(start);
    set_saving(true);
    simulate(no_steps);

    save_history();
    std::string python_script = "python3 diagrams/space_time_diagram.py " + std::to_string(seed);
    std::cout << "Creating diagrams...\n";
    system(python_script.c_str());
}

void Roundabout::reset_rbt() {
    delete_all_cars();
    second = 0;
    capacity = 0;
    capacity_rbt = 0;
    cumulative_densities = 0.0;
    cumulative_densities_rbt = 0.0;
    cars_left = 0;
    history = "";
    saving = false;
}

void Roundabout::adjust_velocity_car(Car *car, int d_to_next, int next_car_v_old) {
    double car_d_acc = d_acc(car->get_v_old(), car->get_a_plus(), car->get_g(), next_car_v_old);
    double car_d_keep = d_keep(car->get_v_old(), car->get_g(), next_car_v_old);
    double car_d_dec = d_dec(car->get_v_old(), car->get_a_minus(), car->get_g(), next_car_v_old);

    int v_new = car->get_v_old();
    int vn = car->get_v_old();

    // accelerate
    if (d_to_next >= car_d_acc) {
        v_new = std::min(vn + car->get_a_plus(), car->get_max_v());
    }

    // random deaccelerate
    if ((d_to_next >= car_d_keep && d_to_next < car_d_acc) || vn == car->get_max_v()) {
        if (((double)rand() / (RAND_MAX)) < R_S) {
            v_new = std::max(vn + car->get_a_minus(), 0);
        }
    }

    // brake
    if (d_to_next >= car_d_dec && d_to_next < car_d_keep) {
        v_new = std::max(vn + car->get_a_minus(), 0);
    }

    // emergancy brake
    if (d_to_next < car_d_dec) {
        v_new = std::max(vn - V_M, 0);
    }

    car->set_v(v_new);
}

void Roundabout::adjust_velocity_lane(std::vector<Car *> &lane, bool ee) {
    int d_to_next, idx_next, next_car_v_old;
    Car *car, *next_car;
    int to_end;

    for (int idx = lane.size() - 1; idx >= 0; idx--) {
        car = lane[idx];
        if (is_head(car) && !contains(adjusted, car)) {
            d_to_next = find_next(lane, idx);
            to_end = (int)lane.size() - (idx + 1);

            if (ee && d_to_next >= to_end) {
                // If it is last car on exit set params so it wouldn't slow down
                d_to_next = car->get_max_v();
                next_car = nullptr;
            } else {
                idx_next = proper_idx(lane, idx + d_to_next + 1);
                next_car = lane[idx_next];
            }

            if (next_car) {
                d_to_next -= (next_car->get_space() - next_car->get_tail_number());
                next_car_v_old = next_car->get_v_old();
            } else {
                next_car_v_old = car->get_max_v();
            }

            adjust_velocity_car(car, d_to_next, next_car_v_old);

            // Check if next car is only tail or whole vehicle
            if (next_car && !ee) {
                Car *next_head = next_car->get_head();
                for (int idx_succ = 1; idx_succ <= car->get_max_v() * 2; idx_succ++) {
                    int idx_tmp = proper_idx(lane, idx + idx_succ);
                    if (is_head(lane[idx_tmp]) && lane[idx_tmp] != next_head) {
                        int v_before = car->get_v();
                        adjust_velocity_car(car, d_to_next, lane[idx_tmp]->get_v_old());
                        car->set_v(std::min(v_before, car->get_v()));
                    }
                }
            }

            // Check if after next car is end of the entry
            if (next_car && ee) {
                for (int idx_succ = idx + 1; idx_succ < (int)lane.size(); idx_succ++) {
                    int idx_tmp = proper_idx(lane, idx_succ);
                    if (is_head(lane[idx_tmp])) break;
                    if (idx_succ == (int)lane.size() - 1) {
                        int v_before = car->get_v();
                        adjust_velocity_car(car, d_to_next, 0);
                        car->set_v(std::min(v_before, car->get_v()));
                    }
                }
            }
            adjusted.insert(car);
        }
    }
}

void Roundabout::adjust_velocities_ee(std::map<int, std::vector<Car *>> &e) {
    for (auto &pair : e) adjust_velocity_lane(pair.second, true);
}

void Roundabout::adjust_velocities() {
    for (int l = 0; l < (int)lanes.size(); l++) {
        adjust_velocity_lane(lanes[l]);
    }
    adjust_velocities_ee(entries);
    adjust_velocities_ee(exits);
}

void Roundabout::save_velocities() {
    for (auto &lane : lanes) {
        for (auto &car : lane) {
            if (is_head(car)) car->save_v_old();
        }
    }
    for (auto &lane : entries) {
        for (auto &car : lane.second) {
            if (is_head(car)) car->save_v_old();
        }
    }
    for (auto &lane : exits) {
        for (auto &car : lane.second) {
            if (is_head(car)) car->save_v_old();
        }
    }
}

void Roundabout::simulate() {
    generate_cars();

    save_velocities();
    change_lanes();
    if (DEBUG && saving) {
        history += "\nAfter changing";
        save();
    }
    enter();
    if (DEBUG && saving) {
        history += "\nAfter Enter";
        save();
    }
    exit();
    if (DEBUG && saving) {
        history += "\nAfter Exit";
        save();
    }
    adjust_velocities();
    if (DEBUG && saving) {
        history += "\nAfter velocities adjustment";
        save();
    }
    move();
    if (DEBUG && saving) {
        history += "\nAfter move";
        save();
    }

    adjusted.clear();
    second++;
    if (saving) {
        cumulative_densities += get_density();
        cumulative_densities_rbt += get_density_rbt();
        save();
    }
}

void Roundabout::simulate(int no_times) {
    for (int i = 0; i < no_times; i++)
        simulate();
}
