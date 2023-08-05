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
const float MAX_ABS_ERROR = 2e-6;
const float MAX_ABS_ERROR2 = 1e-5;

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
     {0.5, 0.43, 0.6}
    },
    {{0.4, 0.7, 0.28},
     {1.4, 0.8, 0.9}
    }
};
const yagit::Image3D EVAL_IMAGE_3D = {
    {{0.24, 0.68, 0.2},
     {0.67, 0.9, 0.6}
    },
    {{1.0, 0.8, 0.34},
     {0.8, 0.99, 0.83}
    }
};

const yagit::ImageData REF_2D(REF_IMAGE_2D, {0, 0, -1}, {1, 1, 1});
const yagit::ImageData EVAL_2D(EVAL_IMAGE_2D, {0, -1, 0}, {1, 1, 1});
const yagit::ImageData REF_3D(REF_IMAGE_3D, {-0.2, -5.8, 4.4}, {1.5, 2, 2.5});
const yagit::ImageData EVAL_3D(EVAL_IMAGE_3D, {-0.3, -6.0, 4.5}, {1.5, 2, 2.5});

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

namespace{
yagit::ImageData generateImageData(float value, const yagit::DataSize& size,
                                   const yagit::DataOffset& offset, const yagit::DataSpacing& spacing){
    size_t count = size.frames * size.rows * size.columns;
    return yagit::ImageData(std::vector<float>(count, value), size, offset, spacing);
}
}

namespace yagit{
std::ostream& operator<<(std::ostream& os, const yagit::GammaParameters& gammaParams){
    std::string norm = (gammaParams.normalization == yagit::GammaNormalization::Global ? "global" : (
        gammaParams.normalization == yagit::GammaNormalization::Local ? "local" : "?"
    ));
    return os << "{dd=" << gammaParams.ddThreshold << ", dta=" << gammaParams.dtaThreshold
              << ", norm=" << norm << ", global norm dose=" << gammaParams.globalNormDose
              << ", dco=" << gammaParams.doseCutoff
              << ", max search dist=" << gammaParams.maxSearchDistance
              << ", step size=" << gammaParams.stepSize << "}";
}
}


const GammaParametric2D gammaIndex2DClassicTestValues[] = {
    // GAMMA PARAMETERS                                             EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_2D_MAX, 0},      {{0.816496, 0.333333},
                                                                     {0.942809, 0.333333}}},

    {{2, 2, yagit::GammaNormalization::Global, REF_2D_MAX, 0},      {{1.224744, 0.5},
                                                                     {1.4142135, 0.5}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0},                {{0.857957, 0.333333},
                                                                     {0.942809, 0.333333}}},

    {{2, 2, yagit::GammaNormalization::Local, 0, 0},                {{1.286935, 0.5},
                                                                     {1.4142135, 0.5}}},

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
        {0.666666, 1},
        {0.942809, 1.414213}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
}


