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

#include "yagit/Gamma.hpp"

#include <tuple>
#include <limits>

#include <gtest/gtest.h>
#include "TestUtils.hpp"

namespace{
const float NaN = std::numeric_limits<float>::quiet_NaN();
const float MAX_ABS_ERROR{1e-6};
const float MAX_ABS_ERROR2{2e-6};

const yagit::Image2D REF_IMAGE_2D = {
    {0.93, 0.95},
    {0.97, 1.00}
};
const yagit::Image2D EVAL_IMAGE_2D = {
    {0.95, 0.97},
    {1.00, 1.03}
};

const yagit::Image3D REF_IMAGE_3D = {
    {{0.2, 0.64, 0.3},
     {0.5, 0.43, 0.6}},
    {{0.4, 0.7, 0.28},
     {1.4, 0.8, 0.9}}
};
const yagit::Image3D EVAL_IMAGE_3D = {
    {{0.24, 0.68, 0.2},
     {0.67, 0.9, 0.6}},
    {{1.0, 0.8, 0.34},
     {0.8, 0.99, 0.83}}
};

const yagit::ImageData REF_2D(REF_IMAGE_2D, {0, 0, -1}, {1, 1, 1});
const yagit::ImageData EVAL_2D(EVAL_IMAGE_2D, {0, -1, 0}, {1, 1, 1});
const yagit::ImageData REF_3D(REF_IMAGE_3D, {-0.2, -5.8, 4.4}, {1.5, 2, 2.5});
const yagit::ImageData EVAL_3D(EVAL_IMAGE_3D, {-0.3, -6.0, 4.5}, {1.5, 2, 2.5});

const yagit::ImageData ZERO_2D = generateImageData(0, REF_2D.getSize(), REF_2D.getOffset(), REF_2D.getSpacing());
const yagit::ImageData ZERO_3D = generateImageData(0, REF_3D.getSize(), REF_3D.getOffset(), REF_3D.getSpacing());

const float REF_2D_MAX = REF_2D.max();
const float REF_3D_MAX = REF_3D.max();

const yagit::GammaParameters GAMMA_PARAMS_2D{3, 3, yagit::GammaNormalization::Global, REF_2D_MAX, 0, 10, 0.3};
const yagit::GammaParameters GAMMA_PARAMS_3D{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 10, 0.3};
const yagit::GammaParameters INCORRECT_GAMMA_PARAMS1{0, 3, yagit::GammaNormalization::Global, 10, 0};
const yagit::GammaParameters INCORRECT_GAMMA_PARAMS2{3, 0, yagit::GammaNormalization::Global, 10, 0};
const yagit::GammaParameters INCORRECT_GAMMA_PARAMS3{3, 0, static_cast<yagit::GammaNormalization>(20), 10, 0};
const yagit::GammaParameters INCORRECT_GAMMA_PARAMS4{3, 0, yagit::GammaNormalization::Global, -1, 0};
const yagit::GammaParameters INCORRECT_GAMMA_PARAMS5{3, 3, yagit::GammaNormalization::Global, 10, 0, 0, 0};
const yagit::GammaParameters INCORRECT_GAMMA_PARAMS6{3, 3, yagit::GammaNormalization::Global, 10, 0, 10, 0};
const yagit::GammaParameters INCORRECT_GAMMA_PARAMS7{3, 3, yagit::GammaNormalization::Global, 10, 0, 10, 12};

using GammaParametric2D = std::tuple<yagit::GammaParameters, yagit::Image2D>;
using GammaParametric3D = std::tuple<yagit::GammaParameters, yagit::Image3D>;
}

