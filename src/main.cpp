#include <random>

#include "roundabout.h"

int main() {
    srand(time(NULL));

    std::map<int, float> entries{{3, 0.5}, {13, 0.5}};
    std::map<int, float> exits{{0, 0.5}, {10, 0.5}};
    Roundabout rbt(5, entries, exits, 3, 1);

    rbt.simulate(40);

    for (int i = 0; i < 20; i++) {
        rbt.print();
        rbt.simulate();
    }
    rbt.print();

    return 0;
}
