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

namespace yagit{

/** @enum GammaNormalization
 *  @brief Enum with types of calculating absolute dose difference criterion
*/
enum class GammaNormalization{
    /// Using any value you choose (e.g. max reference value)
    Global,
    /// Using local reference value (value at current voxel)
    Local
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