const GammaParametric3D gammaIndex2_5DClassicTestValues[] = {
    // GAMMA PARAMETERS                                            EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0},     {{{0.9558745, 0.9558745, 2.1712093},
                                                                     {2.9265150, 4.1400346, 0.0816497}},
                                                                    {{2.2217947, 2.3823520, 1.4309029},
                                                                     {9.5521175, 0.7348469, 1.6686655}}}},

    {{2, 2, yagit::GammaNormalization::Global, REF_3D_MAX, 0},     {{{1.4338118, 1.4338118, 3.2568139},
                                                                     {4.3897724, 6.2100519, 0.1224745}},
                                                                    {{3.3326921, 3.5735280, 2.1463543},
                                                                     {14.3281762, 1.1022704, 2.5029982}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0},               {{{1.7016332, 1.8554022, 6.8642391},
                                                                     {6.8804070, 13.2069722, 0.0816497}},
                                                                    {{5.2816243, 4.7626047, 7.1433238},
                                                                     {9.5521175, 0.7348469, 2.5938780}}}},

    {{2, 2, yagit::GammaNormalization::Local, 0, 0},               {{{2.5524498, 2.7831053, 10.2963586},
                                                                     {10.3206129, 19.8104583, 0.1224745}},
                                                                    {{7.9224365, 7.1439071, 10.7149857},
                                                                     {14.3281762, 1.1022704, 3.8908170}}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0.4},   {{{NaN,       0.9558745, NaN},
                                                                     {2.9265150, 4.1400346, 0.0816497}},
                                                                    {{2.2217947, 2.3823520, NaN},
                                                                     {9.5521175, 0.7348469, 1.6686655}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.4},             {{{NaN,       1.8554022, NaN},
                                                                     {6.8804070, 13.2069722, 0.0816497}},
                                                                    {{5.2816243, 4.7626047, NaN},
                                                                     {9.5521175, 0.7348469, 2.5938780}}}},
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
        {{0.952381, 1.380952, 2.457807},
         {4.169319, 4.680737, 2.018059}},
        {{4.363578, 2.769284, 2.653454},
         {9.599343, 1.563472, 2.567457}}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
}


const GammaParametric3D gammaIndex3DClassicTestValues[] = {
    // GAMMA PARAMETERS                                            EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0},     {{{0.9558745, 0.9558745, 1.0631852},
                                                                     {2.9265150, 2.4695148, 0.0816497}},
                                                                    {{2.2217947, 0.7188584, 1.4309029},
                                                                     {9.5521175, 0.7348469, 0.9637888}}}},

    {{2, 2, yagit::GammaNormalization::Global, REF_3D_MAX, 0},     {{{1.4338118, 1.4338118, 1.5947778},
                                                                     {4.3897724, 3.7042722, 0.1224745}},
                                                                    {{3.3326921, 1.0782876, 2.1463543},
                                                                     {14.3281762, 1.1022704, 1.4456832}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0},               {{{1.7016332, 1.8554022, 4.4694988},
                                                                     {6.8804070, 7.0838991, 0.0816497}},
                                                                    {{5.2816243, 1.0940884, 5.0628454},
                                                                     {9.5521175, 0.7348469, 0.9637888}}}},

    {{2, 2, yagit::GammaNormalization::Local, 0, 0},               {{{2.5524498, 2.78310537, 6.7042482},
                                                                     {10.3206129, 10.6258486, 0.1224745}},
                                                                    {{7.9224365, 1.6411326, 7.5942681},
                                                                     {14.3281762, 1.1022704, 1.4456832}}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0.4},   {{{NaN,       0.9558745, NaN},
                                                                     {2.9265150, 2.4695148, 0.0816497}},
                                                                    {{2.2217947, 0.7188584, NaN},
                                                                     {9.5521175, 0.7348469, 0.9637888}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.4},             {{{NaN,       1.8554022, NaN},
                                                                     {6.8804070, 7.0838991, 0.0816497}},
                                                                    {{5.2816243, 1.0940884, NaN},
                                                                     {9.5521175, 0.7348469, 0.9637888}}}},
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
        {{0.952381, 1.380952, 2.457807},
         {4.169319, 3.976689, 2.018059}},
        {{3.824079, 1.156663, 2.240121},
         {9.599343, 1.563472, 1.054093}}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
}


const GammaParametric2D gammaIndex2DWendlingTestValues[] = {
    // GAMMA PARAMETERS                                                     EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_2D_MAX, 0, 5, 0.3},      {{1.0942476, 0.3431877},
                                                                             {0.7218803, 0.4157456}}},
    {{2, 1, yagit::GammaNormalization::Global, REF_2D_MAX, 0, 5, 0.1},      {{1.7320504, 0.9340771},
                                                                             {1.7916474, 1.0}}},
    {{3, 3, yagit::GammaNormalization::Local, 0, 0, 5, 0.3},                {{1.1598970, 0.3475323},
                                                                             {0.7266982, 0.4157456}}},
    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 5, 0.1},                {{1.7765701, 0.9376845},
                                                                             {1.7960243, 1}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_2D_MAX, 0.95, 5, 0.3},   {{NaN, 0.3431877},
                                                                             {0.7218803, 0.4157456}}},
    {{3, 3, yagit::GammaNormalization::Local, 0, 0.95, 5, 0.3},             {{NaN, 0.3475323},
                                                                             {0.7266982, 0.4157456}}},

    // small max search distance
    {{2, 1, yagit::GammaNormalization::Global, REF_2D_MAX, 0, 1.2, 0.1},    {{2.3151658, 0.9340771},
                                                                             {NaN, 1}}},
    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 1.2, 0.1},              {{2.4463859, 0.9376844},
                                                                             {NaN, 1}}}
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
        {0.000002, 0.389267}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
}


