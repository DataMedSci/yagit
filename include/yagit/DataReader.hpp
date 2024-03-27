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

#include <string>

#include "yagit/ImageData.hpp"

namespace yagit::DataReader{

/**
 * @brief Read RT Dose DICOM file and retrieve dose image from it
 * @param filepath File path to RT Dose DICOM (.dcm)
 * @param displayInfo Whether to show additional information read from the file
 * @return Image containing dose values and image info (size, offset, spacing) retrieved from DICOM file
 */
ImageData readRTDoseDicom(const std::string& filepath, bool displayInfo = false);

/**
 * @brief Read MetaImage file and retrieve image from it
 * @param filepath File path to MetaImage file (.mha)
 * @param displayInfo Whether to show additional information read from the file
 * @return Image and its info (size, offset, spacing) retrieved from MetaImage file
 */
ImageData readMetaImage(const std::string& filepath, bool displayInfo = false);

}