const GammaParametric2D gammaIndex2DClassicTestValues[] = {
    // GAMMA PARAMETERS                                             EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_2D_MAX, 0},      {{0.816496, 0.333333},
                                                                     {0.942809, 0.333333}}},

    {{2, 1, yagit::GammaNormalization::Global, REF_2D_MAX, 0},      {{1.732050, 1.000000},
                                                                     {2.061552, 1.000000}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0},                {{0.857956, 0.333333},
                                                                     {0.942809, 0.333333}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0},                {{1.776570, 1.000000},
                                                                     {2.095548, 1.000000}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_2D_MAX, 0.95},   {{NaN,      0.333333},
                                                                     {0.942809, 0.333333}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.95},             {{NaN,      0.333333},
                                                                     {0.942809, 0.333333}}}
};

class GammaIndex2DClassicTest : public ::testing::TestWithParam<GammaParametric2D> {};

INSTANTIATE_TEST_SUITE_P(GammaTest, GammaIndex2DClassicTest, ::testing::ValuesIn(gammaIndex2DClassicTestValues));

TEST_P(GammaIndex2DClassicTest, gammaIndex2DClassic){
    const auto& [gammaParams, expectedGamma] = GetParam();
    yagit::GammaResult gammaRes = yagit::gammaIndex2DClassic(REF_2D, EVAL_2D, gammaParams);

    yagit::GammaResult expectedGammaRes(expectedGamma, REF_2D.getOffset(), REF_2D.getSpacing());
    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2DClassicForImagesWithDifferentSpacings){
    const yagit::ImageData refImg(REF_IMAGE_2D, {0, 0, 0}, {1, 2, 3});
    const yagit::ImageData evalImg(EVAL_IMAGE_2D, {0, 0, 0}, {4, 5, 6});

    const auto gammaRes = yagit::gammaIndex2DClassic(refImg, evalImg, GAMMA_PARAMS_2D);

    const yagit::Image2D expected = {
        {0.666666, 1.000000},
        {0.942810, 1.414214}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}


const GammaParametric3D gammaIndex2_5DClassicTestValues[] = {
    // GAMMA PARAMETERS                                            EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0},     {{{0.955874, 0.955875, 2.171209},
                                                                     {2.926515, 4.140035, 0.081650}},
                                                                    {{2.221795, 2.382353, 1.430903},
                                                                     {9.552117, 0.734847, 1.668665}}}},

    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0},     {{{1.449419, 1.449420, 3.579819},
                                                                     {6.076368, 6.608498, 0.244949}},
                                                                    {{5.536410, 3.579819, 2.156812},
                                                                     {14.454812, 2.204540, 2.511971}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0},               {{{1.701633, 1.855404, 6.864241},
                                                                     {6.880408, 13.206973, 0.081650}},
                                                                    {{5.281624, 4.762606, 7.143324},
                                                                     {9.552117, 0.734847, 2.593878}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0},               {{{5.104899, 3.134587, 11.138225},
                                                                     {11.227647, 19.938953, 0.244949}},
                                                                    {{9.072486, 7.147058, 10.717086},
                                                                     {14.454812, 2.204540, 3.896595}}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0.4},   {{{NaN,      0.955875, NaN},
                                                                     {2.926515, 4.140035, 0.081650}},
                                                                    {{2.221795, 2.382353, NaN},
                                                                     {9.552117, 0.734847, 1.668665}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.4},             {{{NaN,      1.855404, NaN},
                                                                     {6.880408, 13.206973, 0.081650}},
                                                                    {{5.281624, 4.762606, NaN},
                                                                     {9.552117, 0.734847, 2.593878}}}},
};

class GammaIndex2_5DClassicTest : public ::testing::TestWithParam<GammaParametric3D> {};

INSTANTIATE_TEST_SUITE_P(GammaTest, GammaIndex2_5DClassicTest, ::testing::ValuesIn(gammaIndex2_5DClassicTestValues));

TEST_P(GammaIndex2_5DClassicTest, gammaIndex2_5DClassic){
    const auto& [gammaParams, expectedGamma] = GetParam();
    yagit::GammaResult gammaRes = yagit::gammaIndex2_5DClassic(REF_3D, EVAL_3D, gammaParams);

    yagit::GammaResult expectedGammaRes(expectedGamma, REF_3D.getOffset(), REF_3D.getSpacing());
    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DClassicForImagesWithDifferentSpacings){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {1, 2, 3});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {0, 0, 0}, {4, 5, 6});

    const auto gammaRes = yagit::gammaIndex2_5DClassic(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{0.952381, 1.380953, 2.457807},
         {4.169319, 4.680737, 2.018059}},
        {{4.363578, 2.769284, 2.653454},
         {9.599343, 1.563472, 2.567458}}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}


