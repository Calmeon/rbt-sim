#include <random>

#include "roundabout.h"

int main() {
    srand(time(NULL));

    std::map<int, float> entries{{3, 0.5}, {13, 0.5}};
    std::map<int, float> exits{{0, 0.5}, {10, 0.5}};
    Roundabout rbt(5, entries, exits, 3, 9);
    rbt.add_car_rbt(0, 6, 5);
    rbt.add_car_rbt(0, 15, 4);
    rbt.add_car_rbt(0, 20, 2);
    rbt.add_car_rbt(0, 40, 3);
    rbt.print();

    for (int i = 0; i < 1; i++) {
        rbt.simulate();
        rbt.print();
    }

    return 0;
}
