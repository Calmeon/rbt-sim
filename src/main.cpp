#include "roundabout.h"

int main() {
    Rounbabout rbt(5, 1, 9);
    rbt.add_car(0, 6, 5);
    rbt.add_car(0, 15, 4);
    rbt.add_car(0, 20, 2);
    rbt.add_car(0, 40, 3);
    rbt.print();

    for (int i = 0; i < 10; i++) {
        rbt.simulate();
        rbt.print();
    }

    return 0;
}