const GammaParametric3D gammaIndex2_5DWendlingTestValues[] = {
    // GAMMA PARAMETERS                                                      EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 10, 0.3},      {{{2.0744779, 0.2353202, 0.5686724},
                                                                               {0.4720460, 0.8494650, 0.1951010}},
                                                                              {{NaN, NaN, NaN},
                                                                               {NaN, NaN, NaN}}}},

    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 10, 0.1},      {{{3.2458065, 0.2830736, 0.2648694},
                                                                               {1.1140437, 2.5462873, 0.3625809}},
                                                                              {{NaN, NaN, NaN},
                                                                               {NaN, NaN, NaN}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0, 10, 0.3},                {{{8.4940739, 0.2772369, 2.6143649},
                                                                               {0.6694975, 0.8584009, 0.4034760}},
                                                                              {{NaN, NaN, NaN},
                                                                               {NaN, NaN, NaN}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 10, 0.1},                {{{5.6120272, 0.2839459, 0.8472390},
                                                                               {1.1770180, 2.5530200, 0.4144889}},
                                                                              {{NaN, NaN, NaN},
                                                                               {NaN, NaN, NaN}}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0.4, 10, 0.3},    {{{NaN,       0.2353202, NaN},
                                                                               {0.4720460, 0.8494650, 0.1951010}},
                                                                              {{NaN, NaN, NaN},
                                                                               {NaN, NaN, NaN}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.4, 10, 0.3},              {{{NaN,       0.2772369, NaN},
                                                                               {0.6694975, 0.8584009, 0.4034760}},
                                                                              {{NaN, NaN, NaN},
                                                                               {NaN, NaN, NaN}}}},

    // small max search distance
    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 1.2, 0.1},     {{{3.2458065, 0.2830736, 0.2648694},
                                                                               {1.1140437, 10.9944038, 0.3625809}},
                                                                              {{NaN, NaN, NaN},
                                                                               {NaN, NaN, NaN}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 1.2, 0.1},               {{{22.6677685, 0.2839459, 0.8472390},
                                                                               {1.1770180, 35.6128922, 0.4144889}},
                                                                              {{NaN, NaN, NaN},
                                                                               {NaN, NaN, NaN}}}}
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

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
}

TEST(GammaTest, gammaIndex2_5DWendlingForEvalImageShiftedOneFrameUpFromRefImg){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {2, 2, 2});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {-2, 0, 0}, {2, 2, 2});

    const auto res = yagit::gammaIndex2_5DWendling(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{4.0928965, 0.3189612, 0.9523808},
         {1.4590440, 0.8498633, 0.3757150}},
        {{NaN, NaN, NaN},
         {NaN, NaN, NaN}}
    };
    yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());
    EXPECT_THAT(res , matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
}

