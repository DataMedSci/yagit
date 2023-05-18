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
#include <numeric>
#include <cmath>
#include <limits>

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
 * 
 * @tparam T type of pixel value
 */
template <typename T>
class ImageData{
public:
    using value_type = T;
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

    // ImageData(ImageData&& other) noexcept = delete;
    // ImageData& operator=(ImageData&& other) noexcept = delete;

    ImageData(ImageData&& other) noexcept = default;
    ImageData& operator=(ImageData&& other) noexcept = default;

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

    /// @brief Get image element at position (frame, row, column) ensuring that the position is within the valid range
    T& at(uint32_t frame, uint32_t row, uint32_t column);
    /// @brief Get image element at position (frame, row, column) ensuring that the position is within the valid range
    const T& at(uint32_t frame, uint32_t row, uint32_t column) const;

    /// @brief Get image element at position (frame, row, column)
    T& get(uint32_t frame, uint32_t row, uint32_t column);
    /// @brief Get image element at position (frame, row, column)
    const T& get(uint32_t frame, uint32_t row, uint32_t column) const;

    /// @brief Get element at @a index of flattened image
    T& get(uint32_t index);
    /// @brief Get element at @a index of flattened image
    const T& get(uint32_t index) const;

    pointer data();
    const_pointer data() const;

    /// @brief Returns copy of flattened image
    std::vector<T> getData() const;
    /// @brief Returns copy of 2D fragment of image
    Image2D<T> getImage2D(uint32_t frame, ImagePlane plane = ImagePlane::Axial) const;
    /// @brief Returns copy of 3D image
    Image3D<T> getImage3D(ImagePlane plane = ImagePlane::Axial) const;

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

    T min() const;
    T max() const;
    T sum() const;
    T mean() const;
    T var() const;

    /// @brief Minimum value of the image, ignoring any NaNs
    T nanmin() const;
    /// @brief Maximum value of the image, ignoring any NaNs
    T nanmax() const;
    /// @brief Sum of image values, ignoring any NaNs
    T nansum() const;
    /// @brief Mean of image values, ignoring any NaNs
    T nanmean() const;
    /// @brief Variance of image values, ignoring any NaNs
    T nanvar() const;

    /// @brief Number of elements of image, ignoring any NaNs
    size_type nansize() const;

    bool containsNan() const;
    bool containsInf() const;

protected:
    std::vector<T> m_data;

    DataSize m_size;
    DataOffset m_offset;
    DataSpacing m_spacing;
};

}

#include "ImageData.tpp"
