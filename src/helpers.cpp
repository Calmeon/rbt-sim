#include "helpers.h"

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