const GammaParametric3D gammaIndex3DClassicTestValues[] = {
    // GAMMA PARAMETERS                                            EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0},     {{{0.955874, 0.955875, 1.063185},
                                                                     {2.926515, 2.469515, 0.081650}},
                                                                    {{2.221795, 0.718858, 1.430903},
                                                                     {9.552117, 0.734847, 0.963789}}}},

    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0},     {{{1.449419, 1.449420, 2.012664},
                                                                     {6.076368, 4.887907, 0.244949}},
                                                                    {{5.536410, 1.766410, 2.156812},
                                                                     {14.454812, 2.204540, 2.511971}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0},               {{{1.701633, 1.855404, 4.469498},
                                                                     {6.880408, 7.083899, 0.081650}},
                                                                    {{5.281624, 1.094088, 5.062846},
                                                                     {9.552117, 0.734847, 0.963789}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0},               {{{5.104899, 3.134587, 6.815749},
                                                                     {11.227647, 11.094082, 0.244949}},
                                                                    {{9.072486, 2.156574, 8.810813},
                                                                     {14.454812, 2.204540, 2.891366}}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0.4},   {{{NaN,      0.955875, NaN},
                                                                     {2.926515, 2.469515, 0.081650}},
                                                                    {{2.221795, 0.718858, NaN},
                                                                     {9.552117, 0.734847, 0.963789}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.4},             {{{NaN,      1.855404, NaN},
                                                                     {6.880408, 7.083899, 0.081650}},
                                                                    {{5.281624, 1.094088, NaN},
                                                                     {9.552117, 0.734847, 0.963789}}}},
};

class GammaIndex3DClassicTest : public ::testing::TestWithParam<GammaParametric3D> {};

INSTANTIATE_TEST_SUITE_P(GammaTest, GammaIndex3DClassicTest, ::testing::ValuesIn(gammaIndex3DClassicTestValues));

TEST_P(GammaIndex3DClassicTest, gammaIndex3DClassic){
    const auto& [gammaParams, expectedGamma] = GetParam();
    yagit::GammaResult gammaRes = yagit::gammaIndex3DClassic(REF_3D, EVAL_3D, gammaParams);

    yagit::GammaResult expectedGammaRes(expectedGamma, REF_3D.getOffset(), REF_3D.getSpacing());
    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex3DClassicForImagesWithDifferentSpacings){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {1, 2, 3});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {0, 0, 0}, {4, 5, 6});

    const auto gammaRes = yagit::gammaIndex3DClassic(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{0.952381, 1.380953, 2.457807},
         {4.169319, 3.976690, 2.018059}},
        {{3.824080, 1.156662, 2.240121},
         {9.599343, 1.563472, 1.054093}}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}


const GammaParametric2D gammaIndex2DWendlingTestValues[] = {
    // GAMMA PARAMETERS                                                     EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_2D_MAX, 0, 5, 0.3},      {{1.094246, 0.343188},
                                                                             {0.721880, 0.415746}}},

    {{2, 1, yagit::GammaNormalization::Global, REF_2D_MAX, 0, 5, 0.1},      {{1.732050, 0.934077},
                                                                             {1.791647, 1.000000}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0, 5, 0.3},                {{1.159896, 0.347532},
                                                                             {0.726698, 0.415746}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 5, 0.1},                {{1.776570, 0.937684},
                                                                             {1.796024, 1.000000}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_2D_MAX, 0.95, 5, 0.3},   {{NaN,      0.343188},
                                                                             {0.721880, 0.415746}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.95, 5, 0.3},             {{NaN,      0.347532},
                                                                             {0.726698, 0.415746}}},

    // small max search distance
    {{2, 1, yagit::GammaNormalization::Global, REF_2D_MAX, 0, 1.2, 0.1},    {{2.315166, 0.934077},
                                                                             {NaN,      1.000000}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 1.2, 0.1},              {{2.446386, 0.937684},
                                                                             {NaN,      1.000000}}}
};

