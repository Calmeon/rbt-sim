#include <iostream>
#include <random>

#include "roundabout.h"
#include "settings.h"

int main() {
    srand(seed);

    std::map<int, int> entries{{3, 3}, {43, 5}, {83, 5}, {123, 6}};
    std::map<int, int> exits{{0, 10}, {40, 20}, {80, 15}, {120, 15}};
    Roundabout rbt(28, entries, exits, 3, 9, 40.0, 200);

    rbt.space_time_diagram(200, 200);

    return 0;
}
