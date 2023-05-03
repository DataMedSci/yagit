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

#include <vector>
#include <stdexcept>
#include <algorithm>

#include "DataStructs.hpp"
#include "Image.hpp"

namespace yagit{

template <typename T>
class ImageData{
public:
    using value_type = T;
    using size_type = std::vector<value_type>::size_type;
    using reference = value_type&;
    using const_reference = const value_type&;

    ImageData() = delete;
    template <typename U>
    ImageData(const std::vector<U>& data, const DataSize& size, const DataOffset& offset, const DataSpacing& spacing);
    template <typename U>
    ImageData(const Image2D<U>& image2d, const DataOffset& offset, const DataSpacing& spacing);
    template <typename U>
    ImageData(const Image3D<U>& image3d, const DataOffset& offset, const DataSpacing& spacing);

    ImageData(const ImageData& other) = default;
    ImageData& operator=(const ImageData& other) = default;

    ImageData(ImageData&& other) noexcept = delete;
    ImageData& operator=(ImageData&& other) noexcept = delete;

    bool operator==(const ImageData& other) const = default; // add: dont compare m_min and m_max

    DataSize getSize() const;
    DataOffset getOffset() const;
    DataSpacing getSpacing() const;

    void setSize(const DataSize& size);
    void setOffset(const DataOffset& offset);
    void setSpacing(const DataSpacing& spacing);

    size_type size() const;

    T& at(uint32_t z, uint32_t y, uint32_t x);
    const T& at(uint32_t z, uint32_t y, uint32_t x) const;
    T& get(uint32_t z, uint32_t y, uint32_t x);
    const T& get(uint32_t z, uint32_t y, uint32_t x) const;
    // T& operator[](uint32_t z, uint32_t y, uint32_t x);  // C++23 required (Multidimensional subscript operator)
    T& get(uint32_t index);
    const T& get(uint32_t index) const;

    std::vector<T> getData() const;
    Image2D<T> getImage2D(uint32_t frame, ImagePlane imgPlane = ImagePlane::Axial) const;
    Image3D<T> getImage3D() const;

    ImageData getImageData2D(uint32_t frame, ImagePlane imgPlane = ImagePlane::Axial) const;

    T getMin() const;
    // T getMin(const std::function<bool(float)>& filter>) const;
    T getMax() const;
    // T getMean() const;
    // T getStd() const;

protected:
    std::vector<T> m_data;

    DataSize m_size;
    DataOffset m_offset;
    DataSpacing m_spacing;

    mutable T m_min; // won't that be more trouble than it's worth?? (when using T& get(...) we need to "null" it)
    mutable T m_max;
};

}

#include "ImageData.tpp"
