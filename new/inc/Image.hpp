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

template <typename T>
using Image2D = std::vector<std::vector<T>>;
template <typename T>
using Image3D = std::vector<Image2D<T>>;

// desc
// Axial/Transverse/Horizontal
// Coronal/Frontal
// Sagittal/Longitudinal
enum class ImagePlane{
    Axial = 0,
    Transverse = 0,
    Horizontal = 0,
    Coronal = 1,
    Frontal = 1,
    Sagittal = 2,
    Longitudinal = 2
};

}