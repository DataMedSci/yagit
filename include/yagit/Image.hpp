/********************************************************************************************
 * Copyright (C) 2023-2024 'Yet Another Gamma Index Tool' Developers.
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
#include <string>

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
 * @brief Convert Image2D to string
 * @param img Image2D to convert
 * @param precision Round numbers to @a precision digits after the decimal point.
 * When set to a negative number, conversion precision has a default behavior.
 * @return String representation of Image2D
 */
std::string image2DToString(const yagit::Image2D& img, std::streamsize precision = -1);

/**
 * @brief Convert Image3D to string
 * @param img Image3D to convert
 * @param precision Round numbers to @a precision digits after the decimal point.
 * When set to a negative number, conversion precision has a default behavior.
 * @return String representation of Image3D
 */
std::string image3DToString(const yagit::Image3D& img, std::streamsize precision = -1);

/**
 * @brief Axis of image (Z, Y, X)
 */
enum class ImageAxis{
    Z,  ///< Z axis
    Y,  ///< Y axis
    X   ///< X axis
};

/**
 * @brief Plane of image (YX, ZX, ZY).
 * 
 * Planes based on LPS coordinate system
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
