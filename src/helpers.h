#pragma once

#include <map>
#include <vector>

#include "car.h"

// corrects bad indexes (negative or beyond)
int proper_idx(std::vector<Car *> &lane, int idx);

// return distance to next car
int find_next(std::vector<Car *> &lane, int idx);

// return distance to previous car
int find_prev(std::vector<Car *> &lane, int idx);

int weighted_random_choice(std::map<int, float> &dict);