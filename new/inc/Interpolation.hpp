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

#include "DoseData.hpp"
#include "Image.hpp"

namespace yagit{

class Interpolation{
public:
    static DoseData linear(const DoseData& img, float spacing, ImageAxis axis);
    static DoseData bilinear(const DoseData& img, float firstAxisSpacing, float secondAxisSpacing, ImagePlane plane);
    static DoseData trilinear(const DoseData& img, const DataSpacing& spacing);

    static DoseData linear(const DoseData& img, float offset, float spacing, ImageAxis axis);
    static DoseData bilinear(const DoseData& img, float firstAxisOffset, float secondAxisOffset,
                             float firstAxisSpacing, float secondAxisSpacing, ImagePlane plane);
    static DoseData trilinear(const DoseData& img, const DataOffset& offset, const DataSpacing& spacing);

    static DoseData linear(const DoseData& evalImg, const DoseData& refImg, ImageAxis axis);
    static DoseData bilinear(const DoseData& evalImg, const DoseData& refImg, ImagePlane plane);
    static DoseData trilinear(const DoseData& evalImg, const DoseData& refImg);
};

}
