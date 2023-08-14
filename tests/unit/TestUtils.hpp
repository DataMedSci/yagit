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
#include "yagit/GammaParameters.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Matcher;

Matcher<yagit::DataSize> matchDataSize(const yagit::DataSize& expectedDataSize);
Matcher<yagit::DataOffset> matchDataOffset(const yagit::DataOffset& expectedDataOffset);
Matcher<yagit::DataSpacing> matchDataSpacing(const yagit::DataSpacing& expectedDataSpacing);
Matcher<yagit::ImageData> matchImageData(const yagit::ImageData& expectedImageData, float maxAbsError = -1);
Matcher<yagit::ImageData> matchImageData(const std::vector<float>& expectedData,
                                         const yagit::DataSize& expectedDataSize,
                                         const yagit::DataOffset& expectedDataOffset,
                                         const yagit::DataSpacing& expectedDataSpacing,
                                         float maxAbsError = -1);

yagit::ImageData generateImageData(float value, const yagit::DataSize& size,
                                   const yagit::DataOffset& offset, const yagit::DataSpacing& spacing);

namespace yagit{
std::ostream& operator<<(std::ostream& os, const yagit::GammaParameters& gammaParams);
}
