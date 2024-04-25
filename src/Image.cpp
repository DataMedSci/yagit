/********************************************************************************************
 * Copyright (C) 2024 'Yet Another Gamma Index Tool' Developers.
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

#include "yagit/Image.hpp"

#include <sstream>
#include <iomanip>

namespace yagit{

namespace{
std::string imageNDimToString(const std::vector<float>& vector, std::streamsize precision = -1, uint32_t = 0){
    std::ostringstream oss;
    if(precision > 0){
        oss << std::fixed << std::setprecision(precision);
    }
    oss << "[";
    for(size_t i = 0; i < vector.size() - 1; i++){
        oss << vector[i] << ", ";
    }
    oss << vector.back() << "]";
    return oss.str();
}

template <typename T>
std::string imageNDimToString(const std::vector<T>& vector, std::streamsize precision = -1, uint32_t level = 0){
    std::string spaces(level + 1, ' ');
    std::string res = "";
    res += "[";
    for(size_t i = 0; i < vector.size() - 1; i++){
        res += imageNDimToString(vector[i], precision, level + 1) + ",\n" + spaces;
    }
    res += imageNDimToString(vector.back(), precision, level + 1);
    res += "]";
    return res;
}
}

std::string image2DToString(const yagit::Image2D& img, std::streamsize precision){
    return imageNDimToString(img, precision);
}

std::string image3DToString(const yagit::Image3D& img, std::streamsize precision){
    return imageNDimToString(img, precision);
}

}
