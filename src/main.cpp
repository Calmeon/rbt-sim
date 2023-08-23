#include <iostream>
#include <vector>

#include "car.h"
#include "roundabout.h"

int main() {
    Rounbabout rbt(5, 1, 2);
    rbt.add_car(0, 6, 5);

    for (int i = 0; i < 40; i++) {
        rbt.simulate();
    }

    return 0;
}
