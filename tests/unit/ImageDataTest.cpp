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

#include "yagit/ImageData.hpp"

#include <limits>
#include <cmath>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TestUtils.hpp"

using testing::IsNan, testing::ThrowsMessage, testing::HasSubstr;

namespace{
const float NaN = std::numeric_limits<float>::quiet_NaN();
const float INF = std::numeric_limits<float>::infinity();

const std::vector<float>& DATA{1.5, 2.3, 4.4, 0.1, -0.3, 0.0, -2.5, 153.0, -200.4, 12.9, 9.0, 0.0};
const yagit::Image2D IMAGE_2D = {
    {DATA[0], DATA[1], DATA[2], DATA[3]},
    {DATA[4], DATA[5], DATA[6], DATA[7]},
    {DATA[8], DATA[9], DATA[10], DATA[11]}
};
const yagit::Image3D IMAGE_3D = {
    {
        {DATA[0], DATA[1], DATA[2]},
        {DATA[3], DATA[4], DATA[5]}
    },
    {
        {DATA[6], DATA[7], DATA[8]},
        {DATA[9], DATA[10], DATA[11]}
    }
};

const std::vector<float> DATA_SMALL{1.3, 20.4, -13.5, 0.0};
const std::vector<float> DATA_SMALL_WITH_NANS{NaN, 1.3, 20.4, NaN, -13.5, 0.0};
const std::vector<float> DATA_SMALL_WITH_INFS{INF, 1.3, 20.4, INF, -13.5, 0.0};
const std::vector<float> DATA_SMALL_WITH_INFS2{INF, 1.3, 20.4, -INF, -13.5, 0.0};

const yagit::DataSize DATA_SIZE{2, 3, 2};
const yagit::DataSize DATA_SIZE_2D{static_cast<uint32_t>(1),
                                   static_cast<uint32_t>(IMAGE_2D.size()),
                                   static_cast<uint32_t>(IMAGE_2D.at(0).size())};
const yagit::DataSize DATA_SIZE_3D{static_cast<uint32_t>(IMAGE_3D.size()),
                                   static_cast<uint32_t>(IMAGE_3D.at(0).size()),
                                   static_cast<uint32_t>(IMAGE_3D.at(0).at(0).size())};
const yagit::DataOffset DATA_OFFSET{0.1, -1.2, 0.0};
const yagit::DataSpacing DATA_SPACING{1.0, 2.0, 2.5};

const yagit::ImageData IMAGE_DATA(DATA, DATA_SIZE, DATA_OFFSET, DATA_SPACING);
const yagit::ImageData IMAGE_2D_DATA(IMAGE_2D, DATA_OFFSET, DATA_SPACING);
const yagit::ImageData IMAGE_3D_DATA(IMAGE_3D, DATA_OFFSET, DATA_SPACING);
const yagit::ImageData IMAGE_DATA_SMALL(DATA_SMALL, {1, 2, 2}, DATA_OFFSET, DATA_SPACING);
const yagit::ImageData IMAGE_DATA_SMALL_WITH_NANS(DATA_SMALL_WITH_NANS, {1, 2, 3}, DATA_OFFSET, DATA_SPACING);
const yagit::ImageData IMAGE_DATA_SMALL_WITH_INFS(DATA_SMALL_WITH_INFS, {1, 2, 3}, DATA_OFFSET, DATA_SPACING);
const yagit::ImageData IMAGE_DATA_SMALL_WITH_INFS2(DATA_SMALL_WITH_INFS2, {1, 2, 3}, DATA_OFFSET, DATA_SPACING);
const yagit::ImageData EMPTY_IMAGE_DATA(std::vector<float>{}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0});
const yagit::ImageData EMPTY_IMAGE_DATA2(std::vector<float>{}, {0, 0, 0}, DATA_OFFSET, DATA_SPACING);
}

TEST(ImageDataTest, defaultConstructor){
    yagit::ImageData imageData;
    EXPECT_THAT(imageData, matchImageData(std::vector<float>{}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}));
}

TEST(ImageDataTest, dataConstructor){
    yagit::ImageData imageData(DATA, DATA_SIZE, DATA_OFFSET, DATA_SPACING);
    EXPECT_THAT(imageData, matchImageData(DATA, DATA_SIZE, DATA_OFFSET, DATA_SPACING));
}

TEST(ImageDataTest, dataConstructorForInconsistentSizeShouldThrow){
    const auto dataConstructor = [](){ yagit::ImageData(DATA, yagit::DataSize{10, 10, 10}, DATA_OFFSET, DATA_SPACING); };
    EXPECT_THAT(dataConstructor, ThrowsMessage<std::invalid_argument>("size is inconsistent with data size information"));
}

TEST(ImageDataTest, image2DConstructor){
    yagit::ImageData imageData(IMAGE_2D, DATA_OFFSET, DATA_SPACING);
    EXPECT_THAT(imageData, matchImageData(DATA, DATA_SIZE_2D, DATA_OFFSET, DATA_SPACING));
}

TEST(ImageDataTest, image2DConstructorForInconsistentSizeShouldThrow){
    const yagit::Image2D inconsistentImage2D = {
        {1, 2, 3},
        {1, 2}
    };
    const auto image2DConstructor = [&inconsistentImage2D](){ yagit::ImageData(inconsistentImage2D, DATA_OFFSET, DATA_SPACING); };
    EXPECT_THAT(image2DConstructor, ThrowsMessage<std::invalid_argument>("inner vectors don't have the same size"));
}

TEST(ImageDataTest, image2DConstructorForEmptyImages){
    EXPECT_THAT(yagit::ImageData(yagit::Image2D{}, DATA_OFFSET, DATA_SPACING), matchImageData(EMPTY_IMAGE_DATA2));
    EXPECT_THAT(yagit::ImageData(yagit::Image2D{{}}, DATA_OFFSET, DATA_SPACING), matchImageData(EMPTY_IMAGE_DATA2));
    EXPECT_THAT(yagit::ImageData(yagit::Image2D{{}, {}}, DATA_OFFSET, DATA_SPACING), matchImageData(EMPTY_IMAGE_DATA2));
}

TEST(ImageDataTest, image2DConstructorForEmptyAndInconsistentSizeShouldThrow){
    const auto image2DConstructor = [](){ yagit::ImageData(yagit::Image2D{{}, {1}}, DATA_OFFSET, DATA_SPACING); };
    EXPECT_THAT(image2DConstructor, ThrowsMessage<std::invalid_argument>("inner vectors don't have the same size"));
}

TEST(ImageDataTest, image3DConstructor){
    yagit::ImageData imageData(IMAGE_3D, DATA_OFFSET, DATA_SPACING);
    EXPECT_THAT(imageData, matchImageData(DATA, DATA_SIZE_3D, DATA_OFFSET, DATA_SPACING));
}

TEST(ImageDataTest, image3DConstructorForInconsistentRowsShouldThrow){
    const yagit::Image3D inconsistentImage3D = {
        {
            {1, 2}
        },
        {
            {1, 2},
            {1, 2}
        }
    };
    const auto image3DConstructor = [&inconsistentImage3D](){ yagit::ImageData(inconsistentImage3D, DATA_OFFSET, DATA_SPACING); };
    EXPECT_THAT(image3DConstructor, ThrowsMessage<std::invalid_argument>("singly nested vectors don't have the same size"));
}

TEST(ImageDataTest, image3DConstructorForInconsistentColumnsShouldThrow){
    const yagit::Image3D inconsistentImage3D = {
        {
            {1, 2, 3},
            {1, 2}
        },
        {
            {1, 2, 3},
            {1, 2, 3}
        }
    };
    const auto image3DConstructor = [&inconsistentImage3D](){ yagit::ImageData(inconsistentImage3D, DATA_OFFSET, DATA_SPACING); };
    EXPECT_THAT(image3DConstructor, ThrowsMessage<std::invalid_argument>("double nested vectors don't have the same size"));
}

TEST(ImageDataTest, image3DConstructorForEmptyImages){
    EXPECT_THAT(yagit::ImageData(yagit::Image3D{}, DATA_OFFSET, DATA_SPACING), matchImageData(EMPTY_IMAGE_DATA2));
    EXPECT_THAT(yagit::ImageData(yagit::Image3D{{}}, DATA_OFFSET, DATA_SPACING), matchImageData(EMPTY_IMAGE_DATA2));
    EXPECT_THAT(yagit::ImageData(yagit::Image3D{{}, {}}, DATA_OFFSET, DATA_SPACING), matchImageData(EMPTY_IMAGE_DATA2));
    EXPECT_THAT(yagit::ImageData(yagit::Image3D{{{}}}, DATA_OFFSET, DATA_SPACING), matchImageData(EMPTY_IMAGE_DATA2));
    EXPECT_THAT(yagit::ImageData(yagit::Image3D{{{}, {}}}, DATA_OFFSET, DATA_SPACING), matchImageData(EMPTY_IMAGE_DATA2));
    EXPECT_THAT(yagit::ImageData(yagit::Image3D{{{}}, {{}}}, DATA_OFFSET, DATA_SPACING), matchImageData(EMPTY_IMAGE_DATA2));
}

TEST(ImageDataTest, image3DConstructorForEmptyAndInconsistentSizeShouldThrow){
    const auto image3DConstructor2 = [](){ yagit::ImageData(yagit::Image3D{{}, {{1}}}, DATA_OFFSET, DATA_SPACING); };
    EXPECT_THAT(image3DConstructor2, ThrowsMessage<std::invalid_argument>("singly nested vectors don't have the same size"));

    const auto image3DConstructor1 = [](){ yagit::ImageData(yagit::Image3D{{{}, {1}}}, DATA_OFFSET, DATA_SPACING); };
    EXPECT_THAT(image3DConstructor1, ThrowsMessage<std::invalid_argument>("double nested vectors don't have the same size"));
}

TEST(ImageDataTest, moveDataConstructor){
    std::vector<float> data(DATA);

    yagit::ImageData imageData(std::move(data), DATA_SIZE, DATA_OFFSET, DATA_SPACING);

    EXPECT_EQ(std::vector<float>{}, data);
    EXPECT_THAT(imageData, matchImageData(DATA, DATA_SIZE, DATA_OFFSET, DATA_SPACING));
}

TEST(ImageDataTest, copyConstructor){
    yagit::ImageData imageData2(IMAGE_DATA);

    EXPECT_THAT(imageData2, matchImageData(IMAGE_DATA));
}

TEST(ImageDataTest, copyAssignmentOperator){
    yagit::ImageData imageData2, imageData3;

    imageData3 = imageData2 = IMAGE_DATA;

    EXPECT_THAT(imageData2, matchImageData(IMAGE_DATA));
    EXPECT_THAT(imageData3, matchImageData(IMAGE_DATA));
}

TEST(ImageDataTest, moveConstructor){
    yagit::ImageData imageData1(IMAGE_DATA);
    yagit::ImageData imageData2(std::move(imageData1));

    EXPECT_THAT(imageData1, matchImageData(EMPTY_IMAGE_DATA));
    EXPECT_THAT(imageData2, matchImageData(IMAGE_DATA));
}

TEST(ImageDataTest, moveAssignmentOperator){
    yagit::ImageData imageData1(IMAGE_DATA);
    yagit::ImageData imageData2;

    imageData2 = std::move(imageData1);

    EXPECT_THAT(imageData1, matchImageData(EMPTY_IMAGE_DATA));
    EXPECT_THAT(imageData2, matchImageData(IMAGE_DATA));
}

