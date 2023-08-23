#pragma once

#include <vector>

#include "car.h"

class Rounbabout {
    float island_radius;  // roundabout island radius in m
    int number_of_lanes;  // > 0
    int max_velocity;  // max velocity in m/s
    float density;     // car denisty on the road <0,1>
    // 2D vector with rbt lanes
    // smaller idx = lane more to the middle
    // ex. 0 - island lane
    //     1 - middle lane
    /*
     * 2D vector with rbt lanes
     * smaller idx = lane closer to the island
     * ex. 0 - inside lane
     *     1 - middle lane
     *     2 - outer lane
     */
    std::vector<std::vector<Car *>> lanes;

   public:
    Rounbabout(
        float island_radius,
        int number_of_lanes = 1,
        int max_velocity = 9,
        float density = 1);
    ~Rounbabout();

    void print();
};