class GammaIndex2DWendlingTest : public ::testing::TestWithParam<GammaParametric2D> {};

INSTANTIATE_TEST_SUITE_P(GammaTest, GammaIndex2DWendlingTest, ::testing::ValuesIn(gammaIndex2DWendlingTestValues));

TEST_P(GammaIndex2DWendlingTest, gammaIndex2DWendling){
    const auto& [gammaParams, expectedGamma] = GetParam();
    yagit::GammaResult gammaRes = yagit::gammaIndex2DWendling(REF_2D, EVAL_2D, gammaParams);

    yagit::GammaResult expectedGammaRes(expectedGamma, REF_2D.getOffset(), REF_2D.getSpacing());
    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2DWendlingForImagesWithDifferentSpacings){
    const yagit::ImageData refImg(REF_IMAGE_2D, {0, 0, 0}, {1, 2, 3});
    const yagit::ImageData evalImg(EVAL_IMAGE_2D, {0, 0, 0}, {4, 5, 6});

    const auto gammaRes = yagit::gammaIndex2DWendling(refImg, evalImg, GAMMA_PARAMS_2D);

    const yagit::Image2D expected = {
        {0.666666, 0.316228},
        {0.000000, 0.389267}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}


const GammaParametric3D gammaIndex2_5DWendlingTestValues[] = {
    // GAMMA PARAMETERS                                                      EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 10, 0.3},      {{{2.074477, 0.235322, 0.568673},
                                                                               {0.472046, 0.849464, 0.195100}},
                                                                              {{NaN,      NaN,      NaN},
                                                                               {NaN,      NaN,      NaN}}}},

    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 10, 0.1},      {{{3.245806, 0.283074, 0.264869},
                                                                               {1.114044, 2.546287, 0.362581}},
                                                                              {{NaN,      NaN,      NaN},
                                                                               {NaN,      NaN,      NaN}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0, 10, 0.3},                {{{8.494074, 0.277237, 2.614365},
                                                                               {0.669498, 0.858401, 0.403476}},
                                                                              {{NaN,      NaN,      NaN},
                                                                               {NaN,      NaN,      NaN}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 10, 0.1},                {{{5.612027, 0.283946, 0.847239},
                                                                               {1.177018, 2.553020, 0.414489}},
                                                                              {{NaN,      NaN,      NaN},
                                                                               {NaN,      NaN,      NaN}}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0.4, 10, 0.3},    {{{NaN,      0.235322, NaN},
                                                                               {0.472046, 0.849464, 0.195100}},
                                                                              {{NaN,      NaN,      NaN},
                                                                               {NaN,      NaN,      NaN}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.4, 10, 0.3},              {{{NaN,      0.277237, NaN},
                                                                               {0.669498, 0.858401, 0.403476}},
                                                                              {{NaN,      NaN,      NaN},
                                                                               {NaN,      NaN,      NaN}}}},

    // small max search distance
    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 1.2, 0.1},     {{{3.245806, 0.283074, 0.264869},
                                                                               {1.114044, 10.994404, 0.362581}},
                                                                              {{NaN,      NaN,       NaN},
                                                                               {NaN,      NaN,       NaN}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 1.2, 0.1},               {{{22.667768, 0.283946, 0.847239},
                                                                               {1.177018, 35.612892, 0.414489}},
                                                                              {{NaN,      NaN,       NaN},
                                                                               {NaN,      NaN,       NaN}}}}
};

class GammaIndex2_5DWendlingTest : public ::testing::TestWithParam<GammaParametric3D> {};

