import sys
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import re


def read_file(filename):
    rbt_dict = dict()
    file = open(filename, mode="r")
    lines = file.readlines()
    file.close()
    # info
    values = lines[1].strip().split(",")

    no_lanes = int(values[0])
    no_entries = int(values[1])
    no_exits = int(values[2])

    # lanes
    l = 3  # first 3 lines in txt are part of header
    while l < len(lines):
        for lane in range(no_lanes):
            line = lines[l].strip().split()[1]

            dict_key = f"Lane {lane}"
            if dict_key not in rbt_dict:
                rbt_dict[dict_key] = []
            rbt_dict[dict_key].append(list(line))
            l += 1

        for lane in range(no_entries):
            line = lines[l].strip().split()
            entry_number = re.findall(r"\d+", line[0])[0]

            dict_key = f"Entry {entry_number}"
            if dict_key not in rbt_dict:
                rbt_dict[dict_key] = []
            rbt_dict[dict_key].append(list(line[1]))
            l += 1

        for lane in range(no_exits):
            line = lines[l].strip().split()
            exit_number = re.findall(r"\d+", line[0])[0]

            dict_key = f"Exit {exit_number}"
            if dict_key not in rbt_dict:
                rbt_dict[dict_key] = []
            rbt_dict[dict_key].append(list(line[1]))
            l += 1

        l += 1

    return rbt_dict


def plot(folder_path, rbt_dict):
    folder_path += "/plots"
    if not Path(f"{folder_path}/").exists():
        Path(f"{folder_path}").mkdir()

    for key in rbt_dict.keys():
        lane = np.array(rbt_dict[key])
        seconds = len(lane)
        plt.figure(figsize=(len(lane[0])/10 ,seconds/10))

        tails = (lane == ">").nonzero()
        if len(tails) == 2:
            plt.scatter(tails[1], tails[0],  c="gray", marker="3")

        heads = (np.bitwise_and(lane != ">", lane != ".")).nonzero()
        if len(heads) == 2:
            plt.scatter(heads[1], heads[0], c="blue", marker=">")

        plt.xlabel("Driving direction")
        plt.ylabel("Time")
        plt.xlim(0, len(lane[0]))
        plt.ylim(-0.5, seconds - 0.5)
        plt.title(key)

        plt.savefig(f"{folder_path}/{key}.png")
        plt.clf()


seed = sys.argv[1]
folder_path = f"../history/{seed}"
rbt_dict = read_file(f"{folder_path}/output.txt")

plot(folder_path, rbt_dict)
