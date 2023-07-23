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

#include "yagit/Interpolation.hpp"

#include <tuple>

#include <gtest/gtest.h>
#include "TestUtils.hpp"

namespace{
const yagit::DataOffset DATA_OFFSET{0, 0, 0};

const yagit::Image3D<float> IMAGE_3D = {
    {{1, 2},
     {4, 5}},
    {{7, 8},
     {10, 11}}
};

const yagit::ImageData IMAGE_DATA(IMAGE_3D, DATA_OFFSET, {1, 1, 1});
const yagit::ImageData IMAGE_DATA2(IMAGE_3D, {1, 1, 1}, {1, 1, 1});
}

using LinearAlongAxisWithSpacingType = std::tuple<std::vector<float>, float,
                                                  std::vector<float>, float>;

class LinearAlongAxisWithSpacingTest : public ::testing::TestWithParam<LinearAlongAxisWithSpacingType> {};

const LinearAlongAxisWithSpacingType linearAlongAxisWithSpacingValues[] = {
    // original image             interpolated image
    {{0, 2, 4}, 2.0,              {0, 1, 2, 3, 4}, 1.0},
    {{0, 3, 6, 9, 12, 15}, 3.0,   {0, 1.4, 2.8, 4.2, 5.6, 7, 8.4, 9.8, 11.2, 12.6, 14}, 1.4},
    {{2, 4, 7, -5, 3.5}, 1.2,     {2, 3.16666666, 4.5, 6.25, 3, -4, -0.75}, 0.7},
    {{4, 2, 6, 7}, 1.0,           {4, 6}, 2.0},
    {{5}, 2.0,                    {5}, 1.2}
};

INSTANTIATE_TEST_SUITE_P(InterpolationTest, LinearAlongAxisWithSpacingTest,
                         ::testing::ValuesIn(linearAlongAxisWithSpacingValues));

TEST_P(LinearAlongAxisWithSpacingTest, ZAxis){
    const auto& [original, oldSpacingVal, interpolated, newSpacingVal] = GetParam();
    const yagit::DataSize oldSize{static_cast<uint32_t>(original.size()), 1, 1};
    const yagit::DataSize newSize{static_cast<uint32_t>(interpolated.size()), 1, 1};
    const yagit::DataSpacing oldSpacing{oldSpacingVal, 2, 2};
    const yagit::DataSpacing newSpacing{newSpacingVal, 2, 2};

    const yagit::ImageData imageData(original, oldSize, DATA_OFFSET, oldSpacing);
    const yagit::ImageData expected(interpolated, newSize, DATA_OFFSET, newSpacing);
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, newSpacingVal, yagit::ImageAxis::Z),
                matchImageData(expected, 1e-5));
}

TEST_P(LinearAlongAxisWithSpacingTest, YAxis){
    const auto& [original, oldSpacingVal, interpolated, newSpacingVal] = GetParam();
    const yagit::DataSize oldSize{1, static_cast<uint32_t>(original.size()), 1};
    const yagit::DataSize newSize{1, static_cast<uint32_t>(interpolated.size()), 1};
    const yagit::DataSpacing oldSpacing{2, oldSpacingVal, 2};
    const yagit::DataSpacing newSpacing{2, newSpacingVal, 2};

    const yagit::ImageData imageData(original, oldSize, DATA_OFFSET, oldSpacing);
    const yagit::ImageData expected(interpolated, newSize, DATA_OFFSET, newSpacing);
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, newSpacingVal, yagit::ImageAxis::Y),
                matchImageData(expected, 1e-5));
}

TEST_P(LinearAlongAxisWithSpacingTest, XAxis){
    const auto& [original, oldSpacingVal, interpolated, newSpacingVal] = GetParam();
    const yagit::DataSize oldSize{1, 1, static_cast<uint32_t>(original.size())};
    const yagit::DataSize newSize{1, 1, static_cast<uint32_t>(interpolated.size())};
    const yagit::DataSpacing oldSpacing{2, 2, oldSpacingVal};
    const yagit::DataSpacing newSpacing{2, 2, newSpacingVal};

    const yagit::ImageData imageData(original, oldSize, DATA_OFFSET, oldSpacing);
    const yagit::ImageData expected(interpolated, newSize, DATA_OFFSET, newSpacing);
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, newSpacingVal, yagit::ImageAxis::X),
                matchImageData(expected, 1e-5));
}