TEST(ImageDataTest, operatorEq){
    const yagit::ImageData imageData1(IMAGE_DATA);
    const yagit::ImageData imageData2(IMAGE_DATA);
    const yagit::ImageData imageData3({1.0, 2.0}, {1, 1, 2}, DATA_OFFSET, DATA_SPACING);

    EXPECT_TRUE(imageData1 == imageData2);
    EXPECT_FALSE(imageData1 == imageData3);

    EXPECT_TRUE(IMAGE_DATA_SMALL_WITH_NANS == IMAGE_DATA_SMALL_WITH_NANS);
    EXPECT_TRUE(IMAGE_DATA_SMALL_WITH_INFS == IMAGE_DATA_SMALL_WITH_INFS);
    EXPECT_TRUE(IMAGE_DATA_SMALL_WITH_INFS2 == IMAGE_DATA_SMALL_WITH_INFS2);
}

TEST(ImageDataTest, operatorNeq){
    const yagit::ImageData imageData1(IMAGE_DATA);
    const yagit::ImageData imageData2(IMAGE_DATA);
    const yagit::ImageData imageData3({1.0, 2.0}, {1, 1, 2}, DATA_OFFSET, DATA_SPACING);

    EXPECT_FALSE(imageData1 != imageData2);
    EXPECT_TRUE(imageData1 != imageData3);

    EXPECT_FALSE(IMAGE_DATA_SMALL_WITH_NANS != IMAGE_DATA_SMALL_WITH_NANS);
    EXPECT_FALSE(IMAGE_DATA_SMALL_WITH_INFS != IMAGE_DATA_SMALL_WITH_INFS);
    EXPECT_FALSE(IMAGE_DATA_SMALL_WITH_INFS2 != IMAGE_DATA_SMALL_WITH_INFS2);
}

TEST(ImageDataTest, setAndGetSize){
    yagit::ImageData imageData(IMAGE_DATA);
    EXPECT_THAT(imageData.getSize(), matchDataSize(DATA_SIZE));

    const yagit::DataSize newSize{3, 2, 2};
    imageData.setSize(newSize);
    EXPECT_THAT(imageData.getSize(), matchDataSize(newSize));
}

TEST(ImageDataTest, setSizeForInconsistentSizeShouldThrow){
    yagit::ImageData imageData(IMAGE_DATA);
    const auto setSize = [&imageData](){ imageData.setSize({10, 10, 10}); };
    EXPECT_THAT(setSize, ThrowsMessage<std::invalid_argument>(
        "the total number of elements in the new size does not match the total number of elements in the old size"));
}

TEST(ImageDataTest, setAndGetOffset){
    yagit::ImageData imageData(IMAGE_DATA);
    EXPECT_THAT(imageData.getOffset(), matchDataOffset(DATA_OFFSET));

    const yagit::DataOffset newOffset{5.2, 3.1, -2.2};
    imageData.setOffset(newOffset);
    EXPECT_THAT(imageData.getOffset(), matchDataOffset(newOffset));
}

TEST(ImageDataTest, setAndGetSpacing){
    yagit::ImageData imageData(IMAGE_DATA);
    EXPECT_THAT(imageData.getSpacing(), matchDataSpacing(DATA_SPACING));

    const yagit::DataSpacing newSpacing{2.0, 1.5, 3.0};
    imageData.setSpacing(newSpacing);
    EXPECT_THAT(imageData.getSpacing(), matchDataSpacing(newSpacing));
}

TEST(ImageDataTest, size){
    EXPECT_EQ(DATA.size(), IMAGE_DATA.size());

    yagit::ImageData imageData2({}, {0, 0, 0}, DATA_OFFSET, DATA_SPACING);
    EXPECT_EQ(0, imageData2.size());

    yagit::ImageData imageData3({2.0}, {1, 1, 1}, DATA_OFFSET, DATA_SPACING);
    EXPECT_EQ(1, imageData3.size());
}

