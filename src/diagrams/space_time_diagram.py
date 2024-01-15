import sys
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import re

LABELS = False


def read_file(filename):
    rbt_dict = dict()
    file = open(filename, mode="r")
    lines = file.readlines()
    file.close()
    # info
    values = lines[2].strip().split(",")

    no_lanes = int(values[0])
    no_entries = int(values[1])
    no_exits = int(values[2])
    lanes_lengths = []

    for l in range(no_lanes):
        lanes_lengths.append(int(values[3 + l]))

    # lanes
    l = 14  # first lines in txt are part of header
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

    return (rbt_dict, lanes_lengths)


def proper_idx(lane_length, idx):
    return (idx + lane_length) % lane_length


def calculate_another_lane_idx(idx, destination_lane, lanes_lengths):
    if destination_lane == (len(lanes_lengths) - 1):
        return idx
    new_idx = round(lanes_lengths[destination_lane] / lanes_lengths[-1] * idx)
    return proper_idx(lanes_lengths[destination_lane], new_idx)


def plot_ee(e, key, lane_lengths, color, y):
    lane_number = (int)(key.split()[-1])
    for e_idx in e:
        lane_e_idx = calculate_another_lane_idx(e_idx, lane_number, lane_lengths)
        plt.axvline(x=lane_e_idx, color=color)
        if lane_e_idx not in list(plt.xticks()[0]):
            plt.xticks(list(plt.xticks()[0]) + [lane_e_idx])


def plot(folder_path, rbt_dict, lane_lengths):
    folder_path += "/space_time_diagrams"
    if not Path(f"{folder_path}/").exists():
        Path(f"{folder_path}").mkdir()
    if not Path(f"{folder_path}/Entries").exists():
        Path(f"{folder_path}/Entries").mkdir()
    if not Path(f"{folder_path}/Exits").exists():
        Path(f"{folder_path}/Exits").mkdir()

    entries = []
    exits = []
    for key in rbt_dict.keys():
        idx = (int)(key.split()[-1])
        if key.startswith("Entry"):
            entries.append(idx)
        elif key.startswith("Exit"):
            exits.append(idx)

    for key in rbt_dict.keys():
        lane = np.array(rbt_dict[key])
        seconds = len(lane)
        lane_len = len(lane[0])
        plt.figure(figsize=(lane_len / 10, seconds / 10))

        tails = (lane == ">").nonzero()
        if len(tails) == 2:
            plt.scatter(tails[1], tails[0], c="gray", marker="3")

        heads = (np.bitwise_and(lane != ">", lane != ".")).nonzero()
        if len(heads) == 2:
            plt.scatter(heads[1], heads[0], c="blue", marker=">")

        if key.startswith("Lane"):
            plt.xticks([0, lane_len - 1])
            plt.xticks(rotation=90)
            plot_ee(exits, key, lane_lengths, "red", 0)
            plot_ee(entries, key, lane_lengths, "green", 1)

        if LABELS:
            plt.xlabel("Driving direction (->)")
            plt.ylabel("Time (s)")
            plt.title(key)
        plt.xlim(0, len(lane[0]))
        plt.ylim(-0.5, seconds - 0.5)
        plt.tight_layout()

        if key.startswith("Entry"):
            save_folder = f"{folder_path}/Entries/{key}.png"
        elif key.startswith("Exit"):
            save_folder = f"{folder_path}/Exits/{key}.png"
        else:
            save_folder = f"{folder_path}/{key}.png"
        plt.savefig(save_folder)
        plt.clf()
        plt.close()


seed = sys.argv[1]
folder_path = f"../history/{seed}"
rbt_dict, lane_lengths = read_file(f"{folder_path}/output.txt")

plot(folder_path, rbt_dict, lane_lengths)
