import pandas as pd
import os
import sys


def extract_number(filename):
    return int(filename.split(" ")[0])


def color_cells(value):
    value = round(value)
    color = "red!25" if value < 0 else "green!25" if value > 0 else "white"
    return f"\\cellcolor{{{color}}} {'0' if value == 0 else f'{value:+}' }"


def main():
    folder_path = sys.argv[1]

    file_list = os.listdir(folder_path)
    file_list = [filename for filename in file_list if filename.endswith(".txt")]
    file_list = sorted(file_list, key=extract_number)

    result = pd.DataFrame()
    data = pd.read_csv(
        folder_path + "/" + file_list[0],
        delimiter=":",
        skiprows=11,
    )
    result["Zadana gęstość (\%)"] = data["Density"]

    for file in file_list:
        data = pd.read_csv(
            folder_path + "/" + file,
            delimiter=":",
            skiprows=11,
        )
        col_name = file.split(" ", 1)[1].split(".")[0].replace("Type", "Typ")
        result[col_name] = data["Avg. density"] - data["Density"]

    result_colored = result.iloc[1:, 1:].map(color_cells)
    result_colored = pd.concat([result.iloc[1:, 0], result_colored], axis=1)

    col_format = "c|" + "|".join(["c" for _ in file_list])

    latex_table = result_colored.to_latex(
        index=False, column_format=col_format, escape=False
    )
    foldername = folder_path.split("/")[-1]
    latex_table = (
        "\\begin{table}[!ht]\n\centering\n"
        + latex_table
        + "\caption{Różnice od zadanych gęstości dla dr = "
        + foldername.split("=")[-1]
        + "}\n\label{tab:"
        + foldername
        + "_limit_density}\n\end{table}\n"
    )

    with open(f"{folder_path}/{foldername}.tex", "w") as file:
        file.write(latex_table)


if __name__ == "__main__":
    main()
