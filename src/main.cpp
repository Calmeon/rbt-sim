#include <iostream>
#include <random>

#include "helpers.h"
#include "roundabout.h"
#include "settings.h"

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        int given_seed = atoi(argv[1]);
        srand(given_seed);
        update_seed(given_seed);
    } else {
        srand(seed);
    }
    std::cout << "Seed: " << seed << std::endl;

    std::vector<std::vector<std::vector<int>>> entries{{{0, 9}, {0, 1}},
                                                       {{4, 11}, {2}},

                                                       {{28, 5}, {0, 1, 2}},

                                                       {{36, 2}, {0, 1, 2}},

                                                       {{82, 9}, {0, 1}},
                                                       {{90, 11}, {2}},

                                                       {{116, 6}, {0, 1}},
                                                       {{122, 9}, {2}},

                                                       {{156, 9}, {0}},
                                                       {{160, 11}, {1}},
                                                       {{165, 13}, {2}},

                                                       {{204, 5}, {0, 1, 2}}};
    std::vector<std::vector<int>> exits{{19, 6},

                                        {59, 11},
                                        {66, 9},

                                        {107, 4},

                                        {130, 10},
                                        {133, 13},
                                        {139, 12},

                                        {192, 5},

                                        {221, 16},
                                        {226, 14}
                                        };

    Roundabout rbt(28.5, entries, exits, 3, 31.0);
    rbt.add_agent(5, 1, 2, -2, 3, 0.0, 0.1);
    rbt.space_time_diagram(3600);
    // prepare_fundamental(rbt, 3, 1,  0, 25);
    // fundamental_diagram();

    // Roundabout
    // double radius = 28.5;
    // int no_lanes = 3;

    // // Agent
    // int max_velocity = 9;
    // // int dr = 0;
    // int a_plus = 2;
    // int a_minus = -2;
    // int force_lane_change = 9999;
    // double change_bias = 0.0;
    // double wait_percent = 0.1;
    // std::vector<int> drs{0, 1, 2, 3, 5};

    // // Simulations
    // int samples = 3;
    // int step = 1;
    // int from = 2;
    // int to = 50;

    // for (auto dr : drs) {
    //     Roundabout rbt(radius, entries, exits, no_lanes);
    //     rbt.add_agent(max_velocity, dr, a_plus, a_minus, force_lane_change, change_bias, wait_percent);
    //     prepare_fundamental(rbt, samples, step, from, to, "dr=" + std::to_string(dr));
    // }
    // fundamental_diagram();

    return 0;
}