TEST(GammaTest, gammaIndex2_5DWendlingForEvalImageShiftedOneFrameDownFromRefImg){
    const yagit::ImageData refImg(REF_IMAGE_3D, {0, 0, 0}, {2, 2, 2});
    const yagit::ImageData evalImg(EVAL_IMAGE_3D, {2, 0, 0}, {2, 2, 2});

    const auto res = yagit::gammaIndex2_5DWendling(refImg, evalImg, GAMMA_PARAMS_3D);

    const yagit::Image3D expected = {
        {{NaN, NaN, NaN},
         {NaN, NaN, NaN}},
        {{0.648127, 0.325277, 0.215131},
         {12.281868, 0.300943, 0.751645}}
    };
    yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());
    EXPECT_THAT(res , matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
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
    EXPECT_THAT(res , matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
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
    EXPECT_THAT(res , matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
}


const GammaParametric3D gammaIndex3DWendlingTestValues[] = {
    // GAMMA PARAMETERS                                                      EXPECTED GAMMA
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 10, 0.3},      {{{2.0744772, 0.2314351, 0.1121029},
                                                                               {0.4720462, 0.8494644, 0.1951000}},
                                                                              {{0.5543976, 0.3427831, 0.5720407},
                                                                               {10.5018463, 0.4854333, 0.5203138}}}},

    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 10, 0.1},      {{{1.4494189, 0.2457071, 0.1527447},
                                                                               {1.0124733, 2.4921083, 0.2449490}},
                                                                              {{1.3327116, 0.6633933, 0.7486043},
                                                                               {14.4548120, 1.3661851, 0.9413394}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0, 10, 0.3},                {{{8.4940739, 0.2589318, 0.2567191},
                                                                               {0.6694975, 0.8584012, 0.4034760}},
                                                                              {{0.7089564, 0.4375782, 1.4767407},
                                                                               {10.5018463, 0.5176043, 0.5213040}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 10, 0.1},                {{{5.1048970, 0.2485556, 0.3026561},
                                                                               {1.0295638, 2.5415282, 0.2449490}},
                                                                              {{1.4434612, 0.6635982, 1.0247957},
                                                                               {14.4548120, 1.3780971, 0.9854494}}}},

    // dco > 0
    {{3, 3, yagit::GammaNormalization::Global, REF_3D_MAX, 0.4, 10, 0.3},    {{{NaN,       0.2314351, NaN},
                                                                               {0.4720462, 0.8494644, 0.1951000}},
                                                                              {{0.5543976, 0.3427831, NaN},
                                                                               {10.5018463, 0.4854333, 0.5203138}}}},

    {{3, 3, yagit::GammaNormalization::Local, 0, 0.4, 10, 0.3},              {{{NaN,       0.2589318, NaN},
                                                                               {0.6694975, 0.8584012, 0.4034760}},
                                                                              {{0.7089564, 0.4375782, NaN},
                                                                               {10.5018463, 0.5176043, 0.5213040}}}},

    // small max search distance
    {{2, 1, yagit::GammaNormalization::Global, REF_3D_MAX, 0, 1.2, 0.1},     {{{1.4494189, 0.2457071, 0.1527447},
                                                                               {1.0124733, 10.5678749, 0.2449490}},
                                                                              {{3.5172551, 0.6633933, 0.7486043},
                                                                               {18.1901073, 1.7949475, 0.9413394}}}},

    {{2, 1, yagit::GammaNormalization::Local, 0, 0, 1.2, 0.1},               {{{10.0029974, 0.2485556, 0.3026561},
                                                                               {1.0295638, 34.2153702, 0.2449490}},
                                                                              {{11.7205276, 0.6635981, 1.0247957},
                                                                               {18.1901073, 2.6261902, 0.9854494}}}}
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
        {{0.233715, 0.383647, 1.692145},
         {9.784037, 0.528611, 0.689143}}
    };
    const yagit::GammaResult expectedGammaRes(expected, refImg.getOffset(), refImg.getSpacing());

    EXPECT_THAT(gammaRes, matchImageData(expectedGammaRes, MAX_ABS_ERROR2));
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
        {1.0942476, 0.3431877},
        {0.7218803, 0.4157456}
    };
    const yagit::ImageData expected(expectedImage, REF_2D.getOffset(), REF_2D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex2D(REF_2D, EVAL_2D, GAMMA_PARAMS_2D, yagit::GammaMethod::Wendling);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DForClassicMethod){
    const yagit::Image3D expectedImage = {
        {{0.9558745, 0.9558745, 2.1712093},
         {2.9265150, 4.1400346, 0.0816497}},
        {{2.2217947, 2.3823520, 1.4309029},
         {9.5521175, 0.7348469, 1.6686655}}
    };
    const yagit::ImageData expected(expectedImage, REF_3D.getOffset(), REF_3D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex2_5D(REF_3D, EVAL_3D, GAMMA_PARAMS_3D, yagit::GammaMethod::Classic);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DForWendlingMethod){
    const yagit::Image3D expectedImage = {
        {{2.0744779, 0.2353202, 0.5686724},
          {0.4720460, 0.8494650, 0.1951010}},
         {{NaN, NaN, NaN},
          {NaN, NaN, NaN}}
    };
    const yagit::ImageData expected(expectedImage, REF_3D.getOffset(), REF_3D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex2_5D(REF_3D, EVAL_3D, GAMMA_PARAMS_3D, yagit::GammaMethod::Wendling);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex3DForClassicMethod){
    const yagit::Image3D expectedImage = {
        {{0.9558745, 0.9558745, 1.0631852},
         {2.9265150, 2.4695148, 0.0816497}},
        {{2.2217947, 0.7188584, 1.4309029},
         {9.5521175, 0.7348469, 0.9637888}}
    };
    const yagit::ImageData expected(expectedImage, REF_3D.getOffset(), REF_3D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex3D(REF_3D, EVAL_3D, GAMMA_PARAMS_3D, yagit::GammaMethod::Classic);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex3DForWendlingMethod){
    const yagit::Image3D expectedImage = {
        {{2.0744772, 0.2314351, 0.1121029},
         {0.4720462, 0.8494644, 0.1951000}},
        {{0.5543976, 0.3427831, 0.5720407},
         {10.5018463, 0.4854333, 0.5203138}}
    };
    const yagit::ImageData expected(expectedImage, REF_3D.getOffset(), REF_3D.getSpacing());

    const yagit::GammaResult gammaRes = yagit::gammaIndex3D(REF_3D, EVAL_3D, GAMMA_PARAMS_3D, yagit::GammaMethod::Wendling);
    EXPECT_THAT(gammaRes, matchImageData(expected, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2DForTheSameImagesShouldReturnImageFilledWithZeros){
    const yagit::ImageData zeroImage = generateImageData(0, REF_2D.getSize(), REF_2D.getOffset(), REF_2D.getSpacing());
    EXPECT_THAT(yagit::gammaIndex2DClassic(REF_2D, REF_2D, GAMMA_PARAMS_2D), matchImageData(zeroImage, MAX_ABS_ERROR));
    EXPECT_THAT(yagit::gammaIndex2DWendling(REF_2D, REF_2D, GAMMA_PARAMS_2D), matchImageData(zeroImage, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex2_5DForTheSameImagesShouldReturnImageFilledWithZeros){
    const yagit::ImageData zeroImage = generateImageData(0, REF_3D.getSize(), REF_3D.getOffset(), REF_3D.getSpacing());
    EXPECT_THAT(yagit::gammaIndex2_5DClassic(REF_3D, REF_3D, GAMMA_PARAMS_3D), matchImageData(zeroImage, MAX_ABS_ERROR));
    EXPECT_THAT(yagit::gammaIndex2_5DWendling(REF_3D, REF_3D, GAMMA_PARAMS_3D), matchImageData(zeroImage, MAX_ABS_ERROR));
}

TEST(GammaTest, gammaIndex3DForTheSameImagesShouldReturnImageFilledWithZeros){
    const yagit::ImageData zeroImage = generateImageData(0, REF_3D.getSize(), REF_3D.getOffset(), REF_3D.getSpacing());
    EXPECT_THAT(yagit::gammaIndex3DClassic(REF_3D, REF_3D, GAMMA_PARAMS_3D), matchImageData(zeroImage, MAX_ABS_ERROR));
    EXPECT_THAT(yagit::gammaIndex3DWendling(REF_3D, REF_3D, GAMMA_PARAMS_3D), matchImageData(zeroImage, MAX_ABS_ERROR));
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
