/********************************************************************************************
 * Copyright (C) 2023 'Yet Another Gamma Index Tool' Developers.
 * 
 * This file is part of 'Yet Another Gamma Index Tool'.
 * 
 * 'Yet Another Gamma Index Tool' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * 'Yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'Yet Another Gamma Index Tool'.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************************************/
#pragma once

#include <vector>

namespace yagit{

/**
 * @brief Type for storing 2D image
 */
using Image2D = std::vector<std::vector<float>>;

/**
 * @brief Type for storing 3D image
 */
using Image3D = std::vector<Image2D>;

/**
 * @brief Axis of image (Z, Y, X)
 * 
 * Axes based on DICOM Reference Coordinate System (RCS)
 */
enum class ImageAxis{
    Z,  ///< Z axis
    Y,  ///< Y axis
    X   ///< X axis
};

/**
 * @brief Plane of image (YX, ZX, ZY).
 * 
 * Planes based on DICOM Reference Coordinate System (RCS)
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
