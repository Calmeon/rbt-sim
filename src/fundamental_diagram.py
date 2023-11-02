import sys
import matplotlib.pyplot as plt


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


def plot(densities, flows, folder_path):
    plt.scatter(densities, flows)
    plt.title("Fundamental diagram")
    plt.xlim(densities[0], densities[-1])
    plt.ylim(0, max(flows) * 1.1)
    plt.xlabel("Density (%)")
    plt.ylabel("Flow (veh/h)")
    plt.savefig(f"{folder_path}/fundamental_diagram.png")


seed = sys.argv[1]
folder_path = f"../history/{seed}"
densities, flows = read_file(f"{folder_path}/output.txt")

plot(densities, flows, folder_path)
