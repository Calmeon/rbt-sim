#pragma once

#include <map>

#define ROAD_WIDTH 3.0
#define TURN_VELOCITY 4
extern int seed;
// key - size of car, value - rarity(0-100)
extern std::map<int, int> cars_sizes;