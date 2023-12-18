#include "settings.h"

#include "time.h"

std::map<int, int> cars_sizes{{4, 5}, {6, 60}, {8, 35}, {10, 10}};
int seed = time(0);

void update_seed(int new_seed) {
    seed = new_seed;
}
