#pragma once

#include <set>
#include <vector>

#include "car.h"

// corrects bad indexes (negative or beyond)
int proper_idx(int idx, int vector_size);

class Rounbabout {
    float island_radius;  // roundabout island radius in m
    int max_velocity;     // max velocity in m/s
    float density;        // car denisty on the road <0,1>
    int second;           // second of simulation (no. steps)
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
    std::set<Car *> moved;  // helper set

   public:
    Rounbabout(
        float island_radius,
        int number_of_lanes = 1,
        int max_velocity = 9,
        float density = 1);
    ~Rounbabout();

    void print();
    void add_car(int lane, int idx, int space = 3);

    void delete_tails();
    void fix_tails();
    // transition functions
    void accelerate();
    void move();
    // one step of simulation
    void simulate();
};
