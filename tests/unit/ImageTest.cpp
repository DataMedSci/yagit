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

#include <limits>

#include <gtest/gtest.h>

namespace{
const yagit::Image3D IMAGE = {
    {{1.45364,   45.578234},
     {-0.34921,  999.876543}},
    {{-12.74829, 43.45903},
     {0,         9.999999}}
};

const yagit::Image3D IMAGE_SPECIAL_VALUES = {
    {{std::numeric_limits<float>::quiet_NaN(),
      std::numeric_limits<float>::infinity(),
      -std::numeric_limits<float>::infinity()}}
};

const yagit::Image3D IMAGE_NONUNIFORM = {
    {{1},
     {2, 3},
     {4, 5, 6}},
    {{7, 8, 9, 10}}
};
}

TEST(ImageTest, image2DToString){
    const std::string image2DStr = yagit::image2DToString(IMAGE[0]);

    const std::string expected =
        "[[1.45364, 45.5782],\n"
        " [-0.34921, 999.877]]";
    EXPECT_EQ(expected, image2DStr);
}

TEST(ImageTest, image2DToStringWithPrecision){
    const std::string image2DStr = yagit::image2DToString(IMAGE[0], 2);

    const std::string expected =
        "[[1.45, 45.58],\n"
        " [-0.35, 999.88]]";
    EXPECT_EQ(expected, image2DStr);
}

TEST(ImageTest, image2DToStringWithNegativePrecision){
    const std::string image2DStr = yagit::image2DToString(IMAGE[0], -2);

    const std::string expected =
        "[[1.45364, 45.5782],\n"
        " [-0.34921, 999.877]]";
    EXPECT_EQ(expected, image2DStr);
}

TEST(ImageTest, image2DToStringForSpecialValues){
    const std::string image2DStr = yagit::image2DToString(IMAGE_SPECIAL_VALUES[0]);

    const std::string expected = "[[nan, inf, -inf]]";
    EXPECT_EQ(expected, image2DStr);
}

TEST(ImageTest, image2DToStringForNonuniformImage){
    const std::string image2DStr = yagit::image2DToString(IMAGE_NONUNIFORM[0]);

    const std::string expected =
        "[[1],\n"
        " [2, 3],\n"
        " [4, 5, 6]]";
    EXPECT_EQ(expected, image2DStr);
}

TEST(ImageTest, image3DToString){
    const std::string image3DStr = yagit::image3DToString(IMAGE);

    const std::string expected =
        "[[[1.45364, 45.5782],\n"
        "  [-0.34921, 999.877]],\n"
        " [[-12.7483, 43.459],\n"
        "  [0, 10]]]";
    EXPECT_EQ(expected, image3DStr);
}

TEST(ImageTest, image3DToStringWithPrecision){
    const std::string image3DStr = yagit::image3DToString(IMAGE, 2);

    const std::string expected =
        "[[[1.45, 45.58],\n"
        "  [-0.35, 999.88]],\n"
        " [[-12.75, 43.46],\n"
        "  [0.00, 10.00]]]";
    EXPECT_EQ(expected, image3DStr);
}

TEST(ImageTest, image3DToStringWithNegativePrecision){
    const std::string image3DStr = yagit::image3DToString(IMAGE, -2);

    const std::string expected =
        "[[[1.45364, 45.5782],\n"
        "  [-0.34921, 999.877]],\n"
        " [[-12.7483, 43.459],\n"
        "  [0, 10]]]";
    EXPECT_EQ(expected, image3DStr);
}

TEST(ImageTest, image3DToStringForSpecialValues){
    const std::string image3DStr = yagit::image3DToString(IMAGE_SPECIAL_VALUES);

    const std::string expected = "[[[nan, inf, -inf]]]";
    EXPECT_EQ(expected, image3DStr);
}

TEST(ImageTest, image3DToStringForNonuniformImage){
    const std::string image3DStr = yagit::image3DToString(IMAGE_NONUNIFORM);

    const std::string expected =
        "[[[1],\n"
        "  [2, 3],\n"
        "  [4, 5, 6]],\n"
        " [[7, 8, 9, 10]]]";
    EXPECT_EQ(expected, image3DStr);
}
