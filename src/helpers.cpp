#include "helpers.h"

#include <random>

#include "settings.h"

int proper_idx(std::vector<Car *> &lane, int idx) {
    int size = lane.size();

    return (idx + size) % size;
}

int find_next(std::vector<Car *> &lane, int idx) {
    int p_idx;

    for (int i = 1; i <= (int)lane.size(); i++) {
        p_idx = proper_idx(lane, idx + i);
        // if loop finds not nullptr it is next car
        if (lane[p_idx]) {
            return i - 1;
        }
    }
    return lane.size() - 1;
}

int find_prev(std::vector<Car *> &lane, int idx) {
    int p_idx;

    idx = proper_idx(lane, idx);
    // this gives option to check if there is no car
    if (lane[idx] != nullptr && !lane[idx]->get_is_tail()) {
        idx = idx - lane[idx]->get_space() + 1;
        idx = proper_idx(lane, idx);
    }

    for (int i = 1; i <= (int)lane.size(); i++) {
        p_idx = proper_idx(lane, idx - i);
        // if loop finds not nullptr it is prev car
        if (lane[p_idx]) {
            return i - 1;
        }
    }
    return lane.size() - 1;
}

/*
 * Return random number based on weights
 * Function sums weights draws random number in that range
 * and checks where it lands in distribution array
 * ex.
 * values =               [ 1,  2,  3,  4,  5]
 * weights =              [20, 50, 10,  5, 13]
 * weights_distribution = [20, 70, 80, 85, 98]
 * random_number = 76
 * it lands between 70 and 80 so function returns 2
 */
int weighted_random_choice(std::map<int, int> &dict) {
    std::vector<int> values, weights_distribution;
    int random_number;
    int weight_sum = 0;

    for (auto &el : dict) {
        values.push_back(el.first);
        weight_sum += el.second;
        weights_distribution.push_back(weight_sum);
    }

    random_number = rand() % weight_sum;
    for (int idx = 0; idx < (int)weights_distribution.size(); idx++) {
        if (random_number <= weights_distribution[idx]) {
            return values[idx];
        }
    }

    // In case function fails just pick random from values
    return values[rand() % ((int)values.size() - 1)];
}
