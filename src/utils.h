#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "car.h"
#include "roundabout.h"

bool is_head(Car *car);
bool is_tail(Car *car);

bool contains(std::set<Car *> &set, Car *car);

// corrects bad indexes (negative or beyond)
int proper_idx(std::vector<Car *> &lane, int idx);
// return distance to next car
int find_next(std::vector<Car *> &lane, int idx);
// return distance to previous car
int find_prev(std::vector<Car *> &lane, int idx);

// distance formulas
int d_brake(int v);
int d_f(int v);
double d_acc(int v, int a_plus, double g, int v_next);
double d_keep(int v, double g, int v_next);
double d_dec(int v, int a_minus, double g, int v_next);

int weighted_random_choice(std::map<int, int> &dict);

std::string prepare_string_lane(std::vector<Car *> &lane,
                                std::string s, int intend = 10);

std::string get_output_file_path(std::string filename = "output");

void prepare_fundamental(Roundabout &rbt, int samples, int step, int from,
                         int to, std::string title = "output");
void fundamental_diagram();

void print_error(std::string function, std::string lane_type,
                 int lane_number, int idx, int second);
