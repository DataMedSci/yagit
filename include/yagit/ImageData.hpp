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

#include <vector>
#include <stdexcept>

#include "yagit/DataStructs.hpp"
#include "yagit/Image.hpp"

namespace yagit{

/**
 * @brief Container storing image and its metadata (size, offset, spacing)
 * 
 * Images are in format (frame, row, column) corresponding to (z, y, x) axes.
 * Coordinates are based on LPS coordinate system.
 * 
 * @note It doesn't contain information about the plane in which the image is saved.
 * Instead, it assumes that all images are in the axial plane.
 */
class ImageData{
public:
    // dose data is stored as float type
    // it is a float instead of a double, because a float provides sufficient precision for gamma index calculations
    // additionally, it takes two times less space and has better optimization possibilities
    using value_type = float;
    using size_type = std::vector<value_type>::size_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::vector<value_type>::pointer;
    using const_pointer = std::vector<value_type>::const_pointer;

    ImageData();

    template <typename U>
    ImageData(const std::vector<U>& data, const DataSize& size, const DataOffset& offset, const DataSpacing& spacing);

    ImageData(const Image2D& image2d, const DataOffset& offset, const DataSpacing& spacing);
    ImageData(const Image3D& image3d, const DataOffset& offset, const DataSpacing& spacing);

    ImageData(std::vector<value_type>&& data, const DataSize& size, const DataOffset& offset, const DataSpacing& spacing) noexcept;

    ImageData(const ImageData& other) = default;
    ImageData& operator=(const ImageData& other) = default;

    ImageData(ImageData&& other) noexcept;
    ImageData& operator=(ImageData&& other) noexcept;

    bool operator==(const ImageData& other) const;
    bool operator!=(const ImageData& other) const;

    DataSize getSize() const{
        return m_size;
    }
    DataOffset getOffset() const{
        return m_offset;
    }
    DataSpacing getSpacing() const{
        return m_spacing;
    }

    /**
     * @brief Set the size of image (frames, rows, columns)
     * @param size New size - new number of elements (frames*rows*columns) must be the same as old
     */
    void setSize(const DataSize& size);
    void setOffset(const DataOffset& offset);
    void setSpacing(const DataSpacing& spacing);

    /// @brief Number of elements of image (frames*rows*columns)
    size_type size() const{
        return m_data.size();
    }

    /// @brief Get image element at position (@a frame, @a row, @a column)
    /// with checking that the position is within the valid range
    reference at(uint32_t frame, uint32_t row, uint32_t column);
    /// @brief Get image element at position (@a frame, @a row, @a column)
    /// with checking that the position is within the valid range
    const_reference at(uint32_t frame, uint32_t row, uint32_t column) const;

    /// @brief Get image element at position (@a frame, @a row, @a column)
    reference get(uint32_t frame, uint32_t row, uint32_t column){
        return const_cast<reference>(const_cast<const ImageData*>(this)->get(frame, row, column));
    }
    /// @brief Get image element at position (@a frame, @a row, @a column)
    const_reference get(uint32_t frame, uint32_t row, uint32_t column) const{
        return m_data[(frame * m_size.rows + row) * m_size.columns + column];
    }

    /// @brief Get element at @a index of flattened image
    reference get(uint32_t index){
        return const_cast<reference>(const_cast<const ImageData*>(this)->get(index));
    }
    /// @brief Get element at @a index of flattened image
    const_reference get(uint32_t index) const{
        return m_data[index];
    }

    pointer data(){
        return const_cast<pointer>(const_cast<const ImageData*>(this)->data());
    }
    const_pointer data() const{
        return m_data.data();
    }

    /// @brief Returns copy of flattened image
    std::vector<value_type> getData() const;
    /// @brief Returns copy of 2D fragment of image
    Image2D getImage2D(uint32_t frame, ImagePlane plane = ImagePlane::Axial) const;
    /// @brief Returns copy of 3D image
    Image3D getImage3D(ImagePlane plane = ImagePlane::Axial) const;

    /**
     * @brief Get the ImageData containing a 2D fragment of current image
     * @param frame Frame of @a plane which will be returned
     * @param plane Image plane according to which 2D image is obtained
     * @return ImageData with 2D fragment of current image
     * @note Returned image doesn't contain information about plane - it is assumed it is axial.
     */
    ImageData getImageData2D(uint32_t frame, ImagePlane plane = ImagePlane::Axial) const;

    /**
     * @brief Get the ImageData containing a 3D image that is rotated according to chosen @a plane
     * @param plane Image plane according to which image is rotated
     * @return ImageData with rotated 3D image
     * @note Returned image doesn't contain information about plane - it is assumed it is axial.
     */
    ImageData getImageData3D(ImagePlane plane) const;

    /// @brief Minimum value of the image
    /// @warning If image starts with NaN, then minimum value will be NaN
    value_type min() const;
    /// @brief Maximum value of the image
    /// @warning If image starts with NaN, then maximum value will be NaN
    value_type max() const;
    /// @brief Sum of image values
    /// @warning If image contains NaN, then sum will be Nan
    double sum() const;
    /// @brief Mean of image values
    /// @warning If image contains NaN, then mean will be Nan
    double mean() const;
    /// @brief Variance of image values
    /// @warning If image contains NaN, then variance will be Nan
    double var() const;

    /// @brief Minimum value of the image, ignoring any NaNs
    value_type nanmin() const;
    /// @brief Maximum value of the image, ignoring any NaNs
    value_type nanmax() const;
    /// @brief Sum of image values, ignoring any NaNs
    double nansum() const;
    /// @brief Mean of image values, ignoring any NaNs
    double nanmean() const;
    /// @brief Variance of image values, ignoring any NaNs
    double nanvar() const;

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
    if(spacing.frames <= 0 || spacing.rows <= 0 || spacing.columns <= 0){
        throw std::invalid_argument("spacing should be greater than 0");
    }
    if(data.size() != size.frames * size.rows * size.columns){
        throw std::invalid_argument("size is inconsistent with data size information");
    }
}

}
