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

#include "yagit/DataReader.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TestUtils.hpp"

using ::testing::ThrowsMessage, ::testing::HasSubstr;

namespace{
const yagit::Image3D IMAGE_3D = {
    {{1.0, 2.0}, {0.0, 1.0}},
    {{0.2, 1.0}, {-1.0, -0.1}},
    {{123432.12, 999.99}, {-543213.3, -443.9}}
};

const yagit::DataOffset DATA_OFFSET{2.1, -3.2, 0.0};
const yagit::DataSpacing DATA_SPACING{1.0, 2.5, 0.5};

const yagit::ImageData IMAGE_DATA(IMAGE_3D, DATA_OFFSET, DATA_SPACING);

const std::string DATA_DIR = "data/";
const std::string DICOM_RTDOSE_FILE = DATA_DIR + "test_dicom_rtdose.dcm";
const std::string DICOM_RTDOSE_FILE_BIG_ENDIAN = DATA_DIR + "test_dicom_rtdose_big_endian.dcm";
const std::string DICOM_RTDOSE_FILE_16_BIT = DATA_DIR + "test_dicom_rtdose_16bit.dcm";
const std::string DICOM_CT_FILE = DATA_DIR + "test_dicom_ct.dcm";
const std::string METAIMAGE_FILE = DATA_DIR + "test_metaimage.mha";
const std::string METAIMAGE_FILE_UNFORMATTED = DATA_DIR + "test_metaimage_unformatted.mha";
const std::string METAIMAGE_FILE_BIG_ENDIAN = DATA_DIR + "test_metaimage_big_endian.mha";
const std::string METAIMAGE_FILE_INT = DATA_DIR + "test_metaimage_int.mha";
const std::string NONEXISTENT_FILE = DATA_DIR + "nonexistent_file";
}

class DataReaderRTDoseTest : public ::testing::Test{
public:
    void SetUp() override{
        const std::vector<uint32_t> rawData{
            8191, 16383, 0, 8191, 2047, 8191, 10919, 819, 1011155926, 8191918, 4294967295, 3636428
        };
        const yagit::DataSize dataSize{3, 2, 2};
        const yagit::DataOffset dataOffset{2.1, -3.2, 0.0};
        const yagit::DataSpacing dataSpacing{1.0, 2.5, 0.5};
        const double doseGridScaling = 0.0001220703125284217;

        std::vector<float> data;
        for(const auto& el : rawData){
            data.push_back(static_cast<float>(doseGridScaling * static_cast<double>(el)));
        }
        expectedImageData = yagit::ImageData(std::move(data), dataSize, dataOffset, dataSpacing);
    }

    yagit::ImageData expectedImageData;
};

TEST_F(DataReaderRTDoseTest, readRTDoseDicom){
    yagit::ImageData imageData;
    ASSERT_NO_THROW(imageData = yagit::DataReader::readRTDoseDicom(DICOM_RTDOSE_FILE, true));
    EXPECT_THAT(imageData, matchImageData(expectedImageData));
}

TEST_F(DataReaderRTDoseTest, readRTDoseDicomBigEndian){
    yagit::ImageData imageData;
    ASSERT_NO_THROW(imageData = yagit::DataReader::readRTDoseDicom(DICOM_RTDOSE_FILE_BIG_ENDIAN, true));
    EXPECT_THAT(imageData, matchImageData(expectedImageData));
}

TEST(DataReaderTest, readRTDoseDicom16bit){
    const std::vector<uint32_t> rawData{4, 3213, 0, 31, 67, 177, 28581, 14540, 29548, 63999, 65535, 22125};
    const yagit::DataSize dataSize{3, 2, 2};
    const yagit::DataOffset dataOffset{2.1, -3.2, 0.0};
    const yagit::DataSpacing dataSpacing{1.0, 2.5, 0.5};
    const double doseGridScaling = 0.03125047684443427;

    std::vector<float> data;
    for(const auto& el : rawData){
        data.push_back(static_cast<float>(doseGridScaling * static_cast<double>(el)));
    }

    const yagit::ImageData expectedImageData(std::move(data), dataSize, dataOffset, dataSpacing);

    yagit::ImageData imageData;
    ASSERT_NO_THROW(imageData = yagit::DataReader::readRTDoseDicom(DICOM_RTDOSE_FILE_16_BIT, true));
    EXPECT_THAT(imageData, matchImageData(expectedImageData));
}

TEST(DataReaderTest, readCTDicom){
    const auto readCTDicom = [](){ yagit::DataReader::readRTDoseDicom(DICOM_CT_FILE); };
    EXPECT_THAT(readCTDicom, ThrowsMessage<std::runtime_error>(HasSubstr("SOP Class UID")));
}

TEST(DataReaderTest, readRTDoseDicomForNonexistentFileShouldThrow){
    const auto readNonexistentDicom = [](){ yagit::DataReader::readRTDoseDicom(NONEXISTENT_FILE); };
    EXPECT_THAT(readNonexistentDicom, ThrowsMessage<std::runtime_error>(HasSubstr("cannot read")));
}

TEST(DataReaderTest, readMetaImage){
    yagit::ImageData imageData;
    ASSERT_NO_THROW(imageData = yagit::DataReader::readMetaImage(METAIMAGE_FILE, true));
    EXPECT_THAT(imageData, matchImageData(IMAGE_DATA));
}

TEST(DataReaderTest, readMetaImageUnformatted){
    yagit::ImageData imageData;
    ASSERT_NO_THROW(imageData = yagit::DataReader::readMetaImage(METAIMAGE_FILE_UNFORMATTED, true));
    EXPECT_THAT(imageData, matchImageData(IMAGE_DATA));
}

TEST(DataReaderTest, readMetaImageBigEndian){
    yagit::ImageData imageData;
    ASSERT_NO_THROW(imageData = yagit::DataReader::readMetaImage(METAIMAGE_FILE_BIG_ENDIAN, true));
    EXPECT_THAT(imageData, matchImageData(IMAGE_DATA));
}

TEST(DataReaderTest, readMetaImageWithIntTypeData){
    const yagit::Image3D image3DInt = {
        {{1, 6}, {13, 2}},
        {{0, -1}, {-10, 0}},
        {{123432, 999}, {-543213, -443}}
    };
    const yagit::ImageData imageDataInt(image3DInt, DATA_OFFSET, DATA_SPACING);

    yagit::ImageData imageData;
    ASSERT_NO_THROW(imageData = yagit::DataReader::readMetaImage(METAIMAGE_FILE_INT, true));
    EXPECT_THAT(imageData, matchImageData(imageDataInt));
}

TEST(DataReaderTest, readMetaImageForNonexistentFileShouldThrow){
    const auto readNonexistentMetaImage = [](){ yagit::DataReader::readMetaImage(NONEXISTENT_FILE); };
    EXPECT_THAT(readNonexistentMetaImage, ThrowsMessage<std::runtime_error>(HasSubstr("cannot read")));
}
