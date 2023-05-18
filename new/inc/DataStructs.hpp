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

/**
 * @brief Number of frames, rows and columns in 3D image
 */
struct DataSize{
    /// Number of frames
    uint32_t frames;
    /// Number of rows
    uint32_t rows;
    /// Number of columns
    uint32_t columns;

    bool operator==(const DataSize& other) const = default;
};

/**
 * @brief Position of first voxel in 3D image in milimeters [mm]
 * @note Values can be negative
 */
struct DataOffset{
    /// Position of first frame in image in milimeters [mm]
    float framesOffset;
    /// Position of first row in image in milimeters [mm]
    float rowsOffset;
    /// Position of first column in image in milimeters [mm]
    float columnsOffset;

    bool operator==(const DataOffset& other) const = default;
};

/**
 * @brief Distance between voxels in 3D image in milimeters [mm]
 * @note Values should be positive
 */
struct DataSpacing{
    /// Spacing between frames in milimeters [mm]
    float framesSpacing;
    /// Spacing between rows in milimeters [mm]
    float rowsSpacing;
    /// Spacing between columns in milimeters [mm]
    float columnsSpacing;

    bool operator==(const DataSpacing& other) const = default;
};

// TODO: DataDirection with 3 enum fields
// enum with 2 possible values: Forward, Backward

}
