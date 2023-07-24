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

# Python script that reads two csv files that are output from gammaPerf program and compares them using plots.
# 'TIMES' mode plots times of first and second versions.
# 'SPEEDUP' mode plots speedup of the second version compared to the first.
# There is also the possibility to set filters to show only part of the data (e.g. only classic method).

import pandas as pd
import matplotlib.pyplot as plt

plt.rcParams["figure.figsize"] = (8,5)

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
file1 = "gammaTimesSequential.csv"
file2 = "gammaTimesThreaded.csv"

mode = "TIMES"
# mode = "SPEEDUP"

title = "Comparison of sequential and threaded gamma index"
bars1_label = "Sequential"
bars2_label = "Threaded"

# filters
method = ""
dims = ""
norm = ""
min_dco = 0

if method == "" and dims == "" and norm == "" and min_dco == 0:
    print("filters: none")
else:
    print(f"filters: method={method}, dims={dims}, norm={norm}, min_dco={min_dco}")


# ==============================================
# READ CSV FILES AND PREPARE DATA
df1 = pd.read_csv(file1)
df2 = pd.read_csv(file2)

if method != "":
    df1 = df1[df1["method"] == method]
    df2 = df2[df2["method"] == method]
if dims != "":
    df1 = df1[df1["dims"] == dims]
    df2 = df2[df2["dims"] == dims]
if norm != "":
    df1 = df1[df1["norm"] == norm]
    df2 = df2[df2["norm"] == norm]
if min_dco > 0:
    df1 = df1[df1["dco"] >= min_dco]
    df2 = df2[df2["dco"] >= min_dco]

if df1.shape != df2.shape:
    print("ERROR: dataframes have different shapes")
    exit(1)

# check if columns containing test config are equal
for col in ["method", "dims", "dd[%]", "dta[mm]", "norm", "normDose", "dco", "maxSearchDist[mm]", "stepSize[mm]"]:
    if (df1[col] != df2[col]).any():
        print(f"WARNING: column {col} in two dataframes are not equal")

meanTime_col = "meanTime[ms]"
time1 = df1[meanTime_col]
time2 = df2[meanTime_col]

summed_time1 = (df1["nrOfTests"] * df1[meanTime_col]).sum()
summed_time2 = (df2["nrOfTests"] * df2[meanTime_col]).sum()
print(f"summed time for file1: {summed_time1:.3f} ms ({human_readable_time(summed_time1)})")
print(f"summed time for file2: {summed_time2:.3f} ms ({human_readable_time(summed_time2)})")

speedup = df1[meanTime_col] / df2[meanTime_col]

labels = [f'{method[0].upper()}{dims}: {dd}%{norm}/{dta}mm{" DCO" if dco > 0 else ""}\nspeedup: {sp:.2f}' \
          for method, dims, dd, norm, dta, dco, sp in \
            zip(df1["method"], df1["dims"], df1["dd[%]"], df1["norm"], df1["dta[mm]"], df1["dco"], speedup)]


# ==============================================
# PLOT
fig, ax = plt.subplots()
ax.set_title(title)
ax.set_ylabel('Time [ms]')

indices = range(len(time1))

if mode == "TIMES":
    width = 0.35
    bars1 = ax.bar(indices, time1, width, zorder=3, label=bars1_label)
    bars2 = ax.bar([index + width for index in indices], time2, width, zorder=3, label=bars2_label)
    ax.set_xticks([index + width/2 for index in indices])
elif mode == "SPEEDUP":
    width = 0.5
    bars = ax.bar(indices, speedup, width, zorder=3, label="Speedup")
    ax.set_xticks(indices)

if len(time1) <= 4:
    ax.set_xticklabels(labels)
else:
    ax.set_xticklabels(labels, rotation=90)

ax.grid()
ax.legend()

plt.tight_layout()
plt.show()
