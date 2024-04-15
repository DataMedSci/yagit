/********************************************************************************************
 * Copyright (C) 2023-2024 'Yet Another Gamma Index Tool' Developers.
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
#pragma once

#include <string>
#include <tuple>
#include <algorithm>
#include <stdexcept>
#include <limits>

#include "yagit/ImageData.hpp"
#include "yagit/GammaParameters.hpp"
#include "yagit/Gamma.hpp"

namespace yagit{

namespace{
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
    if(gammaParams.normalization != GammaNormalization::Global &&
       gammaParams.normalization != GammaNormalization::Local){
        throw std::invalid_argument("global normalization is neither global nor local");
    }
    if(gammaParams.normalization == GammaNormalization::Global && gammaParams.globalNormDose <= 0){
        throw std::invalid_argument("global normalization dose is not positive (globalNormDose <= 0)");
    }
}

void validateWendlingGammaParameters(const GammaParameters& gammaParams){
    if(gammaParams.maxSearchDistance <= 0){
        throw std::invalid_argument("maximum search distance is not positive (maxSearchDistance <= 0)");
    }
    if(gammaParams.stepSize <= 0){
        throw std::invalid_argument("step size is not positive (stepSize <= 0)");
    }
    if(gammaParams.stepSize > gammaParams.maxSearchDistance){
        throw std::invalid_argument("step size is greater than maximum search distance (stepSize > maxSearchDistance)");
    }
}
}

namespace{
template <typename T, typename A = std::allocator<T>>
std::vector<T, A> generateVector(T start, T step, size_t size){
    std::vector<T, A> result;
    result.reserve(size);

    T val = start;
    for(size_t i = 0; i < size; i++){
        result.push_back(val);
        val += step;
    }

    return result;
}

std::vector<float> generateCoordinates(const ImageData& image, ImageAxis axis){
    if(axis == ImageAxis::Z){
        return generateVector(image.getOffset().frames, image.getSpacing().frames, image.getSize().frames);
    }
    else if(axis == ImageAxis::Y){
        return generateVector(image.getOffset().rows, image.getSpacing().rows, image.getSize().rows);
    }
    else if(axis == ImageAxis::X){
        return generateVector(image.getOffset().columns, image.getSpacing().columns, image.getSize().columns);
    }
    return {};
}
}

namespace{
const float NaN{std::numeric_limits<float>::quiet_NaN()};
const float Inf{std::numeric_limits<float>::infinity()};

// absolute tolerance that is useful for floating-point computations
// currently it is absolute tolerance, but it can be changed to relative tolerance if it turns out to work better
const float Tolerance{1e-6};
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
}

namespace{
struct Point2D{
    float y;
    float x;
    float distSq;

    Point2D(float y, float x, float distSq)
        : y(y), x(x), distSq(distSq) {}
    
    bool operator==(const Point2D& other) const{
        return y == other.y && x == other.x && distSq == other.distSq;
    }
};

struct Point3D{
    float z;
    float y;
    float x;
    float distSq;

    Point3D(float z, float y, float x, float distSq)
        : z(z), y(y), x(x), distSq(distSq) {}
    
    bool operator==(const Point3D& other) const{
        return z == other.z && y == other.y && x == other.x && distSq == other.distSq;
    }
};

void sortByDistanceAsc(std::vector<Point2D>& points){
    std::sort(points.begin(), points.end(), [](const auto& lhs, const auto& rhs){
        return lhs.distSq < rhs.distSq;
    });
}

void sortByDistanceAsc(std::vector<Point3D>& points){
    std::sort(points.begin(), points.end(), [](const auto& lhs, const auto& rhs){
        return lhs.distSq < rhs.distSq;
    });
}

void addPointAndVariants(float y, float x, float distSq, std::vector<Point2D>& result){
    result.emplace_back(y, x, distSq);
    if(y != 0 && x != 0){
        result.emplace_back(-y, -x, distSq);
    }
    if(y != 0){
        result.emplace_back(-y, x, distSq);
    }
    if(x != 0){
        result.emplace_back(y, -x, distSq);
    }
}

std::vector<Point2D> sortedPointsInCircle(float radius, float stepSize){
    std::vector<Point2D> result;
    const uint32_t elements = static_cast<uint32_t>(radius / stepSize);
    // reserve a little more than pi * elements^2
    result.reserve(static_cast<size_t>(3.5 * elements * elements));

    float rSq = radius * radius + Tolerance;
    radius += Tolerance;

    for(float y = 0; y <= radius; y += stepSize){
        float y2 = y*y;
        for(float x = 0; x <= y + Tolerance; x += stepSize){
            float distSq = y2 + x*x;
            if(distSq <= rSq){
                addPointAndVariants(y, x, distSq, result);

                if(y != x){
                    addPointAndVariants(x, y, distSq, result);
                }
            }
        }
    }

    sortByDistanceAsc(result);
    return result;
}

void addPointAndVariants(float z, float y, float x, float distSq, std::vector<Point3D>& result){
    result.emplace_back(z, y, x, distSq);
    if(z != 0 && y != 0 && x != 0){
        result.emplace_back(-z, -y, -x, distSq);
    }
    if(z != 0 && y != 0){
        result.emplace_back(-z, -y, x, distSq);
    }
    if(z != 0 && x != 0){
        result.emplace_back(-z, y, -x, distSq);
    }
    if(y != 0 && x != 0){
        result.emplace_back(z, -y, -x, distSq);
    }
    if(z != 0){
        result.emplace_back(-z, y, x, distSq);
    }
    if(y != 0){
        result.emplace_back(z, -y, x, distSq);
    }
    if(x != 0){
        result.emplace_back(z, y, -x, distSq);
    }
}

std::vector<Point3D> sortedPointsInSphere(float radius, float stepSize){
    std::vector<Point3D> result;
    const uint32_t elements = static_cast<uint32_t>(radius / stepSize);
    // reserve a little more than 4/3 * pi * elements^3
    result.reserve(static_cast<size_t>(4.5 * elements * elements * elements));

    float rSq = radius * radius + Tolerance;
    radius += Tolerance;

    for(float z = 0; z <= radius; z += stepSize){
        float z2 = z*z;
        for(float y = 0; y <= z + Tolerance; y += stepSize){
            float z2y2 = z2 + y*y;
            for(float x = 0; x <= y + Tolerance; x += stepSize){
                float distSq = z2y2 + x*x;
                if(distSq <= rSq){
                    addPointAndVariants(z, y, x, distSq, result);

                    if(z != y && y != x && z != x){
                        addPointAndVariants(y, x, z, distSq, result);
                        addPointAndVariants(x, z, y, distSq, result);
                    }
                    if(y != x){
                        addPointAndVariants(z, x, y, distSq, result);
                    }
                    if(z != y){
                        addPointAndVariants(y, z, x, distSq, result);
                    }
                    if(z != x){
                        addPointAndVariants(x, y, z, distSq, result);
                    }
                }
            }
        }
    }

    sortByDistanceAsc(result);
    return result;
}
}

}
