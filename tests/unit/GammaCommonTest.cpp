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

#include "../src/gamma/GammaCommon.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Contains, ::testing::FloatEq;
using ::testing::Matcher, ::testing::AllOf, ::testing::Field, ::testing::FieldsAre;

Matcher<yagit::Point2D> matchPoint2D(const yagit::Point2D& expectedPoint){
    return AllOf(Field("y", &yagit::Point2D::y, FloatEq(expectedPoint.y)),
                 Field("x", &yagit::Point2D::x, FloatEq(expectedPoint.x)),
                 Field("distSq", &yagit::Point2D::distSq, FloatEq(expectedPoint.distSq)));
}

Matcher<yagit::Point3D> matchPoint3D(const yagit::Point3D& expectedPoint){
    return AllOf(Field("z", &yagit::Point3D::z, FloatEq(expectedPoint.z)),
                 Field("y", &yagit::Point3D::y, FloatEq(expectedPoint.y)),
                 Field("x", &yagit::Point3D::x, FloatEq(expectedPoint.x)),
                 Field("distSq", &yagit::Point3D::distSq, FloatEq(expectedPoint.distSq)));
}

TEST(GammaCommonTest, distSq1D){
    EXPECT_EQ(4, yagit::distSq1D(0, 2));
    EXPECT_EQ(1, yagit::distSq1D(2, 3));
    EXPECT_EQ(16, yagit::distSq1D(5, 1));
    EXPECT_EQ(0, yagit::distSq1D(4, 4));
}

TEST(GammaCommonTest, distSq1DWithNegativeNumbers){
    EXPECT_EQ(4, yagit::distSq1D(-3, -5));
    EXPECT_EQ(25, yagit::distSq1D(2, -3));
}

TEST(GammaCommonTest, distSq1DWithFloatingPointNumbers){
    EXPECT_FLOAT_EQ(19.36, yagit::distSq1D(1.2, 5.6));
    EXPECT_FLOAT_EQ(32.49, yagit::distSq1D(2.3, -3.4));
}

TEST(GammaCommonTest, distSq2D){
    EXPECT_EQ(8, yagit::distSq2D(1, 2, 3, 4));
    EXPECT_EQ(2, yagit::distSq2D(2, 5, 3, 4));
    EXPECT_EQ(0, yagit::distSq2D(3, 4, 3, 4));
}

TEST(GammaCommonTest, distSq2DWithNegativeNumbers){
    EXPECT_EQ(41, yagit::distSq2D(-1, -3, -6, -7));
    EXPECT_EQ(164, yagit::distSq2D(4, -3, -6, 5));
}

TEST(GammaCommonTest, distSq2DWithFloatingPointNumbers){
    EXPECT_FLOAT_EQ(3.69, yagit::distSq2D(3.4, 5.6, 1.9, 6.8));
    EXPECT_FLOAT_EQ(74.44 , yagit::distSq2D(2.9, -5.7, -3.3, 0.3));
}

TEST(GammaCommonTest, distSq3D){
    EXPECT_EQ(27, yagit::distSq3D(1, 2, 3, 4, 5, 6));
    EXPECT_EQ(45, yagit::distSq3D(6, 8, 9, 4, 3, 5));
    EXPECT_EQ(0, yagit::distSq3D(3, 6, 7, 3, 6, 7));
}

TEST(GammaCommonTest, distSq3DWithNegativeNumbers){
    EXPECT_EQ(49, yagit::distSq3D(-1, -3, -6, -7, -1, -9));
    EXPECT_EQ(165, yagit::distSq3D(-4, 3, -8, 3, -1, 2));
}

TEST(GammaCommonTest, distSq3DWithFloatingPointNumbers){
    EXPECT_FLOAT_EQ(65.97, yagit::distSq3D(6.7, 3.1, 9.2, 0.4, 1.3, 4.4));
    EXPECT_FLOAT_EQ(186.46, yagit::distSq3D(-5.1, 4.8, -8.8, 0.3, -2.9, 1.1));
}

TEST(GammaCommonTest, sortByDistanceAscYXPos){
    #pragma warning(push)
    #pragma warning(disable : 4244)
    std::vector<yagit::Point2D> points{
        {-2, -1, 5}, {3, 0, 9}, {0, 0, 0}, {2, 0, 4}, {-1, 1, 2}
    };
    yagit::sortByDistanceAsc(points);

    const std::vector<yagit::Point2D> expected{
        {0, 0, 0}, {-1, 1, 2}, {2, 0, 4}, {-2, -1, 5}, {3, 0, 9}
    };
    #pragma warning(pop)

    EXPECT_EQ(expected, points);
}

