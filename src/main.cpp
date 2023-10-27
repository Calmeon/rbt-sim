#include <iostream>
#include <random>

#include "roundabout.h"
#include "settings.h"

int main() {
    srand(seed);
    std::cout << "Seed: " << seed << std::endl;

    std::map<int, int> entries{{3, 3}, {43, 5}, {83, 5}, {123, 4}};
    std::map<int, int> exits{{0, 10}, {40, 20}, {80, 15}, {120, 15}};
    // Roundabout(island_radius, entries, exits, number_of_lanes = 1, 
    //              max_velocity = 9, density = (1.0F), exits_entries_len = 50)
    Roundabout rbt(28, entries, exits, 3, 9, 1.0, 200);

    rbt.simulate(200);
    rbt.set_saving(true);

    for (int i = 0; i < 200; i++) {
        // rbt.print();
        rbt.simulate();
    }
    // rbt.print();
    rbt.plot();

    return 0;
}
