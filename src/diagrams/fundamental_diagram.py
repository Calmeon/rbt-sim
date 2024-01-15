import sys
import numpy as np
import matplotlib.pyplot as plt
import os


def read_file(filename):
    file = open(filename, mode="r")
    lines = file.readlines()
    file.close()

    densities = []
    flows = []
    for line in lines[12:]:
        line_values = line.split(":")
        densities.append(float(line_values[0]))
        flows.append(float(line_values[1]))

    return densities, flows


def extract_number(filename):
    return int(filename.split(" ")[0])


def plot_comparison(folder_path):
    densities = []
    flows = []

    file_list = os.listdir(folder_path)
    file_list = [filename for filename in file_list if filename.endswith(".txt")]
    file_list = sorted(file_list, key=extract_number)

    max_flow = 0
    for filename in file_list:
        if filename.endswith(".txt"):
            file_path = os.path.join(folder_path, filename)
            densities, flows = read_file(file_path)
            max_flow = max(max_flow, *flows)
            label = os.path.splitext(filename)[0].split(" ", 1)[1]
            plot(densities, flows, "", label)

    plt.ylim(0, max_flow * 1.1)
    plt.legend()
    plt.savefig(f"{folder_path}/{folder_path.split('/')[-1]}.png", dpi=300)
    plt.clf()


def plot(densities, flows, folder_path="", label=""):
    coeff = np.polyfit(densities, flows, 8)
    poly1d = np.poly1d(coeff)

    xn = np.linspace(min(densities), max(densities), 1000)
    yn = poly1d(xn)

    plt.scatter(densities, flows)
    if label == "":
        plt.plot(xn, yn)
    else:
        plt.plot(xn, yn, label=label)

    plt.xlim(densities[0], densities[-1])
    plt.ylim(0, max(flows) * 1.1)
    plt.xlabel("Density (%)")
    plt.ylabel("Flow (veh/h)")
    if folder_path != "":
        plt.savefig(f"{folder_path}/fundamental_diagram.png")


def main():
    seed = sys.argv[1]
    folder_path = f"../history/{seed}"

    if os.path.isfile(f"{folder_path}/output.txt"):
        densities, flows = read_file(f"{folder_path}/output.txt")
        plot(densities, flows, folder_path)
    else:
        folder_list = os.listdir(folder_path)
        for folder in folder_list:
            plot_comparison(folder_path + f"/{folder}")


if __name__ == "__main__":
    main()