TEST(InterpolationTest, linearAlongAxisWithSpacingForEmptyImage){
    const float newSpacing = 1;
    const yagit::ImageData imageData({}, {0, 0, 0}, DATA_OFFSET, {2, 2, 2});
    const yagit::ImageData expectedZ({}, {0, 0, 0}, DATA_OFFSET, {newSpacing, 2, 2});
    const yagit::ImageData expectedY({}, {0, 0, 0}, DATA_OFFSET, {2, newSpacing, 2});
    const yagit::ImageData expectedX({}, {0, 0, 0}, DATA_OFFSET, {2, 2, newSpacing});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, newSpacing, yagit::ImageAxis::Z), matchImageData(expectedZ));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, newSpacing, yagit::ImageAxis::Y), matchImageData(expectedY));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, newSpacing, yagit::ImageAxis::X), matchImageData(expectedX));
}

TEST(InterpolationTest, linearAlongAxisWithSpacingForMultiframeImage){
    const yagit::Image3D<float> image3Dz = {
        {{1, 2},
         {4, 5}},
        {{5.8, 6.8},
         {8.8, 9.8}}
    };
    const yagit::Image3D<float> image3Dy = {
        {{1, 2},
         {3.4, 4.4}},
        {{7, 8},
         {9.4, 10.4}}
    };
    const yagit::Image3D<float> image3Dx = {
        {{1, 1.8},
         {4, 4.8}},
        {{7, 7.8},
         {10, 10.8}}
    };

    const float newSpacing = 0.8;
    const yagit::ImageData expectedZ(image3Dz, DATA_OFFSET, {newSpacing, 1, 1});
    const yagit::ImageData expectedY(image3Dy, DATA_OFFSET, {1, newSpacing, 1});
    const yagit::ImageData expectedX(image3Dx, DATA_OFFSET, {1, 1, newSpacing});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(IMAGE_DATA, newSpacing, yagit::ImageAxis::Z), matchImageData(expectedZ));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(IMAGE_DATA, newSpacing, yagit::ImageAxis::Y), matchImageData(expectedY));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(IMAGE_DATA, newSpacing, yagit::ImageAxis::X), matchImageData(expectedX));
}

using LinearAlongAxisWithOffsetAndSpacingType = std::tuple<std::vector<float>, float, float,
                                                           std::vector<float>, float, float,
                                                           float>;

class LinearAlongAxisWithOffsetAndSpacingTest : public ::testing::TestWithParam<LinearAlongAxisWithOffsetAndSpacingType>{};

const LinearAlongAxisWithOffsetAndSpacingType linearAlongAxisWithOffsetAndSpacingValues[] = {
    // original image                  interpolated image                                                 grid offset
    {{0, 2, 4}, 1.0, 2.0,              {0.2, 1.2, 2.2, 3.2}, 1.2, 1.0,                                    1.2},
    {{0, 3, 6, 9, 12, 15}, 2.0, 3.0,   {1, 2.4, 3.8, 5.2, 6.6, 8, 9.4, 10.8, 12.2, 13.6, 15}, 3.0, 1.4,   3.0},
    {{3, 5, 2}, 1.6, 2.0,              {3, 5, 2}, 1.6, 2.0,                                               1.6},
    {{4, 2, 6, 7}, 0.5, 1.0,           {3.8, 6.1}, 0.6, 2.0,                                              0.6},
    {{0, 1, 2, 3, 4}, 0.0, 1.0,        {1.9, 3.9}, 1.9, 2.0,                                             -0.1},

    {{3, 5}, 3, 2,                     {3.7, 4.8}, 3.7, 1.1,                                              0.4},
    {{3, 5}, 3, 2,                     {4}, 4, 1.1,                                                      -0.4},
    {{-3, -1}, -3, 2,                  {-2.9, -1.8}, -2.9, 1.1,                                           0.4},
    {{-3, -1}, -3, 2,                  {-2.6, -1.5}, -2.6, 1.1,                                          -0.4}
};

INSTANTIATE_TEST_SUITE_P(InterpolationTest, LinearAlongAxisWithOffsetAndSpacingTest,
                         ::testing::ValuesIn(linearAlongAxisWithOffsetAndSpacingValues));

