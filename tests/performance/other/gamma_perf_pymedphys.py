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

import time
import statistics
import csv

import numpy as np
import pydicom
import pymedphys
import SimpleITK as sitk


def get_z_spacing(image):
    if "SliceThickness" in image and image.SliceThickness is not None:
        return image.SliceThickness

    gridFrameOffsetVector = image.GridFrameOffsetVector
    if isinstance(gridFrameOffsetVector, pydicom.multival.MultiValue):
        slices_diff = np.diff(gridFrameOffsetVector)
        if all(slices_diff == slices_diff[0]):
            return slices_diff[0]
        raise ValueError("z-spacing is not evenly spaced")
    else:
        return gridFrameOffsetVector

def print_dcm_info(image):
    size = (image.NumberOfFrames, image.Rows, image.Columns)
    offset = (image.ImagePositionPatient[2], image.ImagePositionPatient[1], image.ImagePositionPatient[0])
    spacing = (get_z_spacing(image), image.PixelSpacing[0], image.PixelSpacing[1])
    print(f"Size: ({size[0]}, {size[1]}, {size[2]})")
    print(f"Offset: ({offset[0]}, {offset[1]}, {offset[2]})")
    print(f"Spacing: ({spacing[0]}, {spacing[1]}, {spacing[2]})")


def save_gamma_to_file(gamma_arr, img_ref, filepath):
    gamma_img = sitk.GetImageFromArray(gamma_arr)
    gamma_img.SetOrigin(img_ref.ImagePositionPatient)
    gamma_img.SetSpacing((get_z_spacing(img_ref), img_ref.PixelSpacing[0], img_ref.PixelSpacing[1]))
    sitk.WriteImage(gamma_img, filepath)


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
    dd = gamma_options["dose_percent_threshold"]
    dta = gamma_options["distance_mm_threshold"]
    normalization = "L" if gamma_options["local_gamma"] else "G"
    global_norm_dose = max_ref_val if gamma_options["global_normalisation"] is None else gamma_options["global_normalisation"]
    dose_cutoff = gamma_options["lower_percent_dose_cutoff"] / 100 * max_ref_val
    max_search_dist = gamma_options["max_gamma"] * gamma_options["distance_mm_threshold"]
    step_size = dta / gamma_options["interp_fraction"]

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
    valid_gamma = gamma_res[~np.isnan(gamma_res)]
    pass_ratio = np.sum(valid_gamma <= 1) / len(valid_gamma) * 100
    nan_values = gamma_res.size - valid_gamma.size

    return [
        round(pass_ratio, 4),
        round(np.nanmean(gamma_res), 6), np.nanmin(gamma_res), np.nanmax(gamma_res),
        gamma_res.size, nan_values
    ]


# ===========================================================================

DATA_REF = "img_reference.dcm"
DATA_EVAL = "img_evaluated.dcm"

CSV_FILENAME = "output_pymedphys.csv"
MHA_FILENAME = "result_pymedphys.mha"

GLOBAL = False
LOCAL = True

DCO = 5         # 5% of max ref dose
STEP_SIZE = 10  # 1/10 of DTA

# PyMedPhys has only Wendling 3D
# config: dd, dta, norm, dco, max_search_dist, step_size, nr_of_tests
configs = [
    (3, 3, GLOBAL, 0,   9, STEP_SIZE, 3),
    (2, 2, GLOBAL, 0,   6, STEP_SIZE, 3),
    (3, 3, LOCAL,  0,   9, STEP_SIZE, 3),
    (2, 2, LOCAL,  0,   6, STEP_SIZE, 3),

    (3, 3, GLOBAL, DCO, 9, STEP_SIZE, 3),
    (2, 2, GLOBAL, DCO, 6, STEP_SIZE, 3),
    (3, 3, LOCAL,  DCO, 9, STEP_SIZE, 3),
    (2, 2, LOCAL,  DCO, 6, STEP_SIZE, 3)
]


# ===========================================================================

img_ref = pydicom.read_file(DATA_REF, force=True)
img_eval = pydicom.read_file(DATA_EVAL, force=True)

axes_ref, dose_ref = pymedphys.dicom.zyx_and_dose_from_dataset(img_ref)
axes_eval, dose_eval = pymedphys.dicom.zyx_and_dose_from_dataset(img_eval)

max_ref_val = dose_ref.max()

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

    # https://docs.pymedphys.com/en/latest/users/ref/lib/gamma.html
    gamma_options = {
        "dose_percent_threshold": dd,         # DD [%]
        "distance_mm_threshold": dta,         # DTA [mm]
        "local_gamma": norm,                  # local or global
        "global_normalisation": None,         # if None then use max value of ref image
        "lower_percent_dose_cutoff": dco,     # dose cutoff (given as percent of max ref dose)

        "max_gamma": max_search_dist / dta,
        "interp_fraction": step_size,         # fraction of dta

        "random_subset": None,
        "ram_available": 2**32                # 4 GB
    }

    times_ms = []
    gamma_res = None
    for _ in range(nr_of_tests):
        start = time.time()
        gamma_res = pymedphys.gamma(axes_ref, dose_ref,
                                    axes_eval, dose_eval,
                                    **gamma_options)
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
    #     save_gamma_to_file(gamma_res, img_ref, MHA_FILENAME)

csv_file.close()
