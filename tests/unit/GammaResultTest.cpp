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

#include "yagit/GammaResult.hpp"

#include <gtest/gtest.h>

namespace{
const float NaN = std::numeric_limits<float>::quiet_NaN();

const std::vector<float>& DATA{0.5, 0.2, 0.0, 1.0, 0.2, 1.1, 3.0, 0.1};
const std::vector<float>& DATA_WITH_NANS{NaN, 0.5, 0.2, 0.0, 1.0, 0.2, NaN, 1.1, 3.0, 0.1, NaN, NaN};

const yagit::GammaResult GAMMA_RESULT(DATA, {2, 2, 2}, {0, 0, 0}, {1, 1, 1});
const yagit::GammaResult GAMMA_RESULT_WITH_NANS(DATA_WITH_NANS, {2, 2, 3}, {0, 0, 0}, {1, 1, 1});
}

TEST(GammaResultTest, passingRate){
    const double expectedPassingRate = 0.75;
    EXPECT_DOUBLE_EQ(expectedPassingRate, GAMMA_RESULT.passingRate());
    EXPECT_DOUBLE_EQ(expectedPassingRate, GAMMA_RESULT_WITH_NANS.passingRate());
}

TEST(GammaResultTest, minGamma){
    const float expectedMinGamma = 0.0;
    EXPECT_FLOAT_EQ(expectedMinGamma, GAMMA_RESULT.minGamma());
    EXPECT_FLOAT_EQ(expectedMinGamma, GAMMA_RESULT_WITH_NANS.minGamma());
}

TEST(GammaResultTest, maxGamma){
    const float expectedMaxGamma = 3.0;
    EXPECT_FLOAT_EQ(expectedMaxGamma, GAMMA_RESULT.maxGamma());
    EXPECT_FLOAT_EQ(expectedMaxGamma, GAMMA_RESULT_WITH_NANS.maxGamma());
}

TEST(GammaResultTest, sumGamma){
    const float expectedSumGamma = 6.1;
    EXPECT_FLOAT_EQ(expectedSumGamma, GAMMA_RESULT.sumGamma());
    EXPECT_FLOAT_EQ(expectedSumGamma, GAMMA_RESULT_WITH_NANS.sumGamma());
}

TEST(GammaResultTest, meanGamma){
    const float expectedMeanGamma = 0.7625;
    EXPECT_FLOAT_EQ(expectedMeanGamma, GAMMA_RESULT.meanGamma());
    EXPECT_FLOAT_EQ(expectedMeanGamma, GAMMA_RESULT_WITH_NANS.meanGamma());
}

TEST(GammaResultTest, varGamma){
    const float expectedVarGamma = 0.86234375;
    EXPECT_FLOAT_EQ(expectedVarGamma, GAMMA_RESULT.varGamma());
    EXPECT_FLOAT_EQ(expectedVarGamma, GAMMA_RESULT_WITH_NANS.varGamma());
}
