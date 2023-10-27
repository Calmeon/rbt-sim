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

int weighted_random_choice(std::map<int, int> &dict);

std::string prepare_string_lane(std::vector<Car *> &lane, std::string s, int intend = 10);
