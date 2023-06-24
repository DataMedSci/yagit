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

#include "yagit/ImageData.hpp"

namespace yagit{

/**
 * @brief Container storing gamma index values
 */
class GammaResult : public ImageData{
public:
    using ImageData::ImageData;

    /// @brief The percentage of elements that are less than or equal to 1. It ignores NaN values.
    value_type passingRate() const;

    /// @brief Minimum value of gamma index. It ignores NaN values.
    value_type minGamma() const;
    /// @brief Maximum value of gamma index. It ignores NaN values.
    value_type maxGamma() const;
    /// @brief Sum of values of gamma index. It ignores NaN values.
    double sumGamma() const;
    /// @brief Mean of values of gamma index. It ignores NaN values.
    double meanGamma() const;
    /// @brief Variance of values of gamma index. It ignores NaN values.
    double varGamma() const;
};

}
