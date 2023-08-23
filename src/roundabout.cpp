#include "roundabout.h"

#include <math.h>

#include <iostream>

#define ROAD_WIDTH 3.0

Rounbabout::Rounbabout(
    float island_radius,
    int number_of_lanes,
    int max_velocity,
    float density) {
    this->island_radius = island_radius;
    this->number_of_lanes = number_of_lanes;
    this->max_velocity = max_velocity;
    this->density = density;

    // calculate and initialize lanes
    // add to island radius half of raod width
    float radius = island_radius + ROAD_WIDTH / 2;
    int length;
    // from circle circuit formula assign appropiate lengths
    for (int lane = 0; lane < number_of_lanes; lane++) {
        length = 2 * M_PI * (radius + ROAD_WIDTH * lane);
        lanes.push_back(std::vector<Car *>(length, nullptr));
    }
}

Rounbabout::~Rounbabout() {}

void Rounbabout::print() {
    for (int lane = 0; lane < number_of_lanes; lane++) {
        std::cout << lane << " : " << lanes[lane].size() << std::endl;
    }
}