TEST_P(LinearAlongAxisWithOffsetAndSpacingTest, ZAxis){
    const auto& [original, oldOffsetVal, oldSpacingVal,
                 interpolated, newOffsetVal, newSpacingVal,
                 gridOffset] = GetParam();
    const yagit::DataSize oldSize{static_cast<uint32_t>(original.size()), 1, 1};
    const yagit::DataSize newSize{static_cast<uint32_t>(interpolated.size()), 1, 1};
    const yagit::DataOffset oldOffset{oldOffsetVal, 1, 1};
    const yagit::DataOffset newOffset{newOffsetVal, 1, 1};
    const yagit::DataSpacing oldSpacing{oldSpacingVal, 2, 2};
    const yagit::DataSpacing newSpacing{newSpacingVal, 2, 2};

    const yagit::ImageData imageData(original, oldSize, oldOffset, oldSpacing);
    const yagit::ImageData expected(interpolated, newSize, newOffset, newSpacing);
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, gridOffset, newSpacingVal, yagit::ImageAxis::Z),
                matchImageData(expected));
}

TEST_P(LinearAlongAxisWithOffsetAndSpacingTest, YAxis){
    const auto& [original, oldOffsetVal, oldSpacingVal,
                 interpolated, newOffsetVal, newSpacingVal,
                 gridOffset] = GetParam();
    const yagit::DataSize oldSize{1, static_cast<uint32_t>(original.size()), 1};
    const yagit::DataSize newSize{1, static_cast<uint32_t>(interpolated.size()), 1};
    const yagit::DataOffset oldOffset{1, oldOffsetVal, 1};
    const yagit::DataOffset newOffset{1, newOffsetVal, 1};
    const yagit::DataSpacing oldSpacing{2, oldSpacingVal, 2};
    const yagit::DataSpacing newSpacing{2, newSpacingVal, 2};

    const yagit::ImageData imageData(original, oldSize, oldOffset, oldSpacing);
    const yagit::ImageData expected(interpolated, newSize, newOffset, newSpacing);
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, gridOffset, newSpacingVal, yagit::ImageAxis::Y),
                matchImageData(expected));
}

TEST_P(LinearAlongAxisWithOffsetAndSpacingTest, XAxis){
    const auto& [original, oldOffsetVal, oldSpacingVal,
                 interpolated, newOffsetVal, newSpacingVal,
                 gridOffset] = GetParam();
    const yagit::DataSize oldSize{1, 1, static_cast<uint32_t>(original.size())};
    const yagit::DataSize newSize{1, 1, static_cast<uint32_t>(interpolated.size())};
    const yagit::DataOffset oldOffset{1, 1, oldOffsetVal};
    const yagit::DataOffset newOffset{1, 1, newOffsetVal};
    const yagit::DataSpacing oldSpacing{2, 2, oldSpacingVal};
    const yagit::DataSpacing newSpacing{2, 2, newSpacingVal};

    const yagit::ImageData imageData(original, oldSize, oldOffset, oldSpacing);
    const yagit::ImageData expected(interpolated, newSize, newOffset, newSpacing);
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, gridOffset, newSpacingVal, yagit::ImageAxis::X),
                matchImageData(expected));
}

TEST(InterpolationTest, linearAlongAxisWithOffsetAndSpacingForEmptyImage){
    const float newOffset = 1.6;
    const float newSpacing = 1;
    const yagit::ImageData imageData({}, {0, 0, 0}, {1.2, 1.2, 1.2}, {2, 2, 2});
    const yagit::ImageData expectedZ({}, {0, 0, 0}, {newOffset, 1.2, 1.2}, {newSpacing, 2, 2});
    const yagit::ImageData expectedY({}, {0, 0, 0}, {1.2, newOffset, 1.2}, {2, newSpacing, 2});
    const yagit::ImageData expectedX({}, {0, 0, 0}, {1.2, 1.2, newOffset}, {2, 2, newSpacing});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, newOffset, newSpacing, yagit::ImageAxis::Z), matchImageData(expectedZ));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, newOffset, newSpacing, yagit::ImageAxis::Y), matchImageData(expectedY));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, newOffset, newSpacing, yagit::ImageAxis::X), matchImageData(expectedX));
}

