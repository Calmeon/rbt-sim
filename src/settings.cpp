#include "settings.h"

#include "time.h"

std::map<int, int> cars_sizes{{5, 25}, {6, 25}, {7, 25}, {8, 25}};
int seed = time(0);

void update_seed(int new_seed) {
    seed = new_seed;
}
