#include <iostream>
#include <random>

#include "helpers.h"
#include "roundabout.h"
#include "settings.h"

int main() {
    srand(seed);
    std::cout << "Seed: " << seed << std::endl;

    std::vector<std::vector<std::vector<int>>> entries{{{3, 3}, {2, 1, 0}},
                                                       {{59, 5}, {2, 1, 0}},
                                                       {{115, 5}, {2, 1, 0}},
                                                       {{171, 6}, {2, 1, 0}}};
    // std::vector<std::vector<std::vector<int>>> entries{{{3, 3}, {0}},
    //                                                    {{6, 3}, {1}},
    //                                                    {{9, 3}, {2}},
    //                                                    {{59, 5}, {0}},
    //                                                    {{62, 5}, {1}},
    //                                                    {{65, 5}, {2}},
    //                                                    {{115, 5}, {0}},
    //                                                    {{118, 5}, {1}},
    //                                                    {{121, 5}, {2}},
    //                                                    {{171, 5}, {0}},
    //                                                    {{173, 5}, {1}},
    //                                                    {{176, 5}, {2}}};
    std::vector<std::vector<int>> exits{{0, 10},
                                        {56, 20},
                                        {112, 15},
                                        {168, 15}};
    Roundabout rbt(28, entries, exits, 3, 9, 30.0, 16);

    rbt.space_time_diagram(200, 200);
    // fundamental_diagram(28, entries, exits, 3, 9, 16, 5, 5);

    return 0;
}