TEST(InterpolationTest, linearAlongAxisWithOffsetAndSpacingForOffsetOutsideImage){
    yagit::ImageData imageData({1, 2, 3, 4}, {4, 1, 1}, {1, 1, 1}, {1, 1, 1});
    const yagit::ImageData expectedZ({}, {0, 0, 0}, {5, 1, 1}, {5, 1, 1});
    const yagit::ImageData expectedY({}, {0, 0, 0}, {1, 5, 1}, {1, 5, 1});
    const yagit::ImageData expectedX({}, {0, 0, 0}, {1, 1, 5}, {1, 1, 5});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, 0, 5, yagit::ImageAxis::Z), matchImageData(expectedZ));
    imageData.setSize({1, 4, 1});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, 0, 5, yagit::ImageAxis::Y), matchImageData(expectedY));
    imageData.setSize({1, 1, 4});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, 0, 5, yagit::ImageAxis::X), matchImageData(expectedX));
}

TEST(InterpolationTest, linearAlongAxisWithOffsetAndSpacingForOffsetOutsideImage2){
    yagit::ImageData imageData({6}, {1, 1, 1}, {1.3, 1.3, 1.3}, {2, 2, 2});
    const yagit::ImageData expectedZ({}, {0, 0, 0}, {1.4, 1.3, 1.3}, {1.1, 2, 2});
    const yagit::ImageData expectedY({}, {0, 0, 0}, {1.3, 1.4, 1.3}, {2, 1.1, 2});
    const yagit::ImageData expectedX({}, {0, 0, 0}, {1.3, 1.3, 1.4}, {2, 2, 1.1});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, 1.4, 1.1, yagit::ImageAxis::Z), matchImageData(expectedZ));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, 1.4, 1.1, yagit::ImageAxis::Y), matchImageData(expectedY));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(imageData, 1.4, 1.1, yagit::ImageAxis::X), matchImageData(expectedX));
}

TEST(InterpolationTest, linearAlongAxisWithOffsetAndSpacingForMultiframeImage){
    const yagit::Image3D<float> image3Dz = {
        {{2.2, 3.2},
         {5.2, 6.2}},
        {{6.4, 7.4},
         {9.4, 10.4}}
    };
    const yagit::Image3D<float> image3Dy = {
        {{1.6, 2.6},
         {3.7, 4.7}},
        {{7.6, 8.6},
         {9.7, 10.7}}
    };
    const yagit::Image3D<float> image3Dx = {
        {{1.2, 1.9},
         {4.2, 4.9}},
        {{7.2, 7.9},
         {10.2, 10.9}}
    };

    const float newOffset = 1.2;
    const float newSpacing = 0.7;
    const yagit::ImageData expectedZ(image3Dz, {newOffset, 1, 1}, {newSpacing, 1, 1});
    const yagit::ImageData expectedY(image3Dy, {1, newOffset, 1}, {1, newSpacing, 1});
    const yagit::ImageData expectedX(image3Dx, {1, 1, newOffset}, {1, 1, newSpacing});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(IMAGE_DATA2, newOffset, newSpacing, yagit::ImageAxis::Z),
                matchImageData(expectedZ));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(IMAGE_DATA2, newOffset, newSpacing, yagit::ImageAxis::Y),
                matchImageData(expectedY));
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(IMAGE_DATA2, newOffset, newSpacing, yagit::ImageAxis::X),
                matchImageData(expectedX));
}

TEST(InterpolationTest, linearAlongAxisWithRefImg){
    const float newOffset = 1.5;
    const float newSpacing = 2.5;
    yagit::ImageData img({0, 2, 4}, {3, 1, 1}, {1, 1, 1}, {2, 2, 2});
    yagit::ImageData refImg({20, 34, 18}, {3, 1, 1}, {newOffset, newOffset, newOffset}, {newSpacing, newSpacing, newSpacing});

    const yagit::ImageData expectedZ({0.5, 3}, {2, 1, 1}, {newOffset, 1, 1}, {newSpacing, 2, 2});
    const yagit::ImageData expectedY({0.5, 3}, {1, 2, 1}, {1, newOffset, 1}, {2, newSpacing, 2});
    const yagit::ImageData expectedX({0.5, 3}, {1, 1, 2}, {1, 1, newOffset}, {2, 2, newSpacing});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(img, refImg, yagit::ImageAxis::Z), matchImageData(expectedZ));
    img.setSize({1, 3, 1});
    refImg.setSize({1, 3, 1});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(img, refImg, yagit::ImageAxis::Y), matchImageData(expectedY));
    img.setSize({1, 1, 3});
    refImg.setSize({1, 1, 3});
    EXPECT_THAT(yagit::Interpolation::linearAlongAxis(img, refImg, yagit::ImageAxis::X), matchImageData(expectedX));
}

