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

namespace yagit{

namespace{
template <typename T>
const T MinValue{std::numeric_limits<T>::lowest()};
template <typename T>
const T MaxValue{std::numeric_limits<T>::max()};
}

template <typename T>
template <typename U>
ImageData<T>::ImageData(const std::vector<U>& data, const DataSize& size, const DataOffset& offset, const DataSpacing& spacing)
    : m_data(data.begin(), data.end()), m_size(size), m_offset(offset), m_spacing(spacing),
      m_min(MaxValue<T>), m_max(MinValue<T>) {
    if(data.size() != size.frames * size.rows * size.columns){
        throw std::invalid_argument("pixelData size is inconsistent with data size information");
    }
}

template <typename T>
template <typename U>
ImageData<T>::ImageData(const Image2D<U>& image2d, const DataOffset& offset, const DataSpacing& spacing)
    : m_offset(offset), m_spacing(spacing), m_min(MaxValue<T>), m_max(MinValue<T>) {
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

template <typename T>
template <typename U>
ImageData<T>::ImageData(const Image3D<U>& image3d, const DataOffset& offset, const DataSpacing& spacing)
    : m_offset(offset), m_spacing(spacing), m_min(MaxValue<T>), m_max(MinValue<T>) {
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

template <typename T>
DataSize ImageData<T>::getSize() const{
    return m_size;
}

template <typename T>
DataOffset ImageData<T>::getOffset() const{
    return m_offset;
}

template <typename T>
DataSpacing ImageData<T>::getSpacing() const{
    return m_spacing;
}

template <typename T>
void ImageData<T>::setSize(const DataSize& size){
    if(m_size.frames * m_size.rows * m_size.columns != size.frames * size.rows * size.columns){
        throw std::invalid_argument("");
    }
    m_size = size;
}

template <typename T>
void ImageData<T>::setOffset(const DataOffset& offset){
    m_offset = offset;
}

template <typename T>
void ImageData<T>::setSpacing(const DataSpacing& spacing){
    m_spacing = spacing;
}

template <typename T>
ImageData<T>::size_type ImageData<T>::size() const{
    return m_data.size();
}

template <typename T>
T& ImageData<T>::at(uint32_t z, uint32_t y, uint32_t x){
    return const_cast<T&>(const_cast<const ImageData*>(this)->at(z, y, x));
}

template <typename T>
const T& ImageData<T>::at(uint32_t z, uint32_t y, uint32_t x) const{
    if(z >= m_size.frames || y >= m_size.rows || x >= m_size.columns){
        throw std::out_of_range("Data index out of range");
    }
    return get(z, y, x);
}

template <typename T>
T& ImageData<T>::get(uint32_t z, uint32_t y, uint32_t x){
    return const_cast<T&>(const_cast<const ImageData*>(this)->get(z, y, x));
}

template <typename T>
const T& ImageData<T>::get(uint32_t z, uint32_t y, uint32_t x) const{
    // return m_data[z * (size.rows * size.columns) + y * size.columns + x];
    return m_data[(z * m_size.rows + y) * m_size.columns + x];
}

template <typename T>
T& ImageData<T>::get(uint32_t index){
    return const_cast<T&>(const_cast<const ImageData*>(this)->get(index));
}

template <typename T>
const T& ImageData<T>::get(uint32_t index) const{
    return m_data[index];
}

template <typename T>
std::vector<T> ImageData<T>::getData() const{
    return m_data;
}

template <typename T>
Image2D<T> ImageData<T>::getImage2D(uint32_t frame, ImagePlane imgPlane) const{
    Image2D<T> img2d;
    if(imgPlane == ImagePlane::Axial){
        if(frame >= m_size.frames){
            throw std::out_of_range("Frame out of range (frame >= nr of frames)");
        }
        img2d.reserve(m_size.rows);
        for(uint32_t y = 0; y < m_size.rows; y++){
            img2d.emplace_back();
            const T* beginData = m_data.data() + (frame * m_size.rows + y) * m_size.columns;
            const T* endData = beginData + m_size.rows;
            img2d[y].insert(img2d[y].end(), beginData, endData);
        }
    }
    else if(imgPlane == ImagePlane::Coronal){
        if(frame >= m_size.rows){
            throw std::out_of_range("Frame out of range (frame >= nr of rows)");
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
    else if(imgPlane == ImagePlane::Sagittal){
        if(frame >= m_size.columns){
            throw std::out_of_range("Frame out of range (frame >= nr of columns)");
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

template <typename T>
Image3D<T> ImageData<T>::getImage3D() const{
    Image3D<T> img3d;
    img3d.reserve(m_size.frames);
    for(uint32_t z = 0; z < m_size.frames; z++){
        img3d.emplace_back();
        img3d[z].reserve(m_size.rows);
        for(uint32_t y = 0; y < m_size.rows; y++){
            img3d[z].emplace_back();
            const T* beginData = m_data.data() + (z * m_size.rows + y) * m_size.columns;
            const T* endData = beginData + m_size.rows;
            img3d[z][y].insert(img3d[z][y].end(), beginData, endData);
        }
    }
    return img3d;
}

template <typename T>
ImageData<T> ImageData<T>::getImageData2D(uint32_t frame, ImagePlane imgPlane) const{
    DataOffset offset{};
    DataSpacing spacing{};

    if(imgPlane == ImagePlane::Axial){
        offset = {m_offset.framesOffset + frame * m_spacing.framesSpacing, m_offset.rowsOffset, m_offset.columnsOffset};
        spacing = {0, m_spacing.rowsSpacing, m_spacing.columnsSpacing};
    }
    else if(imgPlane == ImagePlane::Coronal){
        offset = {m_offset.framesOffset, m_offset.rowsOffset + frame * m_spacing.rowsSpacing, m_offset.columnsOffset};
        spacing = {0, m_spacing.framesSpacing, m_spacing.columnsSpacing};
    }
    else if(imgPlane == ImagePlane::Sagittal){
        offset = {m_offset.framesOffset, m_offset.rowsOffset, m_offset.columnsOffset + frame * m_spacing.columnsSpacing};
        spacing = {0, m_spacing.framesSpacing, m_spacing.rowsSpacing};
    }

    return ImageData<T>(getImage2D(frame, imgPlane), offset, spacing);
}

template <typename T>
T ImageData<T>::getMin() const{
    if(m_min == MaxValue<T>){
        m_min = *std::min_element(m_data.begin(), m_data.end());
    }
    return m_min;
}

template <typename T>
T ImageData<T>::getMax() const{
    if(m_max == MinValue<T>){
        m_max = *std::max_element(m_data.begin(), m_data.end());
    }
    return m_max;
}

}
