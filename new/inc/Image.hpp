/******************************************************************************************
 * This file is part of 'yet Another Gamma Index Tool'.
 *
 * 'yet Another Gamma Index Tool' is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'yet Another Gamma Index Tool'; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 ******************************************************************************************/
#pragma once

#include <vector>

namespace yagit{

/**
 * @brief Type for storing 2D image
 * @tparam T type of pixel value
 */
template <typename T>
using Image2D = std::vector<std::vector<T>>;

/**
 * @brief Type for storing 3D image
 * @tparam T type of voxel value
 */
template <typename T>
using Image3D = std::vector<Image2D<T>>;

/**
 * @brief Axis of image (Z, Y, X)
 */
enum class ImageAxis{
    Z,  ///< Z axis
    Y,  ///< Y axis
    X   ///< X axis
};

/**
 * @brief Plane of image (YX, ZX, ZY)
 */
enum class ImagePlane{
    Axial = 0,          ///< YX plane (Axial, Transverse, Horizontal)
    Transverse = 0,     ///< YX plane (Axial, Transverse, Horizontal)
    Horizontal = 0,     ///< YX plane (Axial, Transverse, Horizontal)
    YX = 0,             ///< YX plane (Axial, Transverse, Horizontal)
    Coronal = 1,        ///< ZX plane (Coronal, Frontal)
    Frontal = 1,        ///< ZX plane (Coronal, Frontal)
    ZX = 1,             ///< ZX plane (Coronal, Frontal)
    Sagittal = 2,       ///< ZY plane (Sagittal, Longitudinal)
    Longitudinal = 2,   ///< ZY plane (Sagittal, Longitudinal)
    ZY = 2              ///< ZY plane (Sagittal, Longitudinal)
};

}
