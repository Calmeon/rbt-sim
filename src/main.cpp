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

    // std::vector<std::vector<std::vector<int>>> entries{{{3, 3}, {2, 1, 0}},
    //                                                    {{59, 5}, {2, 1, 0}},
    //                                                    {{115, 5}, {2, 1, 0}},
    //                                                    {{171, 6}, {2, 1, 0}}};
    // std::vector<std::vector<std::vector<int>>> entries{{{3, 3}, {0}},
    //                                                    {{6, 3}, {1}},
    //                                                    {{9, 3}, {2}},
    //                                                    {{59, 5}, {0}},
    //                                                    {{62, 5}, {1}},
    //                                                    {{65, 5}, {2}},
    //                                                    {{115, 5}, {0}},
    //                                                    {{118, 5}, {1}},
    //                                                    {{121, 5}, {2}},
    //                                                    {{171, 6}, {0}},
    //                                                    {{173, 6}, {1}},
    //                                                    {{176, 6}, {2}}};
    // std::vector<std::vector<int>> exits{{0, 10},
    //                                     {56, 15},
    //                                     {112, 15},
    //                                     {168, 15}};

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
                                        {226, 14}};

    Roundabout rbt(28.5, entries, exits, 3, 100.0, 30);
    rbt.add_agent(9, 3, 2, -2, 3, 0.0, 0.1);
    // rbt.add_agent(10, 0, 3, -3, 3, 0.0, 0.05);

    rbt.set_max_density(50.0);

    bool only_rbt = true;
    rbt.space_time_diagram(200, only_rbt);
    // fundamental_diagram(rbt, 10, 1, only_rbt, 5, 60);
    return 0;
}