TEST(GammaCommonTest, sortByDistanceAscZYXPos){
    #pragma warning(push)
    #pragma warning(disable : 4244)
    std::vector<yagit::Point3D> points{
        {0, 0, -1, 1}, {1, -2, 3, 14}, {2, 0, 0, 4}, {0, 0, 0, 0}, {-1, 1, 0, 2}
    };
    yagit::sortByDistanceAsc(points);

    const std::vector<yagit::Point3D> expected{
        {0, 0, 0, 0}, {0, 0, -1, 1}, {-1, 1, 0, 2}, {2, 0, 0, 4}, {1, -2, 3, 14}
    };
    #pragma warning(pop)

    EXPECT_EQ(expected, points);
}

TEST(GammaCommonTest, sortedPointsInCircle){
    const auto sortedPoints = yagit::sortedPointsInCircle(3, 1);

    #pragma warning(push)
    #pragma warning(disable : 4244)
    std::vector<yagit::Point2D> expected{
        {0, 0, 0},
        {1, 0, 1}, {-1, 0, 1},
        {0, 1, 1}, {0, -1, 1},
        {1, 1, 2}, {-1, -1, 2}, {-1, 1, 2}, {1, -1, 2},
        {2, 0, 4}, {-2, 0, 4},
        {0, 2, 4}, {0, -2, 4},
        {2, 1, 5}, {-2, -1, 5}, {-2, 1, 5}, {2, -1, 5},
        {1, 2, 5}, {-1, -2, 5}, {-1, 2, 5}, {1, -2, 5},
        {2, 2, 8}, {-2, -2, 8}, {-2, 2, 8}, {2, -2, 8},
        {3, 0, 9}, {-3, 0, 9},
        {0, 3, 9}, {0, -3, 9}
    };
    #pragma warning(pop)

    // expected vector is sorted, but some compilers sort points with the same distance in a different way,
    // so we sort it here again
    yagit::sortByDistanceAsc(expected);

    EXPECT_EQ(expected, sortedPoints);
}

TEST(GammaCommonTest, sortedPointsInCircleShouldContainPointsOnEdge){
    const auto sortedPoints = yagit::sortedPointsInCircle(1, 0.1);

    EXPECT_EQ(317, sortedPoints.size());

    #pragma warning(push)
    #pragma warning(disable : 4244)
    EXPECT_THAT(sortedPoints, Contains(matchPoint2D({0.6f, 0.8f, 1})));
    EXPECT_THAT(sortedPoints, Contains(matchPoint2D({1, 0, 1})));
    EXPECT_THAT(sortedPoints, Contains(matchPoint2D({0, 1, 1})));
    #pragma warning(pop)
}

TEST(GammaCommonTest, sortedPointsInSphere){
    const auto sortedPoints = yagit::sortedPointsInSphere(2, 1);

    #pragma warning(push)
    #pragma warning(disable : 4244)
    std::vector<yagit::Point3D> expected{
        {0, 0, 0, 0},
        {1, 0, 0, 1}, {-1, 0, 0, 1},
        {0, 1, 0, 1}, {0, -1, 0, 1},
        {0, 0, 1, 1}, {0, 0, -1, 1},
        {1, 1, 0, 2}, {-1, -1, 0, 2}, {-1, 1, 0, 2}, {1, -1, 0, 2},
        {1, 0, 1, 2}, {-1, 0, -1, 2}, {-1, 0, 1, 2}, {1, 0, -1, 2},
        {0, 1, 1, 2}, {0, -1, -1, 2}, {0, -1, 1, 2}, {0, 1, -1, 2},
        {1, 1, 1, 3}, {-1, -1, -1, 3}, {-1, -1, 1, 3}, {-1, 1, -1, 3}, {1, -1, -1, 3},
            {-1, 1, 1, 3}, {1, -1, 1, 3}, {1, 1, -1, 3},
        {2, 0, 0, 4}, {-2, 0, 0, 4},
        {0, 2, 0, 4}, {0, -2, 0, 4},
        {0, 0, 2, 4}, {0, 0, -2, 4}
    };
    #pragma warning(pop)

    // expected vector is sorted, but some compilers sort points with the same distance in a different way,
    // so we sort it here again
    yagit::sortByDistanceAsc(expected);

    EXPECT_EQ(expected, sortedPoints);
}

TEST(GammaCommonTest, sortedPointsInSphereShouldContainPointsOnEdge){
    const auto sortedPoints = yagit::sortedPointsInSphere(1, 0.1);

    EXPECT_EQ(4169, sortedPoints.size());

    #pragma warning(push)
    #pragma warning(disable : 4244)
    EXPECT_THAT(sortedPoints, Contains(matchPoint3D({0.6f, 0.8f, 0.0f, 1})));
    EXPECT_THAT(sortedPoints, Contains(matchPoint3D({1, 0, 0, 1})));
    EXPECT_THAT(sortedPoints, Contains(matchPoint3D({0, 1, 0, 1})));
    EXPECT_THAT(sortedPoints, Contains(matchPoint3D({0, 0, 1, 1})));
    #pragma warning(pop)
}