TEST(ImageDataTest, atAndGet){
    int index = 0;
    for(uint32_t k = 0; k < IMAGE_DATA.getSize().frames; k++){
        for(uint32_t j = 0; j < IMAGE_DATA.getSize().rows; j++){
            for(uint32_t i = 0; i < IMAGE_DATA.getSize().columns; i++){
                EXPECT_EQ(DATA[index], IMAGE_DATA.at(k, j, i));
                EXPECT_EQ(DATA[index], IMAGE_DATA.get(k, j, i));
                index++;
            }
        }
    }
}

TEST(ImageDataTest, atOutOfRange){
    uint32_t frame = IMAGE_DATA.getSize().frames - 1;
    uint32_t row = IMAGE_DATA.getSize().rows - 1;
    uint32_t column = IMAGE_DATA.getSize().columns;
    EXPECT_THROW(IMAGE_DATA.at(frame, row, column), std::out_of_range);

    EXPECT_THROW(IMAGE_DATA.at(100, 100, 100), std::out_of_range);
}

TEST(ImageDataTest, setDataUsingGet){
    yagit::ImageData imageData(IMAGE_DATA);
    EXPECT_EQ(DATA[4], imageData.get(0, 2, 0));

    const float newValue = 99.0;
    imageData.get(0, 2, 0) = newValue;
    EXPECT_EQ(newValue, imageData.get(0, 2, 0));
}

TEST(ImageDataTest, getWithIndex){
    for(size_t i = 0; i < DATA.size(); i++){
        EXPECT_EQ(DATA[i], IMAGE_DATA.get(i));
    }
}

TEST(ImageDataTest, setDataUsingGetWithIndex){
    yagit::ImageData imageData(IMAGE_DATA);
    EXPECT_EQ(DATA[4], imageData.get(4));

    const float newValue = 99.0;
    imageData.get(4) = newValue;
    EXPECT_EQ(newValue, imageData.get(4));
}

TEST(ImageDataTest, data){
    std::vector<float> data(DATA);
    const float* dataPtr = data.data();
    const yagit::ImageData imageData(std::move(data), DATA_SIZE, DATA_OFFSET, DATA_SPACING);

    EXPECT_EQ(dataPtr, imageData.data());
}

TEST(ImageDataTest, getData){
    EXPECT_EQ(DATA, IMAGE_DATA.getData());
}

TEST(ImageDataTest, getImage2DAxial){
    EXPECT_EQ(IMAGE_3D[0], IMAGE_3D_DATA.getImage2D(0, yagit::ImagePlane::Axial));
    EXPECT_EQ(IMAGE_3D[1], IMAGE_3D_DATA.getImage2D(1, yagit::ImagePlane::Axial));
}

TEST(ImageDataTest, getImage2DCoronal){
    yagit::Image2D expectedCoronalImg0 = {
        {DATA[0], DATA[1], DATA[2]},
        {DATA[6], DATA[7], DATA[8]}
    };
    EXPECT_EQ(expectedCoronalImg0, IMAGE_3D_DATA.getImage2D(0, yagit::ImagePlane::Coronal));

    yagit::Image2D expectedCoronalImg1 = {
        {DATA[3], DATA[4], DATA[5]},
        {DATA[9], DATA[10], DATA[11]}
    };
    EXPECT_EQ(expectedCoronalImg1, IMAGE_3D_DATA.getImage2D(1, yagit::ImagePlane::Coronal));
}

TEST(ImageDataTest, getImage2DSagittal){
    yagit::Image2D expectedSagittalImg0 = {
        {DATA[0], DATA[3]},
        {DATA[6], DATA[9]}
    };
    EXPECT_EQ(expectedSagittalImg0, IMAGE_3D_DATA.getImage2D(0, yagit::ImagePlane::Sagittal));

    yagit::Image2D expectedSagittalImg1 = {
        {DATA[1], DATA[4]},
        {DATA[7], DATA[10]}
    };
    EXPECT_EQ(expectedSagittalImg1, IMAGE_3D_DATA.getImage2D(1, yagit::ImagePlane::Sagittal));
}

