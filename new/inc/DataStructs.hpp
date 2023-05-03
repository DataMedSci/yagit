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

#include <cstdint>

namespace yagit{

// number of frames, rows and columns in image
struct DataSize{
    uint32_t frames;  // size_t?
    uint32_t rows;
    uint32_t columns;

    bool operator==(const DataSize& other) const = default;
};

// spacing before offset??
// TODO: desc
// position of first pixel in milimeters [mm]
struct DataOffset{  // maybe double?
    float framesOffset;
    float rowsOffset;
    float columnsOffset;

    bool operator==(const DataOffset& other) const = default;
};

// distance between frames, rows and columns in mm
// TODO: desc
// difference between consecutive pixels in milimeters [mm]
// NOT YET: value can be negative
struct DataSpacing{  // maybe double?
    float framesSpacing;
    float rowsSpacing;
    float columnsSpacing;

    bool operator==(const DataSpacing& other) const = default;
};

// DataDirection??
// with bools or +1/-1

}
