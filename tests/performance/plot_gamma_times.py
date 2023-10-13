#############################################################################################
# Copyright (C) 2023 'Yet Another Gamma Index Tool' Developers.
#
# This file is part of 'Yet Another Gamma Index Tool'.
#
# 'Yet Another Gamma Index Tool' is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# 'Yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with 'Yet Another Gamma Index Tool'.  If not, see <http://www.gnu.org/licenses/>.
#############################################################################################

# Python script that reads multiple csv files that are output from gammaPerf program and compares them using plots.
# 'TIMES' mode plots times from all files.
# 'SPEEDUP' mode plots speedup compared to the first file.
# There is also the possibility to set filters to show only part of the data (e.g. only classic method).

import pandas as pd
import matplotlib.pyplot as plt


# ==============================================
# CONFIG

# plot_size = (11, 5)
plot_size = (10, 5)
# plot_size = (8, 5)
# plot_size = (7, 5)
plt.rcParams["figure.figsize"] = plot_size

title = None

files = [
    "output_seq.csv",
    "output_thr.csv",
    "output_simd.csv",
    "output_thr_simd.csv"
]
bars_labels = [
    "Sequential",
    "Multithreaded",
    "SIMD",
    "Multithreaded \nand SIMD"
]

mode = "TIMES"
# mode = "SPEEDUP"

add_bar_text = False
rotate_xtick_labels = False

# filters
method = ""
dims = ""
dd = None
dta = None
norm = ""
min_dco = None
max_dco = None

# save to file
save_to_png = False
save_to_svg = False
save_to_pdf = False


# ==============================================

# check if config is correct
if len(files) == 0:
    print("ERROR: no input files were provided")
    exit(1)

if len(bars_labels) != len(files) and len(bars_labels) != 0:
    print("ERROR: number of bars labels must be equal to number of files or 0")
    exit(1)

if mode == "SPEEDUP" and len(files) < 2:
    print("ERROR: SPEEDUP mode requires at least 2 input files")
    exit(1)

# print filters
filters = []
if method != "":        filters.append(f"method={method}")
if dims != "":          filters.append(f"dims={dims}")
if dd is not None:      filters.append(f"dd={dd}")
if dta is not None:     filters.append(f"dta={dta}")
if norm != "":          filters.append(f"norm={norm}")
if min_dco is not None: filters.append(f"min_dco={min_dco}")
if max_dco is not None: filters.append(f"max_dco={max_dco}")

if len(filters) == 0:
    print("filters: none")
else:
    print(f"filters: {', '.join(filters)}")


# ==============================================
# READ CSV FILES AND PREPARE DATA

dfs = [pd.read_csv(file) for file in files]

for i in range(len(dfs)):
    if method != "":        dfs[i] = dfs[i][dfs[i]["method"] == method]
    if dims != "":          dfs[i] = dfs[i][dfs[i]["dims"] == dims]
    if dd is not None:      dfs[i] = dfs[i][dfs[i]["dd[%]"] == dd]
    if dta is not None:     dfs[i] = dfs[i][dfs[i]["dta[mm]"] == dta]
    if norm != "":          dfs[i] = dfs[i][dfs[i]["norm"] == norm]
    if min_dco is not None: dfs[i] = dfs[i][dfs[i]["dco"] >= min_dco]
    if max_dco is not None: dfs[i] = dfs[i][dfs[i]["dco"] <= max_dco]

    dfs[i] = dfs[i].reset_index(drop=True)

for df in dfs:
    if df.shape[0] == 0:
        print("ERROR: at least one dataframe is empty")
        exit(1)
    if df.shape != dfs[0].shape:
        print("ERROR: dataframes have different shapes")
        exit(1)

# check if columns containing test config are equal
test_config_cols = ["method", "dims", "dd[%]", "dta[mm]", "norm", "normDose", "dco", "maxSearchDist[mm]", "stepSize[mm]"]
for i, df in enumerate(dfs):
    for col in test_config_cols:
        if (df[col] != dfs[0][col]).any():
            print(f"WARNING: column {col} in dataframe {i+1} is not the same as in the first dataframe")


times = [df["meanTime[ms]"] for df in dfs]
speedups = [times[0] / time for time in times[1:]]

xtick_labels = [
    f"{method.capitalize()} {dims}\n{dd}%{norm}/{dta}mm{chr(10) + 'DCO' if dco > 0 else ''}"
    for method, dims, dd, norm, dta, dco in zip(
        dfs[0]["method"], dfs[0]["dims"],
        dfs[0]["dd[%]"], dfs[0]["norm"], dfs[0]["dta[mm]"], dfs[0]["dco"]
    )
]


# ==============================================

