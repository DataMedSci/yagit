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

#include "GammaCommon.hpp"

#include <xsimd/xsimd.hpp>

namespace yagit{

namespace{
template <typename T>
using aligned_allocator = xsimd::default_allocator<T>;

template <typename T>
using aligned_vector = std::vector<T, aligned_allocator<T>>;

constexpr size_t SimdElementCount = xsimd::simd_type<float>::size;
}

namespace{
aligned_vector<float> generateCoordinatesAligned(const ImageData& image, ImageAxis axis){
    if(axis == ImageAxis::Z){
        return generateVector<float, aligned_allocator<float>>(image.getOffset().frames, image.getSpacing().frames, image.getSize().frames);
    }
    else if(axis == ImageAxis::Y){
        return generateVector<float, aligned_allocator<float>>(image.getOffset().rows, image.getSpacing().rows, image.getSize().rows);
    }
    else if(axis == ImageAxis::X){
        return generateVector<float, aligned_allocator<float>>(image.getOffset().columns, image.getSpacing().columns, image.getSize().columns);
    }
    return {};
}
}

}
