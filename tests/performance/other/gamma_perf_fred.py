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

# works only on Linux
# when using WSL, run this script within WSL location and not Windows location (don't use /mnt/c/),
# because running it within Windows location is slower

# I additionaly use maxSearchDist parameter that is absent in fredtools
# to add this, you need to do the following:
# 1. add this parameter to calcGammaIndex function in fredtools
# maxSearchDist=None
# 2. add this code to calcGammaIndex function body (it sets gammaSearchMax based on maxSearchDist)
# if maxSearchDist is not None:
#     gammaSearchMax = maxSearchDist / DTA
#     libFredGI.fredGI_setGammaSearchMax(ctypes.c_float(gammaSearchMax))

import time
import statistics
import csv

import numpy as np
import SimpleITK as sitk
import fredtools as ft


def read_dcm_rtdose(filepath):
    img = sitk.ReadImage(filepath, imageIO="GDCMImageIO")

    # convert dicom int pixels to dose value pixels
    dose_grid_scaling = float(img.GetMetaData("3004|000e"))
    img_dose_arr = dose_grid_scaling * sitk.GetArrayFromImage(img)
    img_dose = sitk.GetImageFromArray(img_dose_arr)

    img_dose.SetOrigin(img.GetOrigin())
    img_dose.SetSpacing(img.GetSpacing())

    return img_dose

def print_dcm_info(image):
    size = image.GetSize()
    offset = image.GetOrigin()
    spacing = image.GetSpacing()
    print(f"Size: ({size[2]}, {size[1]}, {size[0]})")
    print(f"Offset: ({offset[2]}, {offset[1]}, {offset[0]})")
    print(f"Spacing: ({spacing[2]}, {spacing[1]}, {spacing[0]})")


def save_gamma_to_file(gamma, filepath):
    gamma_arr = sitk.GetArrayFromImage(gamma)

    # convert negative numbers to NaN
    gamma_arr[gamma_arr < 0] = np.NaN
    gamma2 = sitk.GetImageFromArray(gamma_arr)

    # copy offset and spacing
    gamma2.SetOrigin(gamma.GetOrigin())
    gamma2.SetSpacing(gamma.GetSpacing())

    # copy all additional metadata
    for key in gamma.GetMetaDataKeys():
        gamma2.SetMetaData(key, gamma.GetMetaData(key))

    ft.writeMHD(gamma2, filepath, singleFile=False, overwrite=True, displayInfo=False)


def csv_header():
    return [
        "method", "dims",
        "dd[%]", "dta[mm]", "norm", "normDose", "dco",
        "maxSearchDist[mm]", "stepSize[mm]",
        "nrOfTests",
        "meanTime[ms]", "stdTime[ms]", "minTime[ms]", "maxTime[ms]",
        "GIPR[%]", "meanGamma", "minGamma", "maxGamma", "gammaSize", "NaNvalues"
    ]

def config_to_csv(gamma_options, max_ref_val, nr_of_tests):
    dd = gamma_options["DD"]
    dta = gamma_options["DTA"]
    normalization = "G" if gamma_options["DDType"] == "global" else "L"
    global_norm_dose = max_ref_val if gamma_options["globalNorm"] is None else gamma_options["globalNorm"]
    dose_cutoff = gamma_options["DCO"] * max_ref_val
    max_search_dist = gamma_options["maxSearchDist"]
    step_size = dta / gamma_options["stepSize"] if gamma_options["fractionalStepSize"] else gamma_options["stepSize"]

    return [
        "wendling", "3D",
        round(dd, 6), round(dta, 6), normalization, round(global_norm_dose, 6), round(dose_cutoff, 6),
        round(max_search_dist, 6), round(step_size, 6),
        nr_of_tests
    ]

def time_stats_to_csv(times_ms):
    mean = statistics.mean(times_ms)
    standard_deviation = statistics.pstdev(times_ms)  # population sd
    min_time = min(times_ms)
    max_time = max(times_ms)

    return [
        f"{mean:.6f}", f"{standard_deviation:.6f}",
        f"{min_time:.3f}", f"{max_time:.3f}"
    ]

def gamma_result_to_csv(gamma_res):
    gamma_stat = ft.getGIstat(gamma_res)
    gamma_arr = sitk.GetArrayFromImage(gamma_res)
    nan_values = np.count_nonzero(gamma_arr == -1)

    return [
        round(gamma_stat["passRate"], 4),
        round(gamma_stat["mean"], 6), gamma_stat["min"], gamma_stat["max"],
        gamma_arr.size, nan_values
    ]


# ===========================================================================

DATA_REF = "img_reference.dcm"
DATA_EVAL = "img_evaluated.dcm"

CSV_FILENAME = "output_fred.csv"
MHA_FILENAME = "result_fred.mha"

GLOBAL = "global"
LOCAL = "local"

DCO = 0.05      # 5% of max ref dose
STEP_SIZE = 10  # 1/10 of DTA

CPU_NO = "auto"
# CPU_NO = None  # sequential
# CPU_NO = 8

# FRED has only Wendling 3D
# FRED requires DCO to be greater than 0
# config: dd, dta, norm, dco, max_search_dist, step_size, nr_of_tests
configs = [
    (3, 3, GLOBAL, DCO, 9, STEP_SIZE, 10),
    (2, 2, GLOBAL, DCO, 6, STEP_SIZE, 10),
    (3, 3, LOCAL,  DCO, 9, STEP_SIZE, 10),
    (2, 2, LOCAL,  DCO, 6, STEP_SIZE, 10)
]


# ===========================================================================

img_ref = read_dcm_rtdose(DATA_REF)
img_eval = read_dcm_rtdose(DATA_EVAL)

max_ref_val = sitk.GetArrayFromImage(img_ref).max()

print("Reference image")
print_dcm_info(img_ref)
print("--------------")
print("Evaluated image")
print_dcm_info(img_eval)

print("==================================")

csv_file = open(CSV_FILENAME, "w")
csv_writer = csv.writer(csv_file)
csv_writer.writerow(csv_header())

for i, config in enumerate(configs):
    print(f"{i+1}/{len(configs)}", end="")

    dd, dta, norm, dco, max_search_dist, step_size, nr_of_tests = config

    # https://www.fredtools.ifj.edu.pl/Documentation/gammaIndexAnalyse.html
    gamma_options = {
        "DD": dd,             # DD [%]
        "DTA": dta,           # DTA [mm]
        "DDType": norm,       # global or local
        "globalNorm": None,   # if None then use max value of ref image
        "DCO": dco,           # dose cutoff (given as fraction of max ref dose)

        "maxSearchDist": max_search_dist,   # my custom parameter!!! you need to modify fredtools source code for this to work
        "fractionalStepSize": True,         # if True then stepSize is fraction of dta, else stepSize is absolute value
        "stepSize": step_size,

        "CPUNo": CPU_NO,
        "displayInfo": False
    }

    times_ms = []
    gamma_res = None
    for _ in range(nr_of_tests):
        start = time.time()
        gamma_res = ft.calcGammaIndex(img_ref, img_eval, **gamma_options)
        end = time.time()

        time_ms = (end - start) * 1000
        times_ms.append(time_ms)

    mean = statistics.mean(times_ms)
    print(f" - mean time: {mean:.6f} ms")

    record = config_to_csv(gamma_options, max_ref_val, nr_of_tests) +\
             time_stats_to_csv(times_ms) +\
             gamma_result_to_csv(gamma_res)
    csv_writer.writerow(record)

    # if i == 0:
    #     save_gamma_to_file(gamma_res, MHA_FILENAME)

csv_file.close()