INSTANTIATE_TEST_SUITE_P(GammaTest, GammaIndex2_5DWendlingTest, ::testing::ValuesIn(gammaIndex2_5DWendlingTestValues));

TEST_P(GammaIndex2_5DWendlingTest, gammaIndex2_5DWendling){
    const auto& [gammaParams, expectedGamma] = GetParam();
    yagit::GammaResult gammaRes = yagit::gammaIndex2_5DWendling(REF_3D, EVAL_3D, gammaParams);

    yagit::GammaResult expectedGammaRes(expectedGamma, REF_3D.getOffset(), REF_3D.getSpacing());
    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DWendlingForImagesWithDifferentSpacings){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {1, 2, 3});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {0, 0, 0}, {4, 5, 6});

    const auto gammaRes = yagit::gammaIndex2_5DWendling(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{0.952381, 0.649612, 1.573772},
         {0.293681, 0.565945, 0.642857}},
        {{0.714286, 0.650642, 1.800984},
         {11.586858, 0.707332, 0.794118}}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DWendlingForEvalImageShiftedOneFrameUpFromRefImg){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {2, 2, 2});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {-2, 0, 0}, {2, 2, 2});

    const auto res = yagit::gammaIndex2_5DWendling(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{4.092896, 0.318961, 0.952381},
         {1.459044, 0.849863, 0.375715}},
        {{NaN,      NaN,      NaN},
         {NaN,      NaN,      NaN}}
    };
    yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());
    EXPECT_THAT(res, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DWendlingForEvalImageShiftedOneFrameDownFromRefImg){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {2, 2, 2});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {2, 0, 0}, {2, 2, 2});

    const auto res = yagit::gammaIndex2_5DWendling(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{NaN,      NaN,      NaN},
         {NaN,      NaN,      NaN}},
        {{0.648127, 0.325277, 0.215131},
         {12.281869, 0.300943, 0.751646}}
    };
    yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());
    EXPECT_THAT(res, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DWendlingForEvalImageShiftedTwoFramesUpFromRefImg){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {2, 2, 2});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {-4, 0, 0}, {2, 2, 2});

    const auto res = yagit::gammaIndex2_5DWendling(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{NaN, NaN, NaN},
         {NaN, NaN, NaN}},
        {{NaN, NaN, NaN},
         {NaN, NaN, NaN}}
    };
    yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());
    EXPECT_THAT(res, matchImageData(expectedGammaRes));
}

TEST(GammaTest, gammaIndex2_5DWendlingForEvalImageShiftedTwoFramesDownFromRefImg){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {2, 2, 2});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {4, 0, 0}, {2, 2, 2});

    const auto res = yagit::gammaIndex2_5DWendling(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{NaN, NaN, NaN},
         {NaN, NaN, NaN}},
        {{NaN, NaN, NaN},
         {NaN, NaN, NaN}},
    };
    yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());
    EXPECT_THAT(res, matchImageData(expectedGammaRes));
}


