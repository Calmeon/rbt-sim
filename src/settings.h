#pragma once

#include <map>

#define ROAD_WIDTH 3.0
// key - size of car, value - rarity(0-1)
std::map<int, float> cars_sizes{{4, 0.05}, {6, 0.5}, {8, 0.45}, {10, 0.1}};
