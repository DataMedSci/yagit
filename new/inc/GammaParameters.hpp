/********************************************************************************************
 * Copyright (C) 2023 'yet Another Gamma Index Tool' Developers.
 * 
 * This file is part of 'yet Another Gamma Index Tool'.
 * 
 * 'yet Another Gamma Index Tool' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'yet Another Gamma Index Tool'.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************************************/
#pragma once

namespace yagit{

/** @enum GammaNormalization
 *  @brief Enum with types of calculating absolute dose difference criterion
*/
enum class GammaNormalization{
    Global,  ///< Using any value you choose (e.g. max reference value)
    Local    ///< Using local reference value (value at current voxel)
};

/** @struct GammaParameters
 *  @brief Structure with parameters of gamma index
 */
struct GammaParameters{
    /// Acceptance criterion for dose difference (DD) in percents [%]
    float ddThreshold;
    /// Acceptance criterion for distance to agreement (DTA) in milimeters [mm]
    float dtaThreshold;
    /// Type of calculating absolute dose difference criterion
    GammaNormalization normalization;
    /// Value used for global normalization
    float globalNormDose;
    /// Value of dose below which gamma index will not be calculated
    float doseCutoff;
};

}