TEST(InterpolationTest, linearAlongAxisInDifferentOrder){
    const float newOffset = 1.2;
    const float newSpacing = 0.7;

    const auto inerpolatedZ = yagit::Interpolation::linearAlongAxis(IMAGE_DATA2, newOffset, newSpacing, yagit::ImageAxis::Z);
    const auto inerpolatedY = yagit::Interpolation::linearAlongAxis(IMAGE_DATA2, newOffset, newSpacing, yagit::ImageAxis::Y);
    const auto inerpolatedX = yagit::Interpolation::linearAlongAxis(IMAGE_DATA2, newOffset, newSpacing, yagit::ImageAxis::X);

    const auto inerpolatedYX = yagit::Interpolation::linearAlongAxis(inerpolatedY, newOffset, newSpacing, yagit::ImageAxis::X);
    const auto inerpolatedXY = yagit::Interpolation::linearAlongAxis(inerpolatedX, newOffset, newSpacing, yagit::ImageAxis::Y);
    EXPECT_THAT(inerpolatedYX, matchImageData(inerpolatedXY));

    const auto inerpolatedZX = yagit::Interpolation::linearAlongAxis(inerpolatedZ, newOffset, newSpacing, yagit::ImageAxis::X);
    const auto inerpolatedXZ = yagit::Interpolation::linearAlongAxis(inerpolatedX, newOffset, newSpacing, yagit::ImageAxis::Z);
    EXPECT_THAT(inerpolatedZX, matchImageData(inerpolatedXZ));

    const auto inerpolatedZY = yagit::Interpolation::linearAlongAxis(inerpolatedZ, newOffset, newSpacing, yagit::ImageAxis::Y);
    const auto inerpolatedYZ = yagit::Interpolation::linearAlongAxis(inerpolatedY, newOffset, newSpacing, yagit::ImageAxis::Z);
    EXPECT_THAT(inerpolatedZY, matchImageData(inerpolatedYZ));
}

TEST(InterpolationTest, bilinearOnPlaneWithSpacing){
    const yagit::Image3D<float> image3Dyx = {
        {{1, 1.8},
         {3.1, 3.9}},
        {{7, 7.8},
         {9.1, 9.9}}
    };
    const yagit::Image3D<float> image3Dzx = {
        {{1, 1.8},
         {4, 4.8}},
        {{5.2, 6},
         {8.2, 9}}
    };
    const yagit::Image3D<float> image3Dzy = {
        {{1, 2},
         {3.4, 4.4}},
        {{5.2, 6.2},
         {7.6, 8.6}}
    };

    const float newSpacing1 = 0.7;
    const float newSpacing2 = 0.8;
    const yagit::ImageData expectedYX(image3Dyx, DATA_OFFSET, {1, newSpacing1, newSpacing2});
    const yagit::ImageData expectedZX(image3Dzx, DATA_OFFSET, {newSpacing1, 1, newSpacing2});
    const yagit::ImageData expectedZY(image3Dzy, DATA_OFFSET, {newSpacing1, newSpacing2, 1});
    EXPECT_THAT(yagit::Interpolation::bilinearOnPlane(IMAGE_DATA, newSpacing1, newSpacing2, yagit::ImagePlane::YX),
                matchImageData(expectedYX));
    EXPECT_THAT(yagit::Interpolation::bilinearOnPlane(IMAGE_DATA, newSpacing1, newSpacing2, yagit::ImagePlane::ZX),
                matchImageData(expectedZX));
    EXPECT_THAT(yagit::Interpolation::bilinearOnPlane(IMAGE_DATA, newSpacing1, newSpacing2, yagit::ImagePlane::ZY),
                matchImageData(expectedZY));
}

