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

class GammaResult : public ImageData<float>{
public:
    using ImageData<float>::ImageData;

    GammaResult(const GammaResult& other) = default;
    GammaResult& operator=(const GammaResult& other) = default;

    GammaResult(GammaResult&& other) noexcept = delete;
    GammaResult& operator=(GammaResult&& other) noexcept = delete;

    bool operator==(const GammaResult& other) const = default;

    float getPassingRate() const;
};

}
