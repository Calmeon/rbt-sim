import sys
import matplotlib.pyplot as plt
from pathlib import Path

# get seed which is folder name as well
seed = sys.argv[1]

# create folder for plots
folder_path = f"../history/{seed}"

if not Path(f"{folder_path}/plots/").exists():
    Path(f"{folder_path}/plots").mkdir()

plt.plot([1, 2, 3, 4], [1, 4, 9, 16])

plt.savefig(f"{folder_path}/plot.png")
plt.clf()

file = open(f"{folder_path}/output.txt", mode="r")
print(file.read())
file.close()
