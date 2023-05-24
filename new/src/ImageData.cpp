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

#include "ImageData.hpp"

#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>

namespace yagit{

namespace{
using value_type = ImageData::value_type;
using reference = ImageData::reference;
using const_reference = ImageData::const_reference;
using pointer = ImageData::pointer;
using const_pointer = ImageData::const_pointer;
}

ImageData::ImageData(std::vector<value_type>&& data, const DataSize& size, const DataOffset& offset, const DataSpacing& spacing) noexcept
    : m_data(std::move(data)), m_size(size), m_offset(offset), m_spacing(spacing) {}

ImageData::ImageData(ImageData&& other) noexcept
    : m_data(std::move(other.m_data)), m_size(other.m_size), m_offset(other.m_offset), m_spacing(other.m_spacing){
    other.m_size = {0, 0, 0};
    other.m_offset = {0, 0, 0};
    other.m_spacing = {0, 0, 0};
}

ImageData& ImageData::operator=(ImageData&& other) noexcept{
    if(this != &other){
        m_data = std::move(other.m_data);
        m_size = other.m_size;
        m_offset = other.m_offset;
        m_spacing = other.m_spacing;
        other.m_size = {0, 0, 0};
        other.m_offset = {0, 0, 0};
        other.m_spacing = {0, 0, 0};
    }
    return *this;
}

bool ImageData::operator==(const ImageData& other) const{
    return m_data == other.m_data && m_size == other.m_size && m_offset == other.m_offset && m_spacing == other.m_spacing;
}

DataSize ImageData::getSize() const{
    return m_size;
}

DataOffset ImageData::getOffset() const{
    return m_offset;
}

DataSpacing ImageData::getSpacing() const{
    return m_spacing;
}

void ImageData::setSize(const DataSize& size){
    if(m_size.frames * m_size.rows * m_size.columns != size.frames * size.rows * size.columns){
        throw std::invalid_argument("the total number of elements in the new size does not match the total number of elements in the old size");
    }
    m_size = size;
}

void ImageData::setOffset(const DataOffset& offset){
    m_offset = offset;
}

void ImageData::setSpacing(const DataSpacing& spacing){
    m_spacing = spacing;
}

ImageData::size_type ImageData::size() const{
    return m_data.size();
}

reference ImageData::at(uint32_t z, uint32_t y, uint32_t x){
    return const_cast<reference>(const_cast<const ImageData*>(this)->at(z, y, x));
}

const_reference ImageData::at(uint32_t z, uint32_t y, uint32_t x) const{
    if(z >= m_size.frames || y >= m_size.rows || x >= m_size.columns){
        throw std::out_of_range("data index out of range");
    }
    return get(z, y, x);
}

reference ImageData::get(uint32_t z, uint32_t y, uint32_t x){
    return const_cast<reference>(const_cast<const ImageData*>(this)->get(z, y, x));
}

const_reference ImageData::get(uint32_t z, uint32_t y, uint32_t x) const{
    return m_data[(z * m_size.rows + y) * m_size.columns + x];
}

reference ImageData::get(uint32_t index){
    return const_cast<reference>(const_cast<const ImageData*>(this)->get(index));
}

const_reference ImageData::get(uint32_t index) const{
    return m_data[index];
}

pointer ImageData::data(){
    return const_cast<pointer>(const_cast<const ImageData*>(this)->data());
}

const_pointer ImageData::data() const{
    return m_data.data();
}

std::vector<value_type> ImageData::getData() const{
    return m_data;
}

Image2D<value_type> ImageData::getImage2D(uint32_t frame, ImagePlane imgPlane) const{
    Image2D<value_type> img2d;
    if(imgPlane == ImagePlane::Axial){  // YX
        if(frame >= m_size.frames){
            throw std::out_of_range("frame out of range (frame >= nr of frames)");
        }
        img2d.reserve(m_size.rows);
        for(uint32_t y = 0; y < m_size.rows; y++){
            img2d.emplace_back();
            const value_type* beginData = m_data.data() + (frame * m_size.rows + y) * m_size.columns;
            const value_type* endData = beginData + m_size.columns;
            img2d[y].insert(img2d[y].end(), beginData, endData);
        }
    }
    else if(imgPlane == ImagePlane::Coronal){  // ZX
        if(frame >= m_size.rows){
            throw std::out_of_range("frame out of range (frame >= nr of rows)");
        }
        img2d.reserve(m_size.frames);
        for(uint32_t z = 0; z < m_size.frames; z++){
            img2d.emplace_back();
            img2d[z].reserve(m_size.columns);
            for(uint32_t x = 0; x < m_size.columns; x++){
                img2d[z].emplace_back(get(z, frame, x));
            }
        }
    }
    else if(imgPlane == ImagePlane::Sagittal){  // ZY
        if(frame >= m_size.columns){
            throw std::out_of_range("frame out of range (frame >= nr of columns)");
        }
        img2d.reserve(m_size.frames);
        for(uint32_t z = 0; z < m_size.frames; z++){
            img2d.emplace_back();
            img2d[z].reserve(m_size.rows);
            for(uint32_t y = 0; y < m_size.rows; y++){
                img2d[z].emplace_back(get(z, y, frame));
            }
        }
    }
    return img2d;
}

Image3D<value_type> ImageData::getImage3D(ImagePlane imgPlane) const{
    Image3D<value_type> img3d;
    if(imgPlane == ImagePlane::Axial){  // ZYX
        img3d.reserve(m_size.frames);
        for(uint32_t z = 0; z < m_size.frames; z++){
            img3d.emplace_back(getImage2D(z, ImagePlane::Axial));
        }
    }
    else if(imgPlane == ImagePlane::Coronal){  // YZX
        img3d.reserve(m_size.rows);
        for(uint32_t y = 0; y < m_size.rows; y++){
            img3d.emplace_back(getImage2D(y, ImagePlane::Coronal));
        }
    }
    else if(imgPlane == ImagePlane::Sagittal){  // XZY
        img3d.reserve(m_size.columns);
        for(uint32_t x = 0; x < m_size.columns; x++){
            img3d.emplace_back(getImage2D(x, ImagePlane::Sagittal));
        }
    }
    return img3d;
}

ImageData ImageData::getImageData2D(uint32_t frame, ImagePlane imgPlane) const{
    DataOffset offset{};
    DataSpacing spacing{};

    if(imgPlane == ImagePlane::Axial){
        offset = {m_offset.frames + frame * m_spacing.frames, m_offset.rows, m_offset.columns};
        spacing = {0, m_spacing.rows, m_spacing.columns};
    }
    else if(imgPlane == ImagePlane::Coronal){
        offset = {m_offset.rows + frame * m_spacing.rows, m_offset.frames, m_offset.columns};
        spacing = {0, m_spacing.frames, m_spacing.columns};
    }
    else if(imgPlane == ImagePlane::Sagittal){
        offset = {m_offset.columns + frame * m_spacing.columns, m_offset.frames, m_offset.rows};
        spacing = {0, m_spacing.frames, m_spacing.rows};
    }

    return ImageData(getImage2D(frame, imgPlane), offset, spacing);
}

ImageData ImageData::getImageData3D(ImagePlane imgPlane) const{
    if(imgPlane == ImagePlane::Axial){
        return *this;
    }
    else{
        DataOffset offset{};
        DataSpacing spacing{};

        if(imgPlane == ImagePlane::Coronal){
            offset = {m_offset.rows, m_offset.frames, m_offset.columns};
            spacing = {m_spacing.rows, m_spacing.frames, m_spacing.columns};
        }
        else if(imgPlane == ImagePlane::Sagittal){
            offset = {m_offset.columns, m_offset.frames, m_offset.rows};
            spacing = {m_spacing.columns, m_spacing.frames, m_spacing.rows};
        }

        return ImageData(getImage3D(imgPlane), offset, spacing);
    }
}

value_type ImageData::min() const{
    return *std::min_element(m_data.begin(), m_data.end());
}

value_type ImageData::max() const{
    return *std::max_element(m_data.begin(), m_data.end());
}

value_type ImageData::sum() const{
    return std::accumulate(m_data.begin(), m_data.end(), value_type());
}

value_type ImageData::mean() const{
    return sum() / size();
}

value_type ImageData::var() const{
    const value_type meanV = mean();
    return std::accumulate(m_data.begin(), m_data.end(), value_type(), [&meanV](auto a, auto b){
        return a + (b - meanV) * (b - meanV);
    }) / size();
}

value_type ImageData::nanmin() const{
    value_type minV = std::numeric_limits<value_type>::infinity();
    for(const auto& el : m_data){
        if(!std::isnan(el) && el < minV){
            minV = el;
        }
    }
    return minV;
}

value_type ImageData::nanmax() const{
    value_type maxV = -std::numeric_limits<value_type>::infinity();
    for(const auto& el : m_data){
        if(!std::isnan(el) && el > maxV){
            maxV = el;
        }
    }
    return maxV;
}

value_type ImageData::nansum() const{
    return std::accumulate(m_data.begin(), m_data.end(), value_type(), [](auto a, auto b){ return !std::isnan(b) ? (a+b) : a; });
}

value_type ImageData::nanmean() const{
    return nansum() / nansize();
}

value_type ImageData::nanvar() const{
    const value_type meanVal = nanmean();
    return std::accumulate(m_data.begin(), m_data.end(), value_type(), [&meanVal](const auto& a, const auto& b){
        return !std::isnan(b) ? (a + (b - meanVal) * (b - meanVal)) : a;
    }) / nansize();
}

ImageData::size_type ImageData::nansize() const{
    return std::count_if(m_data.begin(), m_data.end(), [](const auto& el) { return !std::isnan(el); });
}

bool ImageData::containsNan() const{
    return std::find_if(m_data.begin(), m_data.end(), [](const auto& el) { return std::isnan(el); }) != m_data.end();
}

bool ImageData::containsInf() const{
    return std::find_if(m_data.begin(), m_data.end(), [](const auto& el) { return std::isinf(el); }) != m_data.end();
}

}