def human_readable_time(time_ms):
    MSEC_IN_SEC = 1000
    SEC_IN_MIN = 60
    MIN_IN_HOUR = 60

    s, ms = divmod(time_ms, MSEC_IN_SEC)
    m, s = divmod(s, SEC_IN_MIN)
    h, m = divmod(m, MIN_IN_HOUR)

    result = str(int(ms)) + "ms"
    if h > 0 or m > 0 or s > 0:
       result = str(int(s)) + "s " + result
    if h > 0 or m > 0:
       result = str(int(m)) + "min " + result
    if h > 0:
       result = str(int(h)) + "h " + result

    return result

# summed_times = [(df["nrOfTests"] * df["meanTime[ms]"]).sum() for df in dfs]
# for i, stime in enumerate(summed_times):
#     print(f"summed time for file {i+1}: {stime:.3f} ms ({human_readable_time(stime)})")

summed_mean_times = [times_in_df.sum() for times_in_df in times]
for i, smtime in enumerate(summed_mean_times):
    print(f"summed mean time for file {i+1}: {smtime:.3f} ms ({human_readable_time(smtime)})")


# ==============================================
# PLOT

def plot_bars(ax, values, bars_labels, xtick_labels, add_bar_text=False, rotate_xtick_labels=False, color_cycle=None):
    group_count = len(values[0])
    bars_per_group_count = len(values)

    indices = range(group_count)

    width = min(0.2, 0.84 / bars_per_group_count)
    half_width = width / 2
    offset = (1 - bars_per_group_count * width) / 2

    # set colors of bars
    if color_cycle is not None:
        ax.set_prop_cycle("color", color_cycle)

    # add bars
    all_bars = []
    for i in range(bars_per_group_count):
        pos = [index + offset + i * width + half_width for index in indices]

        bar = ax.bar(pos, values[i], width, zorder=3)
        if len(bars_labels) > 0:
            bar.set_label(bars_labels[i])

        all_bars.append(bar)

    # add text with value above bars
    if add_bar_text:
        for bars_container in all_bars:
            for bar in bars_container:
                pos = bar.get_x() + bar.get_width() / 2.0
                height = bar.get_height()
                ax.text(pos, height, f"{height:.1f}", ha="center", va="bottom")

    # add more space at the top
    ax.margins(y=0.1)

    # set x labels
    # ax.set_xlim(0, group_count)
    ax.set_xlim(-offset, group_count + offset)
    ax.set_xticks([index + offset + (bars_per_group_count - 1) * width / 2 + half_width for index in indices])
    if not rotate_xtick_labels:
        ax.set_xticklabels(xtick_labels)
    else:
        ax.set_xticklabels(xtick_labels, rotation=90)

    # double the y-tick density
    # mul = 2
    # current_yticks = ax.get_yticks()
    # new_diff = (current_yticks[1] - current_yticks[0]) / mul
    # new_yticks = [current_yticks[0] + i * new_diff for i in range(len(current_yticks) * mul - 1)]
    # ax.set_yticks(new_yticks)


def add_legend(pos="default", rotated_xtick_labels=False):
    def get_lines_count(str):
        if str == "":
            return 0
        else:
            return str.count("\n") + 1

    if pos == "default":
        ax.legend()
    elif pos == "top":
        x, y = (0.5, 1.03)
        ax.legend(loc="lower center", bbox_to_anchor=(x, y), ncol=6)
    elif pos == "bottom":
        if not rotated_xtick_labels:
            xtl_lines_count = max([get_lines_count(label.get_text()) for label in ax.get_xticklabels()])
            x, y = (0.5, -0.04 * (xtl_lines_count + 1))
        else:
            x, y = (0.5, -0.28)
        ax.legend(loc="upper center", bbox_to_anchor=(x, y), ncol=6)
    elif pos == "right":
        x, y = (1.01, 0.5)
        ax.legend(loc="center left", bbox_to_anchor=(x, y), ncol=1)


color_cycle = ["#1f77b4", "#ff7f0e", "#2ca02c", "#9467bd", "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"]

fig, ax = plt.subplots()

if mode == "TIMES":
    ax.set_ylabel("Time [ms]")
    plot_bars(ax, times, bars_labels, xtick_labels,
              add_bar_text, rotate_xtick_labels, color_cycle)
elif mode == "SPEEDUP":
    ax.set_ylabel("Speedup")
    plot_bars(ax, speedups, bars_labels[1:], xtick_labels,
              add_bar_text, rotate_xtick_labels, color_cycle[1:])

if title is not None:
    ax.set_title(title)

if len(bars_labels) > 0:
    add_legend("bottom", rotate_xtick_labels)

ax.grid(axis="y", linestyle=(0, (5, 5)))

plt.tight_layout()


# ==============================================
# SAVE TO FILE

if save_to_png:
    plt.savefig("plot.png", format="png")
if save_to_svg:
    plt.savefig("plot.svg", format="svg")
if save_to_pdf:
    plt.savefig("plot.pdf", format="pdf")


# ==============================================
# SHOW PLOT

plt.show()
