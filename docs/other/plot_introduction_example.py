#############################################################################################
# Copyright (C) 2024 'Yet Another Gamma Index Tool' Developers.
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

# Python script for plotting image that is used on Introduction subpage of documentation.
# It consists of 4 images: a reference image, an evaluated image, a dose difference image, and a gamma index image.

import matplotlib.pyplot as plt
import numpy as np
import SimpleITK as sitk
from matplotlib.colors import LinearSegmentedColormap
from mpl_toolkits.axes_grid1 import make_axes_locatable


# ==============================================
titles = ["Reference image", "Evaluated image", "Dose difference (eval - ref) image", "Gamma index (3%G/3mm) image"]
image_files = ["img_ref2d.mha", "img_eval2d.mha", "dd.mha", "gamma2d.mha"]
image_types = ["dose", "dose", "dd", "gamma"]
colorbar_labels = ["dose [Gy]", "dose [Gy]", "dose diff [Gy]", "gamma index"]

# set vmin and vmax manually
dose_min, dose_max = 0, 2.8
dd_min, dd_max = -0.5, 0.9
gamma_min, gamma_max = 0, 1.6  # gamma_max >= 1.1


# ==============================================

def val_to_perc(val, min, max):
    percent = (val - min) / (max - min)
    if percent < 0 or percent > 1:
        raise Exception("percent outside [0,1] range")
    return percent


dose_colors = [(0, 0, 0), (0, 0, 0.6), (0, 0.5, 1), (0, 1, 1), 
               (0.5, 1, 0.5), (1, 1, 0), (1, 0.5, 0), (1, 0, 0), (0.5, 0, 0)]
dose_positions = np.linspace(0, 1, len(dose_colors))
dose_cmap = LinearSegmentedColormap.from_list("dose_cmap", list(zip(dose_positions, dose_colors)))

dd_colors = [(0, "#cc99ff"),
             (val_to_perc(-0.25, dd_min, dd_max), "#8000ff"),
             (val_to_perc(-0.05, dd_min, dd_max), "#4d0099"),
             (val_to_perc(0, dd_min, dd_max), "black"),
             (val_to_perc(0.05, dd_min, dd_max), "#004d99"),
             (val_to_perc(0.25, dd_min, dd_max), "#0080ff"),
             (1, "#cce6ff")]
dd_cmap = LinearSegmentedColormap.from_list("dd_cmap", dd_colors)

gamma_colors = [(0, "#062e03"),
                (val_to_perc(0.3, gamma_min, gamma_max), "green"),
                (val_to_perc(0.6, gamma_min, gamma_max), "yellowgreen"),
                (val_to_perc(0.999, gamma_min, gamma_max), "orange"),
                (val_to_perc(1, gamma_min, gamma_max), "#ff0000"),
                (1, "#660000")]
gamma_cmap = LinearSegmentedColormap.from_list("gamma_cmap", gamma_colors)


# ==============================================

def read_metaimage_file(file_path):
    image = sitk.ReadImage(file_path)
    image_array = sitk.GetArrayFromImage(image)[0]  # extract only the first frame

    size = image.GetSize()
    origin = image.GetOrigin()
    spacing = image.GetSpacing()

    return image_array, size, origin, spacing

longest_title_len = len(max(titles, key=len))

def print_image_info(title, image_array, size, origin, spacing):
    print(f"{title:<{longest_title_len + 2}}",
          "size:", size[0:2], "  origin:", origin[0:2], "  spacing:", spacing[0:2],
          "  min:", image_array.min(), "  max:", image_array.max())


fig, axes = plt.subplots(2, 2, figsize=(9, 8))

for i, ax in enumerate(axes.flat):
    image_array, size, origin, spacing = read_metaimage_file(image_files[i])
    print_image_info(titles[i], image_array, size, origin, spacing)

    if image_types[i] == "dose":    im = ax.imshow(image_array, cmap=dose_cmap, vmin=dose_min, vmax=dose_max)
    elif image_types[i] == "dd":    im = ax.imshow(image_array, cmap=dd_cmap, vmin=dd_min, vmax=dd_max)
    elif image_types[i] == "gamma": im = ax.imshow(image_array, cmap=gamma_cmap, vmin=gamma_min, vmax=gamma_max)
    else:                           im = ax.imshow(image_array, cmap="grey")

    # ax.axis("off")
    x_ticks = [i for i in range(0, size[0], 32)]
    y_ticks = [i for i in range(0, size[1], 32)]
    x_ticklabels = [int(origin[0] + i*spacing[0]) for i in x_ticks]
    y_ticklabels = [int(origin[1] + i*spacing[1]) for i in y_ticks]
    ax.set_xticks(x_ticks, labels=x_ticklabels, fontsize=9)
    ax.set_yticks(y_ticks, labels=y_ticklabels, fontsize=9)
    ax.xaxis.tick_top()

    ax.set_ylabel("y [mm]")
    ax.set_xlabel("x [mm]")
    ax.xaxis.set_label_position("top")

    ax.set_title(titles[i], y=-0.135)

    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="5%", pad=0.1)
    fig.colorbar(im, cax=cax, label=colorbar_labels[i])


# ==============================================
plt.subplots_adjust(left=0.07, right=0.93,
                    bottom=0.09, top=0.91,
                    wspace=0.34, hspace=0.34)

# plt.savefig("introduction_example.png", format="png")
plt.savefig("introduction_example.jpg", format="jpg")

plt.show()
