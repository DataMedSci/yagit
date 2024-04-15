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

namespace yagit{

/**
 *  @brief Enum with types of calculation of absolute dose difference criterion
*/
enum class GammaNormalization{
    Global,  ///< Using any value you choose (e.g., max value in the reference image)
    Local    ///< Using local reference value (value at current voxel in the reference image)
};

/**
 *  @brief Structure with parameters of gamma index
 */
struct GammaParameters{
    /// Acceptance criterion for dose difference (DD) in percents [%]
    float ddThreshold;
    /// Acceptance criterion for distance to agreement (DTA) in millimeters [mm]
    float dtaThreshold;
    /// Type of calculation of absolute dose difference criterion
    GammaNormalization normalization;
    /// Value used for global normalization
    float globalNormDose;
    /// Dose value in the reference image, below which gamma index will not be calculated
    float doseCutoff;
    /// @brief Maximum search distance in millimeters [mm].
    /// Radius of the circle/sphere in which searching will be performed.
    /// Used only for Wendling method.
    float maxSearchDistance;
    /// @brief Step size in millimeters [mm] that is used when searching within the circle/sphere.
    /// Used only for Wendling method.
    float stepSize;
};

}