const GammaParametric3D gammaIndex3DWendlingTestValues[] = {
    // GAMMA PARAMETERS                                                      EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 10, 0.3},      {{{2.074477, 0.231435, 0.112103},
                                                                               {0.472046, 0.849464, 0.195100}},
                                                                              {{0.554398, 0.342783, 0.572041},
                                                                               {10.501846, 0.485433, 0.520314}}}},

    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 10, 0.1},      {{{1.449419, 0.245707, 0.152745},
                                                                               {1.012473, 2.492108, 0.244949}},
                                                                              {{1.332712, 0.663393, 0.748604},
                                                                               {14.454812, 1.366185, 0.941339}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0, 10, 0.3},                {{{8.494074, 0.258932, 0.256719},
                                                                               {0.669498, 0.858401, 0.403476}},
                                                                              {{0.708956, 0.437578, 1.476741},
                                                                               {10.501846, 0.517604, 0.521304}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 10, 0.1},                {{{5.104897, 0.248556, 0.302656},
                                                                               {1.029564, 2.541528, 0.244949}},
                                                                              {{1.443461, 0.663598, 1.024796},
                                                                               {14.454812, 1.378097, 0.985449}}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0.4, 10, 0.3},    {{{NaN,      0.231435, NaN},
                                                                               {0.472046, 0.849464, 0.195100}},
                                                                              {{0.554398, 0.342783, NaN},
                                                                               {10.501846, 0.485433, 0.520314}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.4, 10, 0.3},              {{{NaN,      0.258932, NaN},
                                                                               {0.669498, 0.858401, 0.403476}},
                                                                              {{0.708956, 0.437578, NaN},
                                                                               {10.501846, 0.517604, 0.521304}}}},

    // small max search distance
    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 1.2, 0.1},     {{{1.449419, 0.245707, 0.152745},
                                                                               {1.012473, 10.567875, 0.244949}},
                                                                              {{3.517255, 0.663393, 0.748604},
                                                                               {18.190107, 1.794948, 0.941339}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 1.2, 0.1},               {{{10.002997, 0.248556, 0.302656},
                                                                               {1.029564, 34.215370, 0.244949}},
                                                                              {{11.720528, 0.663598, 1.024796},
                                                                               {18.190107, 2.626190, 0.985449}}}}
};

class GammaIndex3DWendlingTest : public ::testing::TestWithParam<GammaParametric3D> {};

INSTANTIATE_TEST_SUITE_P(GammaTest, GammaIndex3DWendlingTest, ::testing::ValuesIn(gammaIndex3DWendlingTestValues));

TEST_P(GammaIndex3DWendlingTest, gammaIndex3DWendling){
    const auto& [gammaParams, expectedGamma] = GetParam();
    yagit::GammaResult gammaRes = yagit::gammaIndex3DWendling(REF_3D, EVAL_3D, gammaParams);

    yagit::GammaResult expectedGammaRes(expectedGamma, REF_3D.getOffset(), REF_3D.getSpacing());
    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex3DWendlingForImagesWithDifferentSpacings){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {1, 2, 3});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {0, 0, 0}, {4, 5, 6});

    const auto gammaRes = yagit::gammaIndex3DWendling(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{0.952381, 0.497177, 1.573772},
         {0.234738, 0.565945, 0.642857}},
        {{0.233715, 0.383646, 1.692145},
         {9.784037, 0.528611, 0.689143}}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR));
}