TEST(ImageDataTest, getImage3DAxial){
    EXPECT_EQ(IMAGE_3D, IMAGE_3D_DATA.getImage3D(yagit::ImagePlane::Axial));
}

TEST(ImageDataTest, getImage3DCoronal){
    yagit::Image3D expectedCoronalImg = {
        {
            {DATA[0], DATA[1], DATA[2]},
            {DATA[6], DATA[7], DATA[8]}
        },
        {
            {DATA[3], DATA[4], DATA[5]},
            {DATA[9], DATA[10], DATA[11]}
        }
    };
    EXPECT_EQ(expectedCoronalImg, IMAGE_3D_DATA.getImage3D(yagit::ImagePlane::Coronal));
}

TEST(ImageDataTest, getImage3DSagittal){
    yagit::Image3D expectedSagittalImg = {
        {
            {DATA[0], DATA[3]},
            {DATA[6], DATA[9]}
        },
        {
            {DATA[1], DATA[4]},
            {DATA[7], DATA[10]}
        },
        {
            {DATA[2], DATA[5]},
            {DATA[8], DATA[11]}
        }
    };
    EXPECT_EQ(expectedSagittalImg, IMAGE_3D_DATA.getImage3D(yagit::ImagePlane::Sagittal));
}

TEST(ImageDataTest, getImageData2DAxial){
    const yagit::ImageData imageData2D = IMAGE_3D_DATA.getImageData2D(1, yagit::ImagePlane::Axial);

    yagit::DataSize newSize{1, DATA_SIZE_3D.rows, DATA_SIZE_3D.columns};
    yagit::DataOffset newOffset{DATA_OFFSET.frames + DATA_SPACING.frames, DATA_OFFSET.rows, DATA_OFFSET.columns};
    yagit::DataSpacing newSpacing{0, DATA_SPACING.rows, DATA_SPACING.columns};

    EXPECT_THAT(imageData2D.getSize(), matchDataSize(newSize));
    EXPECT_THAT(imageData2D.getOffset(), matchDataOffset(newOffset));
    EXPECT_THAT(imageData2D.getSpacing(), matchDataSpacing(newSpacing));
}

TEST(ImageDataTest, getImageData2DCoronal){
    const yagit::ImageData imageData2D = IMAGE_3D_DATA.getImageData2D(1, yagit::ImagePlane::Coronal);

    yagit::DataSize newSize{1, DATA_SIZE_3D.frames, DATA_SIZE_3D.columns};
    yagit::DataOffset newOffset{DATA_OFFSET.rows + DATA_SPACING.rows, DATA_OFFSET.frames, DATA_OFFSET.columns};
    yagit::DataSpacing newSpacing{0, DATA_SPACING.frames, DATA_SPACING.columns};

    EXPECT_THAT(imageData2D.getSize(), matchDataSize(newSize));
    EXPECT_THAT(imageData2D.getOffset(), matchDataOffset(newOffset));
    EXPECT_THAT(imageData2D.getSpacing(), matchDataSpacing(newSpacing));
}

TEST(ImageDataTest, getImageData2DSagittal){
    const yagit::ImageData imageData2D = IMAGE_3D_DATA.getImageData2D(1, yagit::ImagePlane::Sagittal);

    yagit::DataSize newSize{1, DATA_SIZE_3D.frames, DATA_SIZE_3D.rows};
    yagit::DataOffset newOffset{DATA_OFFSET.columns + DATA_SPACING.columns, DATA_OFFSET.frames, DATA_OFFSET.rows};
    yagit::DataSpacing newSpacing{0, DATA_SPACING.frames, DATA_SPACING.rows};

    EXPECT_THAT(imageData2D.getSize(), matchDataSize(newSize));
    EXPECT_THAT(imageData2D.getOffset(), matchDataOffset(newOffset));
    EXPECT_THAT(imageData2D.getSpacing(), matchDataSpacing(newSpacing));
}

