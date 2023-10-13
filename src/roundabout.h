#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "car.h"

class Roundabout {
    float island_radius;  // roundabout island radius in m
    int max_velocity;     // max velocity in m/s
    float density;        // car denisty on the road <0,1>
    int second;           // second of simulation (no. steps)
    /*
     * 2D vector with rbt lanes
     * smaller idx = lane closer to the island
     * ex. 0 - inside lane
     *     1 - middle lane
     *     2 - outer lane
     */
    std::vector<std::vector<Car *>> lanes;

    // key - where connected, value - lane
    std::map<int, std::vector<Car *>> entries;
    std::map<int, std::vector<Car *>> exits;

    // key - where, value - chance(0-100)
    std::map<int, int> entries_chances;
    std::map<int, int> exits_chances;

    std::set<Car *> moved;  // helper set

    bool saving;          // turns saving steps
    std::string history;  // big string containing simulation history

    // utility functions
    std::string prepare_string();
    void save();
    // functions with ee sufix are for exits/entries
    void delete_tails_ee(std::map<int, std::vector<Car *>> &e);
    void delete_tails();
    void fix_tails_ee(std::map<int, std::vector<Car *>> &e);
    void fix_tails();
    // transition functions
    void generate_cars();
    void accelerate_ee(std::map<int, std::vector<Car *>> &e);
    void accelerate();
    int calculate_another_lane_idx(int car_idx, int current_lane, int destination_lane);
    int change_lane_decision(int car_idx, int lane_number);
    void change_lanes();
    void brake_ee(std::map<int, std::vector<Car *>> &e);
    void brake();
    void enter();
    void exit();
    void move_ee(std::map<int, std::vector<Car *>> &e);
    void move();

   public:
    Roundabout(
        float island_radius,
        std::map<int, float> &entries,
        std::map<int, float> &exits,
        int number_of_lanes = 1,
        int max_velocity = 9,
        float density = 1,
        int exits_entries_len = 50);
    ~Roundabout();

    void add_car_rbt(int lane, int idx, int space = 3);
    void add_car(int entry, int v, int space, int destination);
    void set_saving(bool save);
    void print();
    void save_history();
    void plot();
    // one step of simulation
    void simulate();
    // multiple steps of simulation
    void simulate(int no_times);
};
