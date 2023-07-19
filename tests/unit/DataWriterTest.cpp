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

#include "yagit/DataWriter.hpp"

#include <fstream>
#include <cstdio>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using testing::ThrowsMessage, testing::HasSubstr;

namespace{
bool isBigEndian(){
    uint16_t i = 1;
    return static_cast<int>(*reinterpret_cast<unsigned char*>(&i)) == 0;
}

std::string floatToBytes(float number){
    const size_t bytes = sizeof(float);
    std::string result(bytes, '0');
    std::memcpy(result.data(), &number, bytes);
    return result;
}

std::string floatArrayToBytes(const float* numbers, size_t size){
    const size_t bytes = size * sizeof(float);
    std::string result(bytes, '0');
    std::memcpy(result.data(), numbers, bytes);
    return result;
}

std::string floatToString(float number){
    std::ostringstream oss;
    oss << number;
    return oss.str(); 
}
}

namespace{
const yagit::Image3D<float> IMAGE_3D = {
    {{1.0, 2.0}, {0.0, 1.0}},
    {{0.2, 1.0}, {-1.0, -0.1}},
    {{123432.12, 999.99}, {-543213.3, -443.9}}
};
const yagit::DataOffset DATA_OFFSET_3D{2.1, -3.2, 0.0};
const yagit::DataSpacing DATA_SPACING_3D{1.0, 2.5, 0.5};
const yagit::ImageData IMAGE_DATA_3D(IMAGE_3D, DATA_OFFSET_3D, DATA_SPACING_3D);

const std::string METAIMAGE_FILE = "image.mha";
const std::string IS_BIG_ENDIAN = (isBigEndian() ? "True" : "False");
const std::string IMAGE_METAIMAGE =
    "ObjectType = Image\n"
    "NDims = 3\n"
    "DimSize = " + floatToString(IMAGE_DATA_3D.getSize().columns) + " "
                 + floatToString(IMAGE_DATA_3D.getSize().rows) + " "
                 + floatToString(IMAGE_DATA_3D.getSize().frames) + "\n"
    "Offset = " + floatToString(DATA_OFFSET_3D.columns) + " "
                + floatToString(DATA_OFFSET_3D.rows) + " "
                + floatToString(DATA_OFFSET_3D.frames) + "\n"
    "ElementSpacing = " + floatToString(DATA_SPACING_3D.columns) + " "
                        + floatToString(DATA_SPACING_3D.rows) + " "
                        + floatToString(DATA_SPACING_3D.frames) + "\n"
    "Orientation = 1 0 0 0 1 0 0 0 1\n"
    "BinaryData = True\n"
    "BinaryDataByteOrderMSB = " + IS_BIG_ENDIAN + "\n"
    "CompressedData = False\n"
    "ElementType = MET_FLOAT\n"
    "ElementDataFile = LOCAL\n"
    + floatArrayToBytes(IMAGE_DATA_3D.data(), IMAGE_DATA_3D.size());
}

TEST(DataWriterTest, writeToMetaImageForImageData3DShouldSuccess){
    yagit::DataWriter::writeToMetaImage(IMAGE_DATA_3D, METAIMAGE_FILE);

    std::ifstream file(METAIMAGE_FILE, std::ios::binary);
    ASSERT_FALSE(file.fail());

    std::stringstream content;
    content << file.rdbuf();
    EXPECT_EQ(IMAGE_METAIMAGE, content.str());

    file.close();

    std::remove(METAIMAGE_FILE.c_str());
}

TEST(DataWriterTest, writeToMetaImageForIncorrectPathShouldThrow){
    const std::string NONEXISTENT_PATH = "nonexistent_directory/" + METAIMAGE_FILE;
    const auto writeToNonexistentPath = [&NONEXISTENT_PATH](){
        yagit::DataWriter::writeToMetaImage(IMAGE_DATA_3D, NONEXISTENT_PATH);
    };
    EXPECT_THAT(writeToNonexistentPath, ThrowsMessage<std::runtime_error>(HasSubstr("cannot open")));
}