TEST(ImageDataTest, getImageData3DAxial){
    const yagit::ImageData imageData3D = IMAGE_3D_DATA.getImageData3D(yagit::ImagePlane::Axial);
    EXPECT_THAT(imageData3D, matchImageData(IMAGE_3D_DATA));
}

TEST(ImageDataTest, getImageData3DCoronal){
    const yagit::ImageData imageData3D = IMAGE_3D_DATA.getImageData3D(yagit::ImagePlane::Coronal);

    yagit::DataSize newSize{DATA_SIZE_3D.rows, DATA_SIZE_3D.frames, DATA_SIZE_3D.columns};
    yagit::DataOffset newOffset{DATA_OFFSET.rows, DATA_OFFSET.frames, DATA_OFFSET.columns};
    yagit::DataSpacing newSpacing{DATA_SPACING.rows, DATA_SPACING.frames, DATA_SPACING.columns};

    EXPECT_THAT(imageData3D.getSize(), matchDataSize(newSize));
    EXPECT_THAT(imageData3D.getOffset(), matchDataOffset(newOffset));
    EXPECT_THAT(imageData3D.getSpacing(), matchDataSpacing(newSpacing));
}

TEST(ImageDataTest, getImageData3DSagittal){
    const yagit::ImageData imageData3D = IMAGE_3D_DATA.getImageData3D(yagit::ImagePlane::Sagittal);

    yagit::DataSize newSize{DATA_SIZE_3D.columns, DATA_SIZE_3D.frames, DATA_SIZE_3D.rows};
    yagit::DataOffset newOffset{DATA_OFFSET.columns, DATA_OFFSET.frames, DATA_OFFSET.rows};
    yagit::DataSpacing newSpacing{DATA_SPACING.columns, DATA_SPACING.frames, DATA_SPACING.rows};

    EXPECT_THAT(imageData3D.getSize(), matchDataSize(newSize));
    EXPECT_THAT(imageData3D.getOffset(), matchDataOffset(newOffset));
    EXPECT_THAT(imageData3D.getSpacing(), matchDataSpacing(newSpacing));
}

TEST(ImageDataTest, min){
    EXPECT_FLOAT_EQ(-13.5, IMAGE_DATA_SMALL.min());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_NANS.min(), IsNan());
    EXPECT_FLOAT_EQ(-13.5, IMAGE_DATA_SMALL_WITH_INFS.min());
    EXPECT_FLOAT_EQ(-INF, IMAGE_DATA_SMALL_WITH_INFS2.min());
}

TEST(ImageDataTest, max){
    EXPECT_FLOAT_EQ(20.4, IMAGE_DATA_SMALL.max());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_NANS.max(), IsNan());
    EXPECT_FLOAT_EQ(INF, IMAGE_DATA_SMALL_WITH_INFS.max());
    EXPECT_FLOAT_EQ(INF, IMAGE_DATA_SMALL_WITH_INFS2.max());
}

TEST(ImageDataTest, sum){
    EXPECT_FLOAT_EQ(8.2, IMAGE_DATA_SMALL.sum());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_NANS.sum(), IsNan());
    EXPECT_FLOAT_EQ(INF, IMAGE_DATA_SMALL_WITH_INFS.sum());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_INFS2.sum(), IsNan());
}

TEST(ImageDataTest, mean){
    EXPECT_FLOAT_EQ(2.05, IMAGE_DATA_SMALL.mean());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_NANS.mean(), IsNan());
    EXPECT_FLOAT_EQ(INF, IMAGE_DATA_SMALL_WITH_INFS.mean());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_INFS2.mean(), IsNan());
}

