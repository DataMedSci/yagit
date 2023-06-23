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

#include "yagit/DataWriter.hpp"

#include <fstream>

#include <gdcmByteSwap.h>

namespace yagit::DataWriter{

void writeToMetaImage(const ImageData& img, const std::string& filepath){
    std::ofstream file(filepath, std::ios::binary);
    if(!file){
        throw std::runtime_error("cannot open " + filepath + " file");
    }

    DataSize size = img.getSize();
    DataOffset offset = img.getOffset();
    DataSpacing spacing = img.getSpacing();
    // image with spacing equal to 0 is problematic to show in MetaImage viewer programs
    // in yagit, 2D images have frames spacing equal to 0
    if(spacing.frames == 0){
        spacing.frames = 1;
    }

    std::string isBigEndian = (gdcm::ByteSwap<uint16_t>::SystemIsBigEndian() ? "True" : "False");

    std::string elementType;
    if constexpr(std::is_same_v<ImageData::value_type, float>){
        elementType = "MET_FLOAT";
    }
    else if constexpr(std::is_same_v<ImageData::value_type, double>){
        elementType = "MET_DOUBLE";
    }
    else{
        throw std::logic_error("not supported data type");
    }

    file << "ObjectType = Image\n"
         << "NDims = 3\n"
         << "DimSize = " << size.columns << " " << size.rows << " " << size.frames << "\n"
         << "Offset = " << offset.columns << " " << offset.rows << " " << offset.frames << "\n"
         << "ElementSpacing = " << spacing.columns << " " << spacing.rows << " " << spacing.frames << "\n"
         << "Orientation = 1 0 0 0 1 0 0 0 1\n"  // TODO: add support for different orientations
         << "BinaryData = True\n"
         << "BinaryDataByteOrderMSB = " << isBigEndian << "\n"
         << "CompressedData = False\n"
         << "ElementType = " << elementType << "\n"
         << "ElementDataFile = LOCAL\n";

    std::streamsize bytes = img.size() * sizeof(ImageData::value_type);
    file.write(reinterpret_cast<const char*>(img.data()), bytes);

    file.close();
}

}
