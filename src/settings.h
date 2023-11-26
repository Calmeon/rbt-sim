#pragma once

#include <map>

extern int seed;

#define ROAD_WIDTH 3.0

#define TURN_VELOCITY 4
#define V_M 10
#define R_S 0.8

// key - size of car, value - rarity(0-100)
extern std::map<int, int> cars_sizes;