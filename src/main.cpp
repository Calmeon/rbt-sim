#include <iostream>
#include <random>

#include "roundabout.h"
#include "settings.h"

int main() {
    srand(SEED);
    std::cout << "Seed: " << SEED << std::endl;

    std::map<int, float> entries{{3, 20}, {43, 20}, {83, 20}, {123, 20}};
    std::map<int, float> exits{{0, 15}, {40, 20}, {80, 35}, {120, 5}};
    // Roundabout(island_radius, entries, exits, number_of_lanes = 1, max_velocity = 9, density = (1.0F), exits_entries_len = 50)
    Roundabout rbt(28, entries, exits, 3, 9, 1.0, 50);

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