TEST(GammaTest, gammaIndex2DForClassicMethod){
    const yagit::Image2D expectedImage = {
        {0.816496, 0.333333},
        {0.942809, 0.333333}
    };

    const yagit::GammaResult gammaRes = yagit::gammaIndex2D(REF_2D, EVAL_2D, GAMMA_PARAMS_2D, yagit::GammaMethod::Classic);
    const yagit::ImageData expected(expectedImage, REF_2D.getOffset(), REF_2D.getSpacing());
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2DForWendlingMethod){
    const yagit::Image2D expectedImage = {
        {1.094246, 0.343188},
        {0.721880, 0.415746}
    };
    const yagit::ImageData expected(expectedImage, REF_2D.getOffset(), REF_2D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex2D(REF_2D, EVAL_2D, GAMMA_PARAMS_2D, yagit::GammaMethod::Wendling);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DForClassicMethod){
    const yagit::Image3D expectedImage = {
        {{0.955874, 0.955875, 2.171209},
         {2.926515, 4.140035, 0.081650}},
        {{2.221795, 2.382353, 1.430903},
         {9.552117, 0.734847, 1.668665}}
    };
    const yagit::ImageData expected(expectedImage, REF_3D.getOffset(), REF_3D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex2_5D(REF_3D, EVAL_3D, GAMMA_PARAMS_3D, yagit::GammaMethod::Classic);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DForWendlingMethod){
    const yagit::Image3D expectedImage = {
        {{2.074477, 0.235322, 0.568673},
         {0.472046, 0.849464, 0.195100}},
        {{NaN,      NaN,      NaN},
         {NaN,      NaN,      NaN}}
    };
    const yagit::ImageData expected(expectedImage, REF_3D.getOffset(), REF_3D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex2_5D(REF_3D, EVAL_3D, GAMMA_PARAMS_3D, yagit::GammaMethod::Wendling);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex3DForClassicMethod){
    const yagit::Image3D expectedImage = {
        {{0.955874, 0.955875, 1.063185},
         {2.926515, 2.469515, 0.081650}},
        {{2.221795, 0.718858, 1.430903},
         {9.552117, 0.734847, 0.963789}}
    };
    const yagit::ImageData expected(expectedImage, REF_3D.getOffset(), REF_3D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex3D(REF_3D, EVAL_3D, GAMMA_PARAMS_3D, yagit::GammaMethod::Classic);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex3DForWendlingMethod){
    const yagit::Image3D expectedImage = {
        {{2.074477, 0.231435, 0.112103},
         {0.472046, 0.849464, 0.195100}},
        {{0.554398, 0.342783, 0.572041},
         {10.501846, 0.485433, 0.520314}}
    };
    const yagit::ImageData expected(expectedImage, REF_3D.getOffset(), REF_3D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex3D(REF_3D, EVAL_3D, GAMMA_PARAMS_3D, yagit::GammaMethod::Wendling);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2DClassicForTheSameImagesShouldReturnImageFilledWithZeros){
    EXPECT_THAT(yagit::gammaIndex2DClassic(REF_2D, REF_2D, GAMMA_PARAMS_2D), matchImageData(ZERO_2D, MAX_ABS_ERROR2));
}

TEST(GammaTest, gammaIndex2DWendlingForTheSameImagesShouldReturnImageFilledWithZeros){
    EXPECT_THAT(yagit::gammaIndex2DWendling(REF_2D, REF_2D, GAMMA_PARAMS_2D), matchImageData(ZERO_2D, MAX_ABS_ERROR2));
}

TEST(GammaTest, gammaIndex2_5DClassicForTheSameImagesShouldReturnImageFilledWithZeros){
    EXPECT_THAT(yagit::gammaIndex2_5DClassic(REF_3D, REF_3D, GAMMA_PARAMS_3D), matchImageData(ZERO_3D, MAX_ABS_ERROR2));
}

TEST(GammaTest, gammaIndex2_5DWendlingForTheSameImagesShouldReturnImageFilledWithZeros){
    EXPECT_THAT(yagit::gammaIndex2_5DWendling(REF_3D, REF_3D, GAMMA_PARAMS_3D), matchImageData(ZERO_3D, MAX_ABS_ERROR2));
}

TEST(GammaTest, gammaIndex3DClassicForTheSameImagesShouldReturnImageFilledWithZeros){
    EXPECT_THAT(yagit::gammaIndex3DClassic(REF_3D, REF_3D, GAMMA_PARAMS_3D), matchImageData(ZERO_3D, MAX_ABS_ERROR2));
}

TEST(GammaTest, gammaIndex3DWendlingForTheSameImagesShouldReturnImageFilledWithZeros){
    EXPECT_THAT(yagit::gammaIndex3DWendling(REF_3D, REF_3D, GAMMA_PARAMS_3D), matchImageData(ZERO_3D, MAX_ABS_ERROR2));
}

TEST(GammaTest, gammaIndex2DClassicAndWendlingForCorrespondingParametersShouldReturnTheSameImage){
    const float spacing = 2;
    const yagit::DataSpacing dataSpacing{spacing, spacing, spacing};

    const yagit::ImageData refImg(REF_IMAGE_2D, {0, 0, 0}, dataSpacing);
    const yagit::ImageData evalImg(EVAL_IMAGE_2D, {0, 0, 0}, dataSpacing);
    const yagit::GammaParameters gammaParams{3, 3, yagit::GammaNormalization::Global, refImg.max(), 0, 10, spacing};

    const auto classicRes = yagit::gammaIndex2DClassic(refImg, evalImg, gammaParams);
    const auto wendlingRes = yagit::gammaIndex2DWendling(refImg, evalImg, gammaParams);

    EXPECT_THAT(classicRes, matchImageData(wendlingRes));
}

TEST(GammaTest, gammaIndex2_5DClassicAndWendlingForCorrespondingParametersShouldReturnTheSameImage){
    const float spacing = 2;
    const yagit::DataSpacing dataSpacing{spacing, spacing, spacing};

    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, dataSpacing);
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {0, 0, 0}, dataSpacing);
    const yagit::GammaParameters gammaParams{3, 3, yagit::GammaNormalization::Global, refImg.max(), 0, 10, spacing};

    const auto classicRes = yagit::gammaIndex2_5DClassic(refImg, evalImg, gammaParams);
    const auto wendlingRes = yagit::gammaIndex2_5DWendling(refImg, evalImg, gammaParams);

    EXPECT_THAT(classicRes, matchImageData(wendlingRes));
}

TEST(GammaTest, gammaIndex3DClassicAndWendlingForCorrespondingParametersShouldReturnTheSameImage){
    const float spacing = 2;
    const yagit::DataSpacing dataSpacing{spacing, spacing, spacing};

    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, dataSpacing);
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {0, 0, 0}, dataSpacing);
    const yagit::GammaParameters gammaParams{3, 3, yagit::GammaNormalization::Global, refImg.max(), 0, 10, spacing};

    const auto classicRes = yagit::gammaIndex3DClassic(refImg, evalImg, gammaParams);
    const auto wendlingRes = yagit::gammaIndex3DWendling(refImg, evalImg, gammaParams);

    EXPECT_THAT(classicRes, matchImageData(wendlingRes));
}

TEST(GammaTest, gammaIndex2DClassicFor3DImageShouldThrow){
    EXPECT_THROW(yagit::gammaIndex2DClassic(REF_3D, EVAL_2D, GAMMA_PARAMS_2D), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DClassic(REF_2D, EVAL_3D, GAMMA_PARAMS_2D), std::invalid_argument);
}

TEST(GammaTest, gammaIndex2DClassicForIncorrectParametersShouldThrow){
    EXPECT_THROW(yagit::gammaIndex2DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS1), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS2), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS3), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS4), std::invalid_argument);
}