TEST(InterpolationTest, bilinearOnPlaneWithOffsetAndSpacing){
    const yagit::Image3D<float> image3Dyx = {
        {{1.7, 2.5},
         {3.8, 4.6}},
        {{7.7, 8.5},
         {9.8, 10.6}}
    };
    const yagit::Image3D<float> image3Dzx = {
        {{2.3, 3.1},
         {5.3, 6.1}},
        {{6.5, 7.3},
         {9.5, 10.3}}
    };
    const yagit::Image3D<float> image3Dzy = {
        {{2.5, 3.5},
         {4.9, 5.9}},
        {{6.7, 7.7},
         {9.1, 10.1}}
    };

    const float newOffset1 = 1.2;
    const float newOffset2 = 1.1;
    const float newSpacing1 = 0.7;
    const float newSpacing2 = 0.8;
    const yagit::ImageData expectedYX(image3Dyx, {1, newOffset1, newOffset2}, {1, newSpacing1, newSpacing2});
    const yagit::ImageData expectedZX(image3Dzx, {newOffset1, 1, newOffset2}, {newSpacing1, 1, newSpacing2});
    const yagit::ImageData expectedZY(image3Dzy, {newOffset1, newOffset2, 1}, {newSpacing1, newSpacing2, 1});
    EXPECT_THAT(yagit::Interpolation::bilinearOnPlane(IMAGE_DATA2, newOffset1, newOffset2,
                                                      newSpacing1, newSpacing2, yagit::ImagePlane::YX),
                matchImageData(expectedYX));
    EXPECT_THAT(yagit::Interpolation::bilinearOnPlane(IMAGE_DATA2, newOffset1, newOffset2,
                                                      newSpacing1, newSpacing2, yagit::ImagePlane::ZX),
                matchImageData(expectedZX));
    EXPECT_THAT(yagit::Interpolation::bilinearOnPlane(IMAGE_DATA2, newOffset1, newOffset2,
                                                      newSpacing1, newSpacing2, yagit::ImagePlane::ZY),
                matchImageData(expectedZY));
}

TEST(InterpolationTest, bilinearOnPlaneWithRefImg){
    const float newOffset = 1.5;
    const float newSpacing = 1.2;
    yagit::ImageData img({0, 2, 4, 6}, {1, 2, 2}, {1, 1, 1}, {2, 2, 2});
    yagit::ImageData refImg({20, 34, 18, 10}, {1, 2, 2}, {newOffset, newOffset, newOffset}, {newSpacing, newSpacing, newSpacing});

    const yagit::ImageData expectedYX({1.5, 2.7, 3.9, 5.1}, {1, 2, 2}, {1, newOffset, newOffset}, {2, newSpacing, newSpacing});
    const yagit::ImageData expectedZX({1.5, 2.7, 3.9, 5.1}, {2, 1, 2}, {newOffset, 1, newOffset}, {newSpacing, 2, newSpacing});
    const yagit::ImageData expectedZY({1.5, 2.7, 3.9, 5.1}, {2, 2, 1}, {newOffset, newOffset, 1}, {newSpacing, newSpacing, 2});
    EXPECT_THAT(yagit::Interpolation::bilinearOnPlane(img, refImg, yagit::ImagePlane::YX), matchImageData(expectedYX));
    img.setSize({2, 1, 2});
    refImg.setSize({2, 1, 2});
    EXPECT_THAT(yagit::Interpolation::bilinearOnPlane(img, refImg, yagit::ImagePlane::ZX), matchImageData(expectedZX));
    img.setSize({2, 2, 1});
    refImg.setSize({2, 2, 1});
    EXPECT_THAT(yagit::Interpolation::bilinearOnPlane(img, refImg, yagit::ImagePlane::ZY), matchImageData(expectedZY));
}

TEST(InterpolationTest, trilinearWithSpacing){
    const yagit::Image3D<float> image3D = {
        {{1, 1.8},
         {3.1, 3.9}},
        {{4.6, 5.4},
         {6.7, 7.5}}
    };

    const yagit::DataSpacing newSpacing{0.6, 0.7, 0.8};
    const yagit::ImageData expected(image3D, DATA_OFFSET, newSpacing);
    EXPECT_THAT(yagit::Interpolation::trilinear(IMAGE_DATA, newSpacing),
                matchImageData(expected));
}

