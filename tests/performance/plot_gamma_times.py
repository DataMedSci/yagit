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

plt.rcParams["figure.figsize"] = (10, 5)
# plt.rcParams["figure.figsize"] = (7, 5)

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


# ==============================================
# CONFIG

title = "Comparison of gamma index implementations"

files = [
    "gammaTimes_seq.csv",
    "gammaTimes_thr.csv",
    "gammaTimes_simd.csv",
    "gammaTimes_thr_simd.csv"
]
bars_labels = [
    "Sequential",
    "Multithreaded",
    "SIMD (AVX2)",
    "Multithreaded \nand SIMD (AVX2)"
]

mode = "TIMES"
# mode = "SPEEDUP"

# filters
method = ""
dims = ""
norm = ""
min_dco = 0


# check if config is correct
if len(files) == 0:
    print("ERROR: no input files were provided")
    exit(1)

if len(bars_labels) != len(files) and len(bars_labels) != 0:
    print("ERROR: number of bars labels must be 0 or equal to number of files")
    exit(1)

# print filters
if method == "" and dims == "" and norm == "" and min_dco == 0:
    print("filters: none")
else:
    print(f"filters: method={method}, dims={dims}, norm={norm}, min_dco={min_dco}")


# ==============================================
# READ CSV FILES AND PREPARE DATA

dfs = [pd.read_csv(file) for file in files]

for i in range(len(dfs)):
    if method != "":
        dfs[i] = dfs[i][dfs[i]["method"] == method]
    if dims != "":
        dfs[i] = dfs[i][dfs[i]["dims"] == dims]
    if norm != "":
        dfs[i] = dfs[i][dfs[i]["norm"] == norm]
    if min_dco > 0:
        dfs[i] = dfs[i][dfs[i]["dco"] >= min_dco]
    
    dfs[i] = dfs[i].reset_index(drop=True)

for df in dfs:
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

# summed_times = [(df["nrOfTests"] * df["meanTime[ms]"]).sum() for df in dfs]
# for i, stime in enumerate(summed_times):
#     print(f"summed time for file {i+1}: {stime:.3f} ms ({human_readable_time(stime)})")

summed_mean_times = [times_in_df.sum() for times_in_df in times]
for i, smtime in enumerate(summed_mean_times):
    print(f"summed mean time for file {i+1}: {smtime:.3f} ms ({human_readable_time(smtime)})")


x_labels = [
    f'{method} {dims}\n{dd}%{norm}/{dta}mm{" DCO" if dco > 0 else ""}'
    for method, dims, dd, norm, dta, dco in zip(
        dfs[0]["method"], dfs[0]["dims"],
        dfs[0]["dd[%]"], dfs[0]["norm"], dfs[0]["dta[mm]"], dfs[0]["dco"]
    )
]


# ==============================================
# PLOT

def plot_bars(ax, values, bars_labels, x_labels, color_cycle = None):
    group_count = len(values[0])
    bars_per_group_count = len(values)

    indices = range(group_count)
    few_groups = (group_count <= 4)

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
    if few_groups:
        for bars_container in all_bars:
            for bar in bars_container:
                pos = bar.get_x() + bar.get_width() / 2.0
                height = bar.get_height()
                plt.text(pos, height, f'{height:.2f}', ha='center', va='bottom')

    # add more space at the top
    ax.margins(y = 0.1)

    # set x labels
    # ax.set_xlim(0, group_count)
    ax.set_xlim(-offset, group_count + offset)
    ax.set_xticks([index + offset + (bars_per_group_count - 1) * width / 2 + half_width for index in indices])
    if few_groups:
        ax.set_xticklabels(x_labels)
    else:
        ax.set_xticklabels(x_labels, rotation=90)

    # double the y-tick density
    # mul = 2
    # current_yticks = plt.yticks()[0]
    # new_diff = (current_yticks[1] - current_yticks[0]) / mul
    # new_yticks = [current_yticks[0] + i * new_diff for i in range(len(current_yticks) * mul - 1)]
    # plt.yticks(new_yticks)


color_cycle = ["#1f77b4", "#ff7f0e", "#2ca02c", "#9467bd", "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"]

fig, ax = plt.subplots()

if mode == "TIMES":
    ax.set_ylabel("Time [ms]")
    plot_bars(ax, times, bars_labels, x_labels, color_cycle)
elif mode == "SPEEDUP":
    ax.set_ylabel("Speedup")
    plot_bars(ax, speedups, bars_labels[1:], x_labels, color_cycle[1:])

ax.set_title(title)
ax.grid(axis="y", linestyle=(0, (5, 5)))
if len(bars_labels) > 0:
    ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.12), ncol=5)
    # ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
    # ax.legend()

plt.tight_layout()
plt.show()
