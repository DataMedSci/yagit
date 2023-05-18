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

#include "DataStructs.hpp"
#include "Image.hpp"

namespace yagit{

/**
 * @brief Container storing image and its metadata (size, offset, spacing)
 * 
 * Images are in format (frame, row, column) corresponding to (z, y, x) axises.
 * 
 * @note It doesn't contain information in what plane image is oriented.
 * Instead it assumes that all images are in the axial plane.
 */
class ImageData{
public:
    // dose data is stored as float type
    // it is float instead of a double, because float provides sufficient precision for gamma index calculations
    // additionally, it takes two times less space and has better optimization possibilities
    using value_type = float;
    using size_type = std::vector<value_type>::size_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::vector<value_type>::pointer;
    using const_pointer = std::vector<value_type>::const_pointer;

    ImageData() = delete;
    template <typename U>
    ImageData(const std::vector<U>& data, const DataSize& size, const DataOffset& offset, const DataSpacing& spacing);
    template <typename U>
    ImageData(const Image2D<U>& image2d, const DataOffset& offset, const DataSpacing& spacing);
    template <typename U>
    ImageData(const Image3D<U>& image3d, const DataOffset& offset, const DataSpacing& spacing);

    ImageData(const ImageData& other) = default;
    ImageData& operator=(const ImageData& other) = default;

    ImageData(ImageData&& other) noexcept;
    ImageData& operator=(ImageData&& other) noexcept;

    bool operator==(const ImageData& other) const = default;

    DataSize getSize() const;
    DataOffset getOffset() const;
    DataSpacing getSpacing() const;

    /**
     * @brief Set the size of image (frames, rows, columns)
     * @param size New size - new number of elements (frames*rows*columns) must be the same as old
     */
    void setSize(const DataSize& size);
    void setOffset(const DataOffset& offset);
    void setSpacing(const DataSpacing& spacing);

    /// @brief Number of elements of image (frames*rows*columns)
    size_type size() const;

    /// @brief Check that the position is within the valid range and get image element at position (frame, row, column)
    reference at(uint32_t frame, uint32_t row, uint32_t column);
    /// @brief Check that the position is within the valid range and get image element at position (frame, row, column)
    const_reference at(uint32_t frame, uint32_t row, uint32_t column) const;

    /// @brief Get image element at position (frame, row, column)
    reference get(uint32_t frame, uint32_t row, uint32_t column);
    /// @brief Get image element at position (frame, row, column)
    const_reference get(uint32_t frame, uint32_t row, uint32_t column) const;

    /// @brief Get element at @a index of flattened image
    reference get(uint32_t index);
    /// @brief Get element at @a index of flattened image
    const_reference get(uint32_t index) const;

    pointer data();
    const_pointer data() const;

    /// @brief Returns copy of flattened image
    std::vector<value_type> getData() const;
    /// @brief Returns copy of 2D fragment of image
    Image2D<value_type> getImage2D(uint32_t frame, ImagePlane plane = ImagePlane::Axial) const;
    /// @brief Returns copy of 3D image
    Image3D<value_type> getImage3D(ImagePlane plane = ImagePlane::Axial) const;

    /**
     * @brief Get the ImageData containing a 2D fragment of current image
     * @param frame Frame of @a plane which will be returned
     * @param plane 
     * @return ImageData with 2D fragment of current image
     * @note Returned image doesn't contain information about plane - it converts it to axial.
     */
    ImageData getImageData2D(uint32_t frame, ImagePlane plane = ImagePlane::Axial) const;

    /**
     * @brief Get the ImageData containing a 3D image that is rotated according to chosen @a plane
     * @param plane 
     * @return ImageData with rotated 3D image
     * @note Returned image doesn't contain information about plane - it assumes it is axial.
     */
    ImageData getImageData3D(ImagePlane plane) const;

    value_type min() const;
    value_type max() const;
    value_type sum() const;
    value_type mean() const;
    value_type var() const;

    /// @brief Minimum value of the image, ignoring any NaNs
    value_type nanmin() const;
    /// @brief Maximum value of the image, ignoring any NaNs
    value_type nanmax() const;
    /// @brief Sum of image values, ignoring any NaNs
    value_type nansum() const;
    /// @brief Mean of image values, ignoring any NaNs
    value_type nanmean() const;
    /// @brief Variance of image values, ignoring any NaNs
    value_type nanvar() const;

    /// @brief Number of elements of image, ignoring any NaNs
    size_type nansize() const;

    bool containsNan() const;
    bool containsInf() const;

protected:
    std::vector<value_type> m_data;

    DataSize m_size;
    DataOffset m_offset;
    DataSpacing m_spacing;
};


template <typename U>
ImageData::ImageData(const std::vector<U>& data, const DataSize& size, const DataOffset& offset, const DataSpacing& spacing)
    : m_data(data.begin(), data.end()), m_size(size), m_offset(offset), m_spacing(spacing) {
    if(data.size() != size.frames * size.rows * size.columns){
        throw std::invalid_argument("size is inconsistent with data size information");
    }
}

template <typename U>
ImageData::ImageData(const Image2D<U>& image2d, const DataOffset& offset, const DataSpacing& spacing)
    : m_offset(offset), m_spacing(spacing) {
    const uint32_t rows = image2d.size();
    const uint32_t columns = image2d.at(0).size();
    for(const auto& v : image2d){
        if(v.size() != columns){
            throw std::invalid_argument("inner vectors don't have the same size");
        }
        m_data.insert(m_data.end(), v.begin(), v.end());
    }

    m_size = DataSize{1, rows, columns};
}

template <typename U>
ImageData::ImageData(const Image3D<U>& image3d, const DataOffset& offset, const DataSpacing& spacing)
    : m_offset(offset), m_spacing(spacing) {
    const uint32_t frames = image3d.size();
    const uint32_t rows = image3d.at(0).size();
    const uint32_t columns = image3d.at(0).at(0).size();
    for(const auto& v : image3d){
        if(v.size() != rows){
            throw std::invalid_argument("firstly nested vectors don't have the same size");
        }
        for(const auto& v2 : v){
            if(v2.size() != columns){
                throw std::invalid_argument("double nested vectors don't have the same size");
            }
            m_data.insert(m_data.end(), v2.begin(), v2.end());
        }
    }

    m_size = DataSize{frames, rows, columns};
}

}
