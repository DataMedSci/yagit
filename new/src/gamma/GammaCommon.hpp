/********************************************************************************************
 * Copyright (C) 2023 'yet Another Gamma Index Tool' Developers.
 * 
 * This file is part of 'yet Another Gamma Index Tool'.
 * 
 * 'yet Another Gamma Index Tool' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'yet Another Gamma Index Tool'.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************************************/
#pragma once

#include <string>
#include <tuple>
#include <algorithm>
#include <stdexcept>
#include <limits>

#include "ImageData.hpp"
#include "GammaParameters.hpp"

namespace yagit{

namespace{
const float Nan{std::numeric_limits<float>::quiet_NaN()};
const float Inf{std::numeric_limits<float>::infinity()};
}

namespace{
// calculate squared 1D Euclidean distance
constexpr float distSq1D(float x1, float x2){
    return (x2 - x1) * (x2 - x1);
}

// calculate squared 2D Euclidean distance
constexpr float distSq2D(float x1, float y1, float x2, float y2){
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

// calculate squared 3D Euclidean distance
constexpr float distSq3D(float x1, float y1, float z1, float x2, float y2, float z2){
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1);
}

void validateImages2D(const ImageData& refImg, const ImageData& evalImg){
    if(refImg.getSize().frames > 1){
        throw std::invalid_argument("reference image is not 2D (frames=" + std::to_string(refImg.getSize().frames) + " > 1)");
    }
    if(evalImg.getSize().frames > 1){
        throw std::invalid_argument("evaluated image is not 2D (frames=" + std::to_string(evalImg.getSize().frames) + " > 1)");
    }
}

void validateGammaParameters(const GammaParameters& gammaParams){
    if(gammaParams.ddThreshold <= 0){
        throw std::invalid_argument("DD threshold is not positive (ddThreshold <= 0)");
    }
    if(gammaParams.dtaThreshold <= 0){
        throw std::invalid_argument("DTA threshold is not positive (dtaThreshold <= 0)");
    }
    if(gammaParams.normalization == GammaNormalization::Global && gammaParams.globalNormDose <= 0){
        throw std::invalid_argument("global normalization dose is not positive (globalNormDose <= 0)");
    }
}

void validateWendlingGammaParameters(const GammaParameters& gammaParams){
    if(gammaParams.maxSearchDistance <= 0){
        throw std::invalid_argument("Maximum search distance is not positive (maxSearchDistance <= 0)");
    }
    if(gammaParams.stepSize <= 0){
        throw std::invalid_argument("Step size is not positive (stepSize <= 0)");
    }
    if(gammaParams.stepSize > gammaParams.maxSearchDistance){
        throw std::invalid_argument("Step size is greater than maximum search distance (stepSize > maxSearchDistance)");
    }
}
}

namespace{
using YXPos = std::pair<float, float>;
using YXPosWithDistSq = std::pair<YXPos, float>;

using ZYXPos = std::tuple<float, float, float>;
using ZYXPosWithDistSq = std::pair<ZYXPos, float>;

std::vector<YXPosWithDistSq> sortedPointsInCircle(float radius, float stepSize){
    std::vector<YXPosWithDistSq> result;
    const uint32_t elements = static_cast<uint32_t>(radius / stepSize);
    // reserve a little more than pi * e^2
    result.reserve(static_cast<size_t>(3.5 * elements * elements));

    float rSq = radius * radius;
    for(float y = 0; y <= radius; y += stepSize){
        for(float x = 0; x <= radius; x += stepSize){
            float distSq = y*y + x*x;
            if(distSq <= rSq){
                result.emplace_back(YXPos{y, x}, distSq);
                if(y != 0 && x != 0){
                    result.emplace_back(YXPos{-y, -x}, distSq);
                }
                if(y != 0){
                    result.emplace_back(YXPos{-y, x}, distSq);
                }
                if(x != 0){
                    result.emplace_back(YXPos{y, -x}, distSq);
                }
            }
        }
    }

    // sort by squared distance ascending
    std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs){
        return lhs.second < rhs.second;
    });
    return result;
}

std::vector<ZYXPosWithDistSq> sortedPointsInSphere(float radius, float stepSize){
    std::vector<ZYXPosWithDistSq> result;
    const uint32_t elements = static_cast<uint32_t>(radius / stepSize);
    // reserve a little more than 4/3 * pi * e^3
    result.reserve(static_cast<size_t>(4.5 * elements * elements * elements));

    float rSq = radius * radius;
    for(float z = 0; z <= radius; z += stepSize){
        for(float y = 0; y <= radius; y += stepSize){
            for(float x = 0; x <= radius; x += stepSize){
                float distSq = z*z + y*y + x*x;
                if(distSq <= rSq){
                    result.emplace_back(ZYXPos{z, y, x}, distSq);
                    if(z != 0 && y != 0 && x != 0){
                        result.emplace_back(ZYXPos{-z, -y, -x}, distSq);
                    }
                    if(z != 0 && y != 0){
                        result.emplace_back(ZYXPos{-z, -y, x}, distSq);
                    }
                    if(z != 0 && x != 0){
                        result.emplace_back(ZYXPos{-z, y, -x}, distSq);
                    }
                    if(y != 0 && x != 0){
                        result.emplace_back(ZYXPos{z, -y, -x}, distSq);
                    }
                    if(z != 0){
                        result.emplace_back(ZYXPos{-z, y, x}, distSq);
                    }
                    if(y != 0){
                        result.emplace_back(ZYXPos{z, -y, x}, distSq);
                    }
                    if(x != 0){
                        result.emplace_back(ZYXPos{z, y, -x}, distSq);
                    }
                }
            }
        }
    }

    // sort by squared distance ascending
    std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs){
        return lhs.second < rhs.second;
    });
    return result;
}
}

}
