#include "helpers.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

#include "roundabout.h"
#include "settings.h"

bool is_head(Car *car) { return (car && !car->get_is_tail()); }
bool is_tail(Car *car) { return (car && car->get_is_tail()); }

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
            result += std::to_string(car->get_v());
        }
    }
    result += "\n";

    return result;
}

std::string get_output_file_path() {
    // create history folder
    std::string historyPath = "../history";
    if (!std::filesystem::exists(historyPath)) std::filesystem::create_directory(historyPath);
    // create seed folder
    std::string directoryPath = "../history/" + std::to_string(seed);
    if (!std::filesystem::exists(directoryPath)) std::filesystem::create_directory(directoryPath);

    return directoryPath + "/output.txt";
}

void fundamental_diagram(double island_radius,
                         std::map<int, int> &entries, std::map<int, int> &exits,
                         int number_of_lanes, int max_velocity, int exits_entries_len, int samples) {
    double flow, avg_density;
    std::ofstream history_file(get_output_file_path());

    Roundabout rbt(island_radius, entries, exits, number_of_lanes, max_velocity, 100.0, exits_entries_len);
    std::string history = rbt.get_info() + "\nDensity:Flow:Avg. density\n";

    for (int density = 5; density <= 100; density += 5) {
        flow = 0.0;
        avg_density = 0.0;
        for (int sample = 0; sample < samples; sample++) {
            Roundabout rbt(island_radius, entries, exits, number_of_lanes, max_velocity, density, exits_entries_len);
            rbt.simulate(200);
            rbt.set_saving(true);
            rbt.simulate(1000);
            flow += rbt.get_flow();
            avg_density += rbt.get_avg_density();
        }
        flow /= samples;
        avg_density /= samples;
        history += std::to_string(density) + ":" + std::to_string(flow) + ":" + std::to_string(avg_density) + "\n";
    }

    history_file << history;
    history_file.close();

    std::string python_script = "python3 fundamental_diagram.py " + std::to_string(seed);
    std::cout << "Creating diagram..." << std::endl;
    system(python_script.c_str());
}
