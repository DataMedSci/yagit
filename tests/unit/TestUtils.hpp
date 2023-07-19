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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using testing::Matcher, testing::AllOf, testing::Field, testing::Property, testing::Pointwise,
      testing::NanSensitiveFloatEq, testing::NanSensitiveFloatNear;

Matcher<yagit::DataSize> matchDataSize(const yagit::DataSize& expectedDataSize){
    return AllOf(Field("frames", &yagit::DataSize::frames, expectedDataSize.frames),
                 Field("rows", &yagit::DataSize::rows, expectedDataSize.rows),
                 Field("columns", &yagit::DataSize::columns, expectedDataSize.columns));
}

Matcher<yagit::DataOffset> matchDataOffset(const yagit::DataOffset& expectedDataOffset){
    return AllOf(Field("frames", &yagit::DataOffset::frames, expectedDataOffset.frames),
                 Field("rows", &yagit::DataOffset::rows, expectedDataOffset.rows),
                 Field("columns", &yagit::DataOffset::columns, expectedDataOffset.columns));
}

Matcher<yagit::DataSpacing> matchDataSpacing(const yagit::DataSpacing& expectedDataSpacing){
    return AllOf(Field("frames", &yagit::DataSpacing::frames, expectedDataSpacing.frames),
                 Field("rows", &yagit::DataSpacing::rows, expectedDataSpacing.rows),
                 Field("columns", &yagit::DataSpacing::columns, expectedDataSpacing.columns));
}

Matcher<yagit::ImageData> matchImageData(const yagit::ImageData& expectedImageData, float maxAbsError = -1){
    return AllOf(Property("data", &yagit::ImageData::getData,
                          Pointwise((maxAbsError <= 0 ? NanSensitiveFloatEq() : NanSensitiveFloatNear(maxAbsError)),
                                    expectedImageData.getData())),
                 Property("size", &yagit::ImageData::getSize, matchDataSize(expectedImageData.getSize())),
                 Property("offset", &yagit::ImageData::getOffset, matchDataOffset(expectedImageData.getOffset())),
                 Property("spacing", &yagit::ImageData::getSpacing, matchDataSpacing(expectedImageData.getSpacing())));
}

Matcher<yagit::ImageData> matchImageData(const std::vector<float>& expectedData,
                                         const yagit::DataSize& expectedDataSize,
                                         const yagit::DataOffset& expectedDataOffset,
                                         const yagit::DataSpacing& expectedDataSpacing,
                                         float maxAbsError = -1){
    return AllOf(Property("data", &yagit::ImageData::getData,
                          Pointwise((maxAbsError <= 0 ? NanSensitiveFloatEq() : NanSensitiveFloatNear(maxAbsError)),
                                    expectedData)),
                 Property("size", &yagit::ImageData::getSize, matchDataSize(expectedDataSize)),
                 Property("offset", &yagit::ImageData::getOffset, matchDataOffset(expectedDataOffset)),
                 Property("spacing", &yagit::ImageData::getSpacing, matchDataSpacing(expectedDataSpacing)));
}