TEST(GammaTest, gammaIndex2_5DClassicForIncorrectParametersShouldThrow){
    EXPECT_THROW(yagit::gammaIndex2_5DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS1), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2_5DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS2), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2_5DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS3), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2_5DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS4), std::invalid_argument);
}

TEST(GammaTest, gammaIndex3DClassicForIncorrectParametersShouldThrow){
    EXPECT_THROW(yagit::gammaIndex3DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS1), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex3DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS2), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex3DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS3), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex3DClassic(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS4), std::invalid_argument);
}

TEST(GammaTest, gammaIndex2DWendlingForIncorrectParametersShouldThrow){
    EXPECT_THROW(yagit::gammaIndex2DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS1), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS2), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS3), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS4), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS5), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS6), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS7), std::invalid_argument);
}

TEST(GammaTest, gammaIndex2_5DWendlingForIncorrectParametersShouldThrow){
    EXPECT_THROW(yagit::gammaIndex2_5DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS1), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2_5DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS2), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2_5DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS3), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2_5DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS4), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2_5DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS5), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2_5DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS6), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex2_5DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS7), std::invalid_argument);
}

TEST(GammaTest, gammaIndex3DWendlingForIncorrectParametersShouldThrow){
    EXPECT_THROW(yagit::gammaIndex3DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS1), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex3DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS2), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex3DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS3), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex3DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS4), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex3DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS5), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex3DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS6), std::invalid_argument);
    EXPECT_THROW(yagit::gammaIndex3DWendling(REF_2D, EVAL_2D, INCORRECT_GAMMA_PARAMS7), std::invalid_argument);
}
