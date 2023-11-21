#include <iostream>
#include <random>

#include "helpers.h"
#include "roundabout.h"
#include "settings.h"

int main() {
    srand(seed);
    std::cout << "Seed: " << seed << std::endl;

    std::vector<std::vector<std::vector<int>>> entries{{{3, 3}, {2, 1, 0}},
                                                       {{43, 5}, {2, 1, 0}},
                                                       {{83, 5}, {2, 1, 0}},
                                                       {{123, 6}, {2, 1, 0}}};
    std::vector<std::vector<int>> exits{{0, 10},
                                        {40, 20},
                                        {80, 15},
                                        {120, 15}};
    Roundabout rbt(28, entries, exits, 3, 9, 30.0, 16);

    rbt.space_time_diagram(200, 200);
    // fundamental_diagram(28, entries, exits, 3, 9, 16, 3, 2);

    return 0;
}
