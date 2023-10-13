#pragma once

#include <map>

#define ROAD_WIDTH 3.0
#define TURN_VELOCITY 9
#define SEED time(NULL)
// key - size of car, value - rarity(0-100)
extern std::map<int, int> cars_sizes;
