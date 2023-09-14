#pragma once

#include <map>

#define DEBUG false

#define ROAD_WIDTH 3.0
#define TURN_VELOCITY 3
#define SEED time(NULL)
// key - size of car, value - rarity(0-1)
extern std::map<int, float> cars_sizes;
