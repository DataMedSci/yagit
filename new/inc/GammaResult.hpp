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

#include "ImageData.hpp"

namespace yagit{

/**
 * @brief Container storing gamma index values
 */
class GammaResult : public ImageData{
public:
    using ImageData::ImageData;

    GammaResult(const GammaResult& other) = default;
    GammaResult& operator=(const GammaResult& other) = default;

    bool operator==(const GammaResult& other) const = default;

    /// @brief The percentage of elements that are less than or equal to 1. It ignores NaN values.
    value_type passingRate() const;

    /// @brief Minimum value of gamma index. It ignores NaN values.
    value_type minGamma() const;
    /// @brief Maximum value of gamma index. It ignores NaN values.
    value_type maxGamma() const;
    /// @brief Sum of values of gamma index. It ignores NaN values.
    value_type sumGamma() const;
    /// @brief Mean of values of gamma index. It ignores NaN values.
    value_type meanGamma() const;
    /// @brief Variance of values of gamma index. It ignores NaN values.
    value_type varGamma() const;
};

}
