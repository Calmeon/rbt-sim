#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "car.h"

class Roundabout {
    double island_radius;         // roundabout island radius in m
    int max_velocity;             // max velocity in m/s
    double max_density;           // max car denisty on the road <0,100>
    int capacity;                 // current capacity of roundabout
    int max_capacity;             // max capacity of roundabout(with exits/entries)
    int second;                   // second of simulation (no. steps)
    double cumulative_densities;  // store densities for avg. calculation
    int cars_left;                // how much cars exited roundabout
    /*
     * 2D vector with rbt lanes
     * smaller idx = lane closer to the island
     * ex.
     * 0 - inside lane
     * 1 - middle lane
     * 2 - outer lane
     */
    std::vector<std::vector<Car *>> lanes;

    // key - where connected, value - lane
    std::map<int, std::vector<Car *>> entries;
    std::map<int, std::vector<Car *>> exits;

    // key - where, value - chance(0-100)
    std::map<int, int> entries_chances;
    std::map<int, int> exits_chances;

    // which lanes can be accesed from entry
    std::map<int, std::vector<int>> entries_lanes;

    std::set<Car *> moved;     // helper set for moved cars
    std::set<Car *> adjusted;  // helper set for cars with adjusted v

    bool saving;          // turns saving steps
    std::string info;     // string containing simulation info
    std::string history;  // big string containing simulation history

    // utility functions
    std::string prepare_string();
    void save();
    bool is_moved(Car *car);
    // functions with ee sufix are for exits/entries
    void delete_tails_ee(std::map<int, std::vector<Car *>> &e);
    void delete_tails();
    void fix_tails_ee(std::map<int, std::vector<Car *>> &e);
    void fix_tails();
    // transition functions
    void generate_cars();

    void save_velocities();
    void adjust_velocity(std::vector<Car *> &lane, bool ee, bool entry, bool outer_lane = -1);
    void adjust_velocities_ee(std::map<int, std::vector<Car *>> &e);
    void adjust_velocities();

    int calculate_another_lane_idx(int car_idx, int current_lane, int destination_lane, bool forward = true);
    int change_lane_decision(int car_idx, int lane_number, int v, Car *car = nullptr, int dest = -1);
    void change_lanes();

    void enter();
    void exit();

    void move_ee(std::map<int, std::vector<Car *>> &e);
    void move();

    void delete_cars(std::vector<Car *> &lane);

   public:
    Roundabout(
        double island_radius,
        std::vector<std::vector<std::vector<int>>> &entries,
        std::vector<std::vector<int>> &exits,
        int number_of_lanes = 1,
        int max_velocity = 9,
        double max_density = 100.0,
        int exits_entries_len = 50);
    ~Roundabout();

    std::string get_info();
    double get_density();
    double get_flow();
    double get_avg_density();
    void add_car_rbt(int lane, int idx, int space = 3);
    void add_car(int entry, int v, int space, int destination);
    void set_saving(bool save);
    void print();
    void save_history();
    void space_time_diagram(int start, int no_steps);
    // one step of simulation
    void simulate();
    // multiple steps of simulation
    void simulate(int no_times);
};
