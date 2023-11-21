#pragma once

#include <map>
#include <string>
#include <vector>

#include "car.h"

bool is_head(Car *car);
bool is_tail(Car *car);
// corrects bad indexes (negative or beyond)
int proper_idx(std::vector<Car *> &lane, int idx);

// return distance to next car
int find_next(std::vector<Car *> &lane, int idx);

// return distance to previous car
int find_prev(std::vector<Car *> &lane, int idx);

// distance formulas
int d_brake(int v);
int d_f(int v);
int d_acc(int v, int v_next);
int d_keep(int v, int v_next);
int d_dec(int v, int v_next);

int weighted_random_choice(std::map<int, int> &dict);

std::string prepare_string_lane(std::vector<Car *> &lane, std::string s, int intend = 10);

std::string get_output_file_path();

void fundamental_diagram(double island_radius,
                         std::map<int, int> &entries, std::map<int, int> &exits,
                         int number_of_lanes, int max_velocity, int exits_entries_len, int samples = 3,
                         int step = 5, int from = 5, int to = 100);