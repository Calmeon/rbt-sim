#include <iostream>
#include <random>

#include "roundabout.h"
#include "settings.h"

int main() {
    srand(SEED);
    std::cout << "Seed: " << SEED << std::endl;

    std::map<int, float> entries{{3, 0.5}, {13, 0.5}};
    std::map<int, float> exits{{0, 0.5}, {10, 0.5}};
    // Roundabout(island_radius, entries, exits, number_of_lanes = 1, max_velocity = 9, density = (1.0F), exits_entries_len = 50)
    Roundabout rbt(5, entries, exits, 3, 9, 1.0, 50);

    rbt.simulate(100);
    rbt.set_saving(true);

    for (int i = 0; i < 10; i++) {
        // rbt.print();
        rbt.simulate();
    }
    // rbt.print();
    rbt.save_history();
    rbt.plot();

    return 0;
}
