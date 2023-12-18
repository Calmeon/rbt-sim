#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "car.h"

class Roundabout {
    double island_radius;             // roundabout island radius in m
    int max_velocity;                 // max velocity of agent
    double max_density;               // max car denisty on the road <0,100>
    bool only_rbt;                    // flag if density should count exits/entries
    int capacity;                     // current capacity of roundabout(with exits/entries)
    int capacity_rbt;                 // current capacity of roundabout
    int max_capacity;                 // max capacity of roundabout(with exits/entries)
    int max_capacity_rbt;             // max capacity of roundabout
    int second;                       // second of simulation (no. steps)
    double cumulative_densities;      // store densities for avg. calculation
    double cumulative_densities_rbt;  // store densities for avg. calculation (only rbt)
    int cars_left;                    // how much cars exited roundabout
    /*
     * 2D vector with rbt lanes
     * smaller idx = lane closer to the island
     * ex.
     * 0 - inside lane
     * 1 - middle lane
     * 2 - outer lane
     */
    std::vector<std::vector<Car *>> lanes;
    std::vector<std::vector<Car *>> lanes_next;

    // key - where connected, value - lane
    std::map<int, std::vector<Car *>> entries;
    std::map<int, std::vector<Car *>> entries_next;
    std::map<int, std::vector<Car *>> exits;
    std::map<int, std::vector<Car *>> exits_next;

    // key - where, value - chance(0-100)
    std::map<int, int> entries_chances;
    std::map<int, int> exits_chances;

    // which lanes can be accesed from entry
    std::map<int, std::vector<int>> entries_lanes;

    // vector with available actors
    std::vector<Agent *> agents;
    std::map<int, int> agents_chances;

    std::set<Car *> adjusted;  // helper set for cars with adjusted v

    bool saving;          // turns saving steps
    std::string info;     // string containing simulation info
    std::string history;  // big string containing simulation history

    // utility functions
    std::string prepare_string();
    void save();
    // functions with ee sufix are for exits/entries
    void delete_tails_ee(std::map<int, std::vector<Car *>> &e);
    void delete_tails();
    void clear_next();
    void fix_tails_ee(std::map<int, std::vector<Car *>> &e);
    void fix_tails();
    // transition functions
    void generate_cars();

    void save_velocities();
    void adjust_velocity_car(Car *car, int d_to_next, int next_car_v_old);
    void adjust_velocity_lane(std::vector<Car *> &lane, bool ee = false);
    void adjust_velocities_ee(std::map<int, std::vector<Car *>> &e);
    void adjust_velocities();

    int calculate_another_lane_idx(int car_idx, int current_lane, int destination_lane, bool forward = true);
    int change_lane_decision(int car_idx, int lane_number, int v, int destination, double change_bias = 0.0);
    void change_lanes();

    void enter();
    void exit();

    void move_ee(std::map<int, std::vector<Car *>> &e, bool exit = false);
    void move();

    void delete_cars(std::vector<Car *> &lane);
    void delete_all_cars();

   public:
    Roundabout(
        double island_radius,
        std::vector<std::vector<std::vector<int>>> &entries,
        std::vector<std::vector<int>> &exits,
        int number_of_lanes = 1,
        double max_density = 100.0,
        int exits_entries_len = 16);
    ~Roundabout();

    void set_max_density(double density);
    void set_only_rbt(bool only_rbt);
    std::string get_info();
    double get_density();
    double get_density_rbt();
    double get_flow();
    double get_avg_density();
    double get_avg_density_rbt();

    void add_car(int entry, int v, int space, int destination, Agent *agent);
    void add_agent(int max_v, int dr, int a_plus, int a_minus, int force_lane_change,
                   double change_bias, double wait_percent, int chance = 1);
    void set_saving(bool save);
    void print();
    void save_history();
    void space_time_diagram(int no_steps, bool only_rbt = false, int start = 200);
    void reset_rbt();
    // one step of simulation
    void simulate();
    // multiple steps of simulation
    void simulate(int no_times);
};
