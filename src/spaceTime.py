import sys
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
    l = 3
    while l < len(lines):
        for lane in range(no_lanes):
            line = lines[l].strip().split()[1]

            dict_key = f"Lane {lane}"
            if dict_key not in rbt_dict:
                rbt_dict[dict_key] = []
            rbt_dict[dict_key].append(line)
            l += 1

        for lane in range(no_entries):
            line = lines[l].strip().split()
            entry_number = re.findall(r"\d+", line[0])[0]

            dict_key = f"Entry {entry_number}"
            if dict_key not in rbt_dict:
                rbt_dict[dict_key] = []
            rbt_dict[dict_key].append(line[1])
            l += 1

        for lane in range(no_exits):
            line = lines[l].strip().split()
            exit_number = re.findall(r"\d+", line[0])[0]

            dict_key = f"Exit {exit_number}"
            if dict_key not in rbt_dict:
                rbt_dict[dict_key] = []
            rbt_dict[dict_key].append(line[1])
            l += 1

        l += 1

    return rbt_dict


def plot(folder_path, rbt_dict):
    folder_path += "/plots"
    if not Path(f"{folder_path}/").exists():
        Path(f"{folder_path}").mkdir()

    colors = ["red", "gray", "green"]

    seconds = len(rbt_dict["Lane 0"])
    for i, key in enumerate(rbt_dict.keys()):
        for sec in range(seconds):
            lane = rbt_dict[key][sec]
            if i == 2:
                plt.axhline(
                    y=sec * 3 + i + 0.5, color="r", linestyle="-", linewidth=0.5
                )

            for idx, field in enumerate(lane):
                if field == ">":
                    plt.plot(
                        idx, sec * 3 + i, marker=".", color=colors[i], markersize=1
                    )
                if field != ">" and field != ".":
                    plt.plot(idx, sec * 3 + i, ".b", markersize=3)
                if idx == len(lane) - 1:
                    plt.plot(idx + 1, sec * 3 + i, "*r", markersize=3)
        if i > 1:
            break

    plt.xlabel("Driving direction")
    plt.ylabel("Time")
    plt.xlim(right=len(lane))
    plt.ylim(top=seconds * 3)
    plt.title("Roundabout lanes")

    plt.savefig(f"{folder_path}/plot.png", dpi=300)
    plt.clf()


seed = sys.argv[1]
folder_path = f"../history/{seed}"
rbt_dict = read_file(f"{folder_path}/output.txt")

plot(folder_path, rbt_dict)