TEST(ImageDataTest, var){
    EXPECT_FLOAT_EQ(145.8225, IMAGE_DATA_SMALL.var());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_NANS.var(), IsNan());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_INFS.var(), IsNan());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_INFS2.var(), IsNan());
}

TEST(ImageDataTest, nanmin){
    EXPECT_FLOAT_EQ(-13.5, IMAGE_DATA_SMALL.nanmin());
    EXPECT_FLOAT_EQ(-13.5, IMAGE_DATA_SMALL_WITH_NANS.nanmin());
    EXPECT_FLOAT_EQ(-13.5, IMAGE_DATA_SMALL_WITH_INFS.nanmin());
    EXPECT_FLOAT_EQ(-INF, IMAGE_DATA_SMALL_WITH_INFS2.nanmin());
}

TEST(ImageDataTest, nanmax){
    EXPECT_FLOAT_EQ(20.4, IMAGE_DATA_SMALL.nanmax());
    EXPECT_FLOAT_EQ(20.4, IMAGE_DATA_SMALL_WITH_NANS.nanmax());
    EXPECT_FLOAT_EQ(INF, IMAGE_DATA_SMALL_WITH_INFS.nanmax());
    EXPECT_FLOAT_EQ(INF, IMAGE_DATA_SMALL_WITH_INFS2.nanmax());
}

TEST(ImageDataTest, nansum){
    EXPECT_FLOAT_EQ(8.2, IMAGE_DATA_SMALL.nansum());
    EXPECT_FLOAT_EQ(8.2, IMAGE_DATA_SMALL_WITH_NANS.nansum());
    EXPECT_FLOAT_EQ(INF, IMAGE_DATA_SMALL_WITH_INFS.nansum());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_INFS2.nansum(), IsNan());
}

TEST(ImageDataTest, nanmean){
    EXPECT_FLOAT_EQ(2.05, IMAGE_DATA_SMALL.nanmean());
    EXPECT_FLOAT_EQ(2.05, IMAGE_DATA_SMALL_WITH_NANS.nanmean());
    EXPECT_FLOAT_EQ(INF, IMAGE_DATA_SMALL_WITH_INFS.nanmean());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_INFS2.nanmean(), IsNan());
}

TEST(ImageDataTest, nanvar){
    EXPECT_FLOAT_EQ(145.8225, IMAGE_DATA_SMALL.nanvar());
    EXPECT_FLOAT_EQ(145.8225, IMAGE_DATA_SMALL_WITH_NANS.nanvar());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_INFS.nanvar(), IsNan());
    EXPECT_THAT(IMAGE_DATA_SMALL_WITH_INFS2.nanvar(), IsNan());
}

TEST(ImageDataTest, nansize){
    EXPECT_EQ(4, IMAGE_DATA_SMALL.nansize());
    EXPECT_EQ(4, IMAGE_DATA_SMALL_WITH_NANS.nansize());
    EXPECT_EQ(6, IMAGE_DATA_SMALL_WITH_INFS.nansize());
}

TEST(ImageDataTest, containsNan){
    EXPECT_FALSE(IMAGE_DATA_SMALL.containsNan());
    EXPECT_TRUE(IMAGE_DATA_SMALL_WITH_NANS.containsNan());
    EXPECT_FALSE(IMAGE_DATA_SMALL_WITH_INFS.containsNan());
}

TEST(ImageDataTest, containsInf){
    EXPECT_FALSE(IMAGE_DATA_SMALL.containsInf());
    EXPECT_FALSE(IMAGE_DATA_SMALL_WITH_NANS.containsInf());
    EXPECT_TRUE(IMAGE_DATA_SMALL_WITH_INFS.containsInf());
    EXPECT_TRUE(IMAGE_DATA_SMALL_WITH_INFS2.containsInf());
}
