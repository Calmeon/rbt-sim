#pragma once

#include <map>

#define DEBUG false

#define SEGMENT_LENGTH 0.625
#define ROAD_WIDTH 3.75

#define MAX_V 14 // 31.5 km/h = 8.75 m/s / SEGMENT_LENGTH(=0.625) = 14
#define V_M 8   // 5 m/s^2 / SEGMENT_LENGTH(=0.625) = 8 
#define R_S 0.8

// key - size of car, value - rarity(0-100)
extern std::map<int, int> cars_sizes;

extern int seed;

void update_seed(int new_seed);
