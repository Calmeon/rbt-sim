#include "helpers.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "settings.h"

bool is_head(Car *car) { return (car && !car->get_is_tail()); }
bool is_tail(Car *car) { return (car && car->get_is_tail()); }

bool contains(std::set<Car *> &set, Car *car) {
    return set.find(car) != set.end();
}

int proper_idx(std::vector<Car *> &lane, int idx) {
    int size = lane.size();
    return (idx + size) % size;
}

int find_next(std::vector<Car *> &lane, int idx) {
    int p_idx;

    for (int i = 1; i <= (int)lane.size(); i++) {
        p_idx = proper_idx(lane, idx + i);
        // if loop finds not nullptr it is next car
        if (lane[p_idx]) return i - 1;
    }
    return lane.size() - 1;
}

int find_prev(std::vector<Car *> &lane, int idx) {
    int p_idx;

    idx = proper_idx(lane, idx);
    // this gives option to check if there is no car
    if (is_head(lane[idx])) {
        idx = idx - lane[idx]->get_space() + 1;
        idx = proper_idx(lane, idx);
    }

    for (int i = 1; i <= (int)lane.size(); i++) {
        p_idx = proper_idx(lane, idx - i);
        // if loop finds not nullptr it is prev car
        if (lane[p_idx]) return i - 1;
    }
    return lane.size() - 1;
}

int d_brake(int v) {
    int result = 0;
    int no_steps = v / V_M;
    for (int i = 1; i <= no_steps; i++)
        result += (v - i * V_M);
    return result;
}

int d_f(int v) {
    return v + d_brake(v);
}

double d_acc(int v, int a_plus, double g, int v_next) {
    double d_acc = ((double)d_f(v + a_plus) + g) - (double)d_brake(v_next);
    return std::max(0.0, d_acc);
}

double d_keep(int v, double g, int v_next) {
    double d_keep = ((double)d_f(v) + g) - (double)d_brake(v_next);
    return std::max(0.0, d_keep);
}

double d_dec(int v, int a_minus, double g, int v_next) {
    double d_dec = ((double)d_f(v + a_minus) + g) - (double)d_brake(v_next);
    return std::max(0.0, d_dec);
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
        if (random_number <= weights_distribution[idx]) return values[idx];
    }

    // In case function fails just pick random from values
    return values[rand() % ((int)values.size() - 1)];
}

std::string prepare_string_lane(std::vector<Car *> &lane, std::string s, int intend) {
    std::string result = "";

    s.insert(s.end(), intend - s.size(), ' ');
    result += s;
    for (auto &car : lane) {
        if (!car) {
            result += ".";
        } else if (car->get_is_tail()) {
            result += ">";
        } else {
            std::ostringstream ss;
            ss << std::hex << car->get_v();
            result += ss.str();
        }
    }
    result += "\n";

    return result;
}

std::string get_output_file_path(std::string filename) {
    // create history folder
    std::string historyPath = "../history";
    if (!std::filesystem::exists(historyPath)) std::filesystem::create_directory(historyPath);
    // create seed folder
    std::string directoryPath = "../history/" + std::to_string(seed);
    if (!std::filesystem::exists(directoryPath)) std::filesystem::create_directory(directoryPath);

    return directoryPath + "/" + filename + ".txt";
}

void prepare_fundamental(Roundabout &rbt, int samples, int step, int from, int to, std::string title, bool only_rbt) {
    double flow, avg_density;
    std::string history = rbt.get_info() + "\nDensity:Flow:Avg. density\n0:0.000000:0.000000\n";

    for (int density = from; density <= to; density += step) {
        flow = 0.0;
        avg_density = 0.0;

        for (int sample = 0; sample < samples; sample++) {
            rbt.set_max_density(density);
            rbt.simulate(200);
            rbt.set_saving(true);
            rbt.simulate(1000);
            flow += rbt.get_flow();
            avg_density += only_rbt ? rbt.get_avg_density_rbt() : rbt.get_avg_density();
            rbt.reset_rbt();
        }
        flow /= samples;
        avg_density /= samples;
        history += std::to_string(density) + ":" + std::to_string(flow) + ":" + std::to_string(avg_density) + "\n";
    }

    std::ofstream history_file(get_output_file_path(title));
    history_file << history;
    history_file.close();
}

void fundamental_diagram() {
    std::string python_script = "python3 diagrams/fundamental_diagram.py " + std::to_string(seed);
    std::cout << "Creating diagram...\n";
    system(python_script.c_str());
}

void print_error(std::string function, std::string lane_type, int lane_number, int idx, int second) {
    std::cerr << "==========ERROR=========="
              << "\nSeed: " << seed
              << "\nSecond: " << second
              << "\nFunction: " << function
              << "\nLane type: " << lane_type
              << "\nLane number: " << lane_number
              << "\nIndex: " << idx
              << "\n=========================\n";
}