TEST(InterpolationTest, trilinearWithOffsetAndSpacing){
    const yagit::Image3D<float> image3D = {
        {{3.5, 4.3},
         {5.6, 6.4}},
        {{7.1, 7.9},
         {9.2, 10}}
    };

    const yagit::DataOffset newOffset{1.3, 1.2, 1.1};
    const yagit::DataSpacing newSpacing{0.6, 0.7, 0.8};
    const yagit::ImageData expected(image3D, newOffset, newSpacing);
    EXPECT_THAT(yagit::Interpolation::trilinear(IMAGE_DATA2, newOffset, newSpacing), matchImageData(expected));
}

TEST(InterpolationTest, trilinearWithRefImg){
    const yagit::DataOffset newOffset{1.5, 1.5, 1.5};
    const yagit::DataSpacing newSpacing{1.2, 1.2, 1.2};
    yagit::ImageData img({0, 2, 4, 6, 8, 10, 12, 14}, {2, 2, 2}, {1, 1, 1}, {2, 2, 2});
    yagit::ImageData refImg({20, 34, 18, 10, 3, 0, 123, 4}, {2, 2, 2}, newOffset, newSpacing);

    const yagit::ImageData expected({3.5, 4.7, 5.9, 7.1, 8.3, 9.5, 10.7, 11.9}, {2, 2, 2}, newOffset, newSpacing);
    EXPECT_THAT(yagit::Interpolation::trilinear(img, refImg), matchImageData(expected));
}

TEST(InterpolationTest, bilinearAtPoint){
    const yagit::Image3D<float> image3D = {
        {{3, 6.8},
         {5, -2}},
        {{2, 8},
         {10, 2}}
    };
    const yagit::ImageData imageData(image3D, {1, 2, 3}, {1.5, 2, 2.5});

    EXPECT_FLOAT_EQ(3.7064, *yagit::Interpolation::bilinearAtPoint(imageData, 0, 2.7, 3.8));
    EXPECT_FLOAT_EQ(5.152, *yagit::Interpolation::bilinearAtPoint(imageData, 1, 2.7, 3.8));
}

TEST(InterpolationTest, bilinearAtPointForPointOnEdge){
    const yagit::ImageData imageData({1, 2, 3, 4}, {1, 2, 2}, {1, 1, 1}, {2, 2, 2});

    EXPECT_FLOAT_EQ(1, *yagit::Interpolation::bilinearAtPoint(imageData, 0, 1, 1));
    EXPECT_FLOAT_EQ(2, *yagit::Interpolation::bilinearAtPoint(imageData, 0, 1, 3));
    EXPECT_FLOAT_EQ(3, *yagit::Interpolation::bilinearAtPoint(imageData, 0, 3, 1));
    EXPECT_FLOAT_EQ(4, *yagit::Interpolation::bilinearAtPoint(imageData, 0, 3, 3));

    EXPECT_FLOAT_EQ((1 + 2) / 2.0, *yagit::Interpolation::bilinearAtPoint(imageData, 0, 1, 2));
    EXPECT_FLOAT_EQ((2 + 4) / 2.0, *yagit::Interpolation::bilinearAtPoint(imageData, 0, 2, 3));
    EXPECT_FLOAT_EQ((3 + 4) / 2.0, *yagit::Interpolation::bilinearAtPoint(imageData, 0, 3, 2));
    EXPECT_FLOAT_EQ((1 + 3) / 2.0, *yagit::Interpolation::bilinearAtPoint(imageData, 0, 2, 1));
}

TEST(InterpolationTest, bilinearAtPointForPointOutsideImage){
    const yagit::ImageData imageData({1, 2, 3, 4}, {1, 2, 2}, {1, 2, 3}, {2, 2, 2.5});

    EXPECT_EQ(std::nullopt, yagit::Interpolation::bilinearAtPoint(imageData, 0, 20, 20));
    EXPECT_EQ(std::nullopt, yagit::Interpolation::bilinearAtPoint(imageData, 0, -20, -20));

    EXPECT_EQ(std::nullopt, yagit::Interpolation::bilinearAtPoint(imageData, 0, 4.1, 5.6));
    EXPECT_EQ(std::nullopt, yagit::Interpolation::bilinearAtPoint(imageData, 0, 1.9, 2.9));

    EXPECT_THROW(yagit::Interpolation::bilinearAtPoint(imageData, 10, 2.5, 3.5), std::out_of_range);
}

