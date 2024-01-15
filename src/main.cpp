#include <iostream>
#include <random>

#include "roundabout.h"
#include "settings.h"
#include "utils.h"

void space_time(std::vector<std::vector<std::vector<int>>> &entries, std::vector<std::vector<int>> &exits);
void compare_dr_in_types(std::vector<std::vector<std::vector<int>>> &entries, std::vector<std::vector<int>> &exits);
void compare_dr_between_types(std::vector<std::vector<std::vector<int>>> &entries, std::vector<std::vector<int>> &exits);

int main(int argc, char *argv[]) {
    if (argc >= 2) update_seed(atoi(argv[1]));
    srand(seed);
    std::cout << "Seed: " << seed << std::endl;

    std::vector<std::vector<std::vector<int>>> entries{{{0, 9}, {0, 1}},
                                                       {{4, 11}, {2}},

                                                       {{28, 5}, {0, 1, 2}},

                                                       {{36, 2}, {0, 1, 2}},

                                                       {{82, 9}, {0, 1}},
                                                       {{90, 11}, {2}},

                                                       {{116, 6}, {0, 1}},
                                                       {{122, 9}, {2}},

                                                       {{156, 9}, {0}},
                                                       {{160, 11}, {1}},
                                                       {{165, 13}, {2}},

                                                       {{204, 5}, {0, 1, 2}}};
    std::vector<std::vector<int>> exits{{19, 6},

                                        {59, 11},
                                        {66, 9},

                                        {107, 4},

                                        {130, 10},
                                        {133, 13},
                                        {139, 12},

                                        {192, 5},

                                        {221, 16},
                                        {226, 14}};

    // Uncomment wanted mode
    space_time(entries, exits);
    // compare_dr_in_types(entries, exits);
    // compare_dr_between_types(entries, exits);

    return 0;
}

void space_time(std::vector<std::vector<std::vector<int>>> &entries, std::vector<std::vector<int>> &exits) {
    Roundabout rbt(28.5, entries, exits, 3, 100);
    rbt.add_agent(0, 2, -2);
    rbt.space_time_diagram(200);
}

void compare_dr_in_types(std::vector<std::vector<std::vector<int>>> &entries, std::vector<std::vector<int>> &exits) {
    // Roundabout
    double radius = 28.5;
    int no_lanes = 3;

    // Agent
    std::vector<int> drs{0, 5, 10};
    std::vector<int> a_plus{1, 2, 4, 4, 1};
    std::vector<int> a_minus{-1, -2, -4, -1, -4};
    std::vector<std::string> types{"I", "II", "III", "IV", "V"};

    // Simulations
    int samples = 5, step = 1, from = 2, to = 40;

    std::string foldername;
    for (int type = 0; type < (int)types.size(); type++) {
        foldername = "Type " + types[type];
        for (int dr = 0; dr < (int)drs.size(); dr++) {
            std::cout << "Type " << types[type] << " dr=" << std::to_string(drs[dr]) << "\n";
            Roundabout rbt(radius, entries, exits, no_lanes);
            rbt.add_agent(drs[dr], a_plus[type], a_minus[type]);
            prepare_fundamental(rbt, samples, step, from, to, std::to_string(dr) + " dr = " + std::to_string(drs[dr]), foldername);
        }
    }
    fundamental_diagram();
}

void compare_dr_between_types(std::vector<std::vector<std::vector<int>>> &entries, std::vector<std::vector<int>> &exits) {
    // Roundabout
    double radius = 28.5;
    int no_lanes = 3;

    // Agent
    std::vector<int> drs{0, 5, 10};
    std::vector<int> a_plus{1, 2, 4, 4, 1};
    std::vector<int> a_minus{-1, -2, -4, -1, -4};
    std::vector<std::string> types{"I", "II", "III", "IV", "V"};

    // Simulations
    int samples = 5, step = 1, from = 2, to = 40;

    std::string foldername;
    for (auto dr : drs) {
        foldername = "dr=" + std::to_string(dr);
        for (int type = 0; type < (int)types.size(); type++) {
            std::cout << "dr=" << dr << " Type " << types[type] << "\n";
            Roundabout rbt(radius, entries, exits, no_lanes);
            rbt.add_agent(dr, a_plus[type], a_minus[type]);
            prepare_fundamental(rbt, samples, step, from, to, std::to_string(type) + " Type " + types[type], foldername);
        }
    }
    fundamental_diagram();
}
