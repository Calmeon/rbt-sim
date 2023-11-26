#include <iostream>
#include <random>

#include "helpers.h"
#include "roundabout.h"
#include "settings.h"

int main() {
    srand(seed);
    std::cout << "Seed: " << seed << std::endl;

    // std::vector<std::vector<std::vector<int>>> entries{{{3, 3}, {2, 1, 0}},
    //                                                    {{59, 5}, {2, 1, 0}},
    //                                                    {{115, 5}, {2, 1, 0}},
    //                                                    {{171, 6}, {2, 1, 0}}};
    std::vector<std::vector<std::vector<int>>> entries{{{3, 3}, {0}},
                                                       {{6, 3}, {1}},
                                                       {{9, 3}, {2}},
                                                       {{59, 5}, {0}},
                                                       {{62, 5}, {1}},
                                                       {{65, 5}, {2}},
                                                       {{115, 5}, {0}},
                                                       {{118, 5}, {1}},
                                                       {{121, 5}, {2}},
                                                       {{171, 6}, {0}},
                                                       {{173, 6}, {1}},
                                                       {{176, 6}, {2}}};
    std::vector<std::vector<int>> exits{{0, 10},
                                        {56, 15},
                                        {112, 15},
                                        {168, 15}};

    Roundabout rbt(28, entries, exits, 3);
    rbt.add_agent(9, 1, 2, -2, 3, 0.0, 0.05, 50);
    rbt.add_agent(10, 0, 3, -3, 3, 0.0, 0.05, 25);
    rbt.set_max_density(100.0);

    bool only_rbt = false;
    // rbt.space_time_diagram(200, only_rbt);
    fundamental_diagram(rbt, 3, 1, only_rbt);

    return 0;
}