TEST(InterpolationTest, trilinearAtPoint){
    const yagit::Image3D<float> image3D = {
        {{3, 6.8},
         {5, -2}},
        {{2, 8},
         {10, 2}}
    };
    const yagit::ImageData imageData(image3D, {1, 2, 3}, {1.5, 2, 2.5});

    EXPECT_FLOAT_EQ(4.89642666, *yagit::Interpolation::trilinearAtPoint(imageData, 2.1, 2.7, 4.2));
    EXPECT_FLOAT_EQ(3.6376, *yagit::Interpolation::trilinearAtPoint(imageData, 1.3, 3.5, 3.9));
}

TEST(InterpolationTest, trilinearAtPointForPointOnEdge){
    const yagit::ImageData imageData({1, 2, 3, 4, 5, 6, 7, 8}, {2, 2, 2}, {1, 1, 1}, {2, 2, 2});

    EXPECT_FLOAT_EQ(1, *yagit::Interpolation::trilinearAtPoint(imageData, 1, 1, 1));
    EXPECT_FLOAT_EQ(2, *yagit::Interpolation::trilinearAtPoint(imageData, 1, 1, 3));
    EXPECT_FLOAT_EQ(3, *yagit::Interpolation::trilinearAtPoint(imageData, 1, 3, 1));
    EXPECT_FLOAT_EQ(4, *yagit::Interpolation::trilinearAtPoint(imageData, 1, 3, 3));
    EXPECT_FLOAT_EQ(5, *yagit::Interpolation::trilinearAtPoint(imageData, 3, 1, 1));
    EXPECT_FLOAT_EQ(6, *yagit::Interpolation::trilinearAtPoint(imageData, 3, 1, 3));
    EXPECT_FLOAT_EQ(7, *yagit::Interpolation::trilinearAtPoint(imageData, 3, 3, 1));
    EXPECT_FLOAT_EQ(8, *yagit::Interpolation::trilinearAtPoint(imageData, 3, 3, 3));

    EXPECT_FLOAT_EQ((1 + 2) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 1, 1, 2));
    EXPECT_FLOAT_EQ((2 + 4) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 1, 2, 3));
    EXPECT_FLOAT_EQ((3 + 4) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 1, 3, 2));
    EXPECT_FLOAT_EQ((1 + 3) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 1, 2, 1));
    EXPECT_FLOAT_EQ((5 + 6) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 3, 1, 2));
    EXPECT_FLOAT_EQ((6 + 8) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 3, 2, 3));
    EXPECT_FLOAT_EQ((7 + 8) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 3, 3, 2));
    EXPECT_FLOAT_EQ((5 + 7) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 3, 2, 1));
    EXPECT_FLOAT_EQ((1 + 5) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 2, 1, 1));
    EXPECT_FLOAT_EQ((2 + 6) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 2, 1, 3));
    EXPECT_FLOAT_EQ((3 + 7) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 2, 3, 1));
    EXPECT_FLOAT_EQ((4 + 8) / 2.0, *yagit::Interpolation::trilinearAtPoint(imageData, 2, 3, 3));
}

TEST(InterpolationTest, trilinearAtPointForPointOutsideImage){
    const yagit::ImageData imageData({1, 2, 3, 4, 5, 6, 7, 8}, {2, 2, 2}, {1, 2, 3}, {2, 2, 2.5});

    EXPECT_EQ(std::nullopt, yagit::Interpolation::trilinearAtPoint(imageData, 20, 20, 20));
    EXPECT_EQ(std::nullopt, yagit::Interpolation::trilinearAtPoint(imageData, -20, -20, -20));

    EXPECT_EQ(std::nullopt, yagit::Interpolation::trilinearAtPoint(imageData, 3.1, 4.1, 5.6));
    EXPECT_EQ(std::nullopt, yagit::Interpolation::trilinearAtPoint(imageData, 0.9, 1.9, 2.9));
}
