#pragma once

#include <map>

#define DEBUG false

#define SEGMENT_LENGTH 0.625
#define ROAD_WIDTH 4.0

#define V_M 5
#define R_S 0.8

// key - size of car, value - rarity(0-100)
extern std::map<int, int> cars_sizes;

extern int seed;

void update_seed(int new_seed);
