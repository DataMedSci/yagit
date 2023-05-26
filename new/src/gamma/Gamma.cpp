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

#include "Gamma.hpp"
#include "Interpolation.hpp"

#include <string>
#include <limits>
#include <cmath>
#include <algorithm>

#include <chrono>
#include <iostream>

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

void validateGammaParameters(const GammaParameters& gammaParams){
    if(gammaParams.ddThreshold <= 0){
        throw std::invalid_argument("DD threshold is not positive (ddThreshold <= 0)");
    }
    if(gammaParams.dtaThreshold <= 0){
        throw std::invalid_argument("DTA threshold is not positive (dtaThreshold <= 0)");
    }
    if(gammaParams.normalization == yagit::GammaNormalization::Global && gammaParams.globalNormDose <= 0){
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

GammaResult gammaIndex2D(const ImageData& refImg2D, const ImageData& evalImg2D, const GammaParameters& gammaParams){
    if(refImg2D.getSize().frames > 1){
        throw std::invalid_argument("reference image is not 2D (frames=" + std::to_string(refImg2D.getSize().frames) + " > 1)");
    }
    if(evalImg2D.getSize().frames > 1){
        throw std::invalid_argument("evaluated image is not 2D (frames=" + std::to_string(evalImg2D.getSize().frames) + " > 1)");
    }
    validateGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg2D.size());

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);

    const float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    // iterate over each row and column of reference image
    size_t indRef = 0;
    float yr = refImg2D.getOffset().rows;
    for(uint32_t jr = 0; jr < refImg2D.getSize().rows; jr++){
        float xr = refImg2D.getOffset().columns;
        for(uint32_t ir = 0; ir < refImg2D.getSize().columns; ir++){
            float doseRef = refImg2D.get(indRef);
            // assign a value of NaN to points that are below the dose cutoff
            // or where the gamma index calculation is not possible due to division by zero
            // (i.e. where reference dose used for local normalization is zero)
            if(doseRef < gammaParams.doseCutoff || (gammaParams.normalization == yagit::GammaNormalization::Local && doseRef == 0)){
                gammaVals.emplace_back(Nan);
            }
            else{
                float minGammaValSq = Inf;
                // set inversed normalized dd based on the type of normalization, whether it is global or local
                float ddNormInvSq = ddInvSq * (
                    gammaParams.normalization == yagit::GammaNormalization::Global ? globalNormDoseInvSq
                                                                                   : (1 / (doseRef * doseRef)));

                // iterate over each row and column of evaluated image
                size_t indEval = 0;
                float ye = evalImg2D.getOffset().rows;
                for(uint32_t je = 0; je < evalImg2D.getSize().rows; je++){
                    float xe = evalImg2D.getOffset().columns;
                    for(uint32_t ie = 0; ie < evalImg2D.getSize().columns; ie++){
                        float doseEval = evalImg2D.get(indEval);
                        // calculate squared gamma
                        float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + distSq2D(xe, ye, xr, yr) * dtaInvSq;
                        if(gammaValSq < minGammaValSq){
                            minGammaValSq = gammaValSq;
                        }

                        xe += evalImg2D.getSpacing().columns;
                        indEval++;
                    }
                    ye += evalImg2D.getSpacing().rows;
                }

                gammaVals.emplace_back(std::sqrt(minGammaValSq));
            }

            xr += refImg2D.getSpacing().columns;
            indRef++;
        }
        yr += refImg2D.getSpacing().rows;
    }

    return GammaResult(std::move(gammaVals), refImg2D.getSize(), refImg2D.getOffset(), refImg2D.getSpacing());
}

GammaResult gammaIndex2_5D(const ImageData& refImg3D, const ImageData& evalImg3D, const GammaParameters& gammaParams){
    if(refImg3D.getSize().frames != evalImg3D.getSize().frames){
        throw std::invalid_argument("reference image and evaluated image don't have the same number of frames");
    }
    validateGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg3D.size());

    float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);

    float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    float zr = refImg3D.getOffset().frames;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        float yr = refImg3D.getOffset().rows;
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            float xr = refImg3D.getOffset().columns;
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                float doseRef = refImg3D.get(indRef);
                // assign a value of NaN to points that are below the dose cutoff
                // or where the gamma index calculation is not possible due to division by zero
                // (i.e. where reference dose used for local normalization is zero)
                if(doseRef < gammaParams.doseCutoff || (gammaParams.normalization == yagit::GammaNormalization::Local && doseRef == 0)){
                    gammaVals.emplace_back(Nan);
                }
                else{
                    // set inversed normalized dd based on the type of normalization, whether it is global or local
                    float ddNormInvSq = ddInvSq * (
                        gammaParams.normalization == yagit::GammaNormalization::Global ? globalNormDoseInvSq
                                                                                       : (1 / (doseRef * doseRef)));
                    float minGammaVal = Inf;

                    // iterate over each row and column of evaluated image
                    size_t indEval = kr * evalImg3D.getSize().rows * refImg3D.getSize().columns;
                    float ze = evalImg3D.getOffset().frames + kr * evalImg3D.getSpacing().frames;
                    float ye = evalImg3D.getOffset().rows;
                    for(uint32_t je = 0; je < evalImg3D.getSize().rows; je++){
                        float xe = evalImg3D.getOffset().columns;
                        for(uint32_t ie = 0; ie < evalImg3D.getSize().columns; ie++){
                            float doseEval = evalImg3D.get(indEval);
                            // calculate squared gamma
                            float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + distSq3D(xe, ye, ze, xr, yr, zr) * dtaInvSq;
                            if(gammaValSq < minGammaVal){
                                minGammaVal = gammaValSq;
                            }

                            xe += evalImg3D.getSpacing().columns;
                            indEval++;
                        }
                        ye += evalImg3D.getSpacing().rows;
                    }

                    gammaVals.emplace_back(std::sqrt(minGammaVal));
                }

                xr += refImg3D.getSpacing().columns;
                indRef++;
            }
            yr += refImg3D.getSpacing().rows;
        }
        zr += refImg3D.getSpacing().frames;
    }

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

GammaResult gammaIndex3D(const ImageData& refImg3D, const ImageData& evalImg3D, const GammaParameters& gammaParams){
    validateGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg3D.size());

    float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);

    float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    float zr = refImg3D.getOffset().frames;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        float yr = refImg3D.getOffset().rows;
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            float xr = refImg3D.getOffset().columns;
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                float doseRef = refImg3D.get(indRef);
                // assign a value of NaN to points that are below the dose cutoff
                // or where the gamma index calculation is not possible due to division by zero
                // (i.e. where reference dose used for local normalization is zero)
                if(doseRef < gammaParams.doseCutoff || (gammaParams.normalization == yagit::GammaNormalization::Local && doseRef == 0)){
                    gammaVals.emplace_back(Nan);
                }
                else{
                    // set inversed normalized dd based on the type of normalization, whether it is global or local
                    float ddNormInvSq = ddInvSq * (
                        gammaParams.normalization == yagit::GammaNormalization::Global ? globalNormDoseInvSq
                                                                                       : (1 / (doseRef * doseRef)));
                    float minGammaVal = Inf;

                    // iterate over each frame, row and column of evaluated image
                    size_t indEval = 0;
                    float ze = evalImg3D.getOffset().frames;
                    for(uint32_t ke = 0; ke < evalImg3D.getSize().frames; ke++){
                        float ye = evalImg3D.getOffset().rows;
                        for(uint32_t je = 0; je < evalImg3D.getSize().rows; je++){
                            float xe = evalImg3D.getOffset().columns;
                            for(uint32_t ie = 0; ie < evalImg3D.getSize().columns; ie++){
                                float doseEval = evalImg3D.get(indEval);
                                // calculate squared gamma
                                float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + distSq3D(xe, ye, ze, xr, yr, zr) * dtaInvSq;
                                if(gammaValSq < minGammaVal){
                                    minGammaVal = gammaValSq;
                                }

                                xe += evalImg3D.getSpacing().columns;
                                indEval++;
                            }
                            ye += evalImg3D.getSpacing().rows;
                        }
                        ze += evalImg3D.getSpacing().frames;
                    }

                    gammaVals.emplace_back(std::sqrt(minGammaVal));
                }

                xr += refImg3D.getSpacing().columns;
                indRef++;
            }
            yr += refImg3D.getSpacing().rows;
        }
        zr += refImg3D.getSpacing().frames;
    }

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

GammaResult gammaIndex2DWendling(const ImageData& refImg2D, const ImageData& evalImg2D, const GammaParameters& gammaParams){
    if(refImg2D.getSize().frames > 1){
        throw std::invalid_argument("reference image is not 2D (frames=" + std::to_string(refImg2D.getSize().frames) + " > 1)");
    }
    if(evalImg2D.getSize().frames > 1){
        throw std::invalid_argument("evaluated image is not 2D (frames=" + std::to_string(evalImg2D.getSize().frames) + " > 1)");
    }
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg2D.size());

    float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);

    float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const ImageData evalImgOnRefGrid = Interpolation::bilinearOnPlane(evalImg2D, refImg2D, ImagePlane::Axial);

    using YXPos = std::pair<float, float>;
    using PosWithDistSq = std::pair<YXPos, float>;
    std::vector<PosWithDistSq> pointsInCircle;
    float r = gammaParams.maxSearchDistance;
    pointsInCircle.reserve(static_cast<int>(r / refImg2D.getSpacing().frames) *
                           static_cast<int>(r / refImg2D.getSpacing().rows));
    float rSq = r * r;
    for(float y = 0; y <= r; y += gammaParams.stepSize){
        for(float x = 0; x <= r; x += gammaParams.stepSize){
            float distSq = y*y + x*x;
            if(distSq <= rSq){
                pointsInCircle.push_back({{x,y}, distSq});
            }
        }
    }
    std::sort(pointsInCircle.begin(), pointsInCircle.end(), [](const auto& lhs, const auto& rhs){
        return lhs.second < rhs.second;
    });

    const float interpFactor = 1 / (refImg2D.getSpacing().rows * refImg2D.getSpacing().columns);

    // iterate over each row and column of reference image
    size_t indRef = 0;
    float yr = refImg2D.getOffset().rows;
    for(uint32_t jr = 0; jr < refImg2D.getSize().rows; jr++){
        float xr = refImg2D.getOffset().columns;
        for(uint32_t ir = 0; ir < refImg2D.getSize().columns; ir++){
            float doseRef = refImg2D.get(indRef);
            // assign a value of NaN to points that are below the dose cutoff
            // or where the gamma index calculation is not possible due to division by zero
            // (i.e. where reference dose used for local normalization is zero)
            if(doseRef < gammaParams.doseCutoff || (gammaParams.normalization == yagit::GammaNormalization::Local && doseRef == 0)){
                gammaVals.emplace_back(Nan);
            }
            else{
                float minGammaVal = Inf;
                // set inversed normalized dd based on the type of normalization, whether it is global or local
                float ddNormInvSq = ddInvSq * (
                    gammaParams.normalization == yagit::GammaNormalization::Global ? globalNormDoseInvSq
                                                                                   : (1 / (doseRef * doseRef)));
        
                bool atLeastOneInRange = false;
                for(const auto& point : pointsInCircle){
                    if(point.second * dtaInvSq > minGammaVal){
                        break;
                    }

                    auto [y, x] = point.first;
                    std::vector<YXPos> pointVariants;
                    pointVariants.reserve(4);
                    pointVariants.emplace_back(y, x);
                    if(y != 0 && x != 0){
                        pointVariants.emplace_back(-y, -x);
                    }
                    if(y != 0){
                        pointVariants.emplace_back(-y, x);
                    }
                    if(x != 0){
                        pointVariants.emplace_back(y, -x);
                    }
                    
                    for(const auto& point2 : pointVariants){
                        float ye = yr + point2.first;
                        float xe = xr + point2.second;

                        const int32_t indy1 = std::floor((ye - evalImgOnRefGrid.getOffset().rows) / evalImgOnRefGrid.getSpacing().rows);
                        const int32_t indx1 = std::floor((xe - evalImgOnRefGrid.getOffset().columns) / evalImgOnRefGrid.getSpacing().columns);
                        int32_t indy2 = indy1 + 1;
                        int32_t indx2 = indx1 + 1;

                        if(indy1 >= 0 && indy2 <= evalImgOnRefGrid.getSize().rows &&
                           indx1 >= 0 && indx2 <= evalImgOnRefGrid.getSize().columns){
                            atLeastOneInRange = true;

                            if(indy2 == evalImgOnRefGrid.getSize().rows){
                                indy2 = indy1;
                            }
                            if(indx2 == evalImgOnRefGrid.getSize().columns){
                                indx2 = indx1;
                            }

                            float y1 = evalImgOnRefGrid.getOffset().rows + indy1 * evalImgOnRefGrid.getSpacing().rows;
                            float x1 = evalImgOnRefGrid.getOffset().columns + indx1 * evalImgOnRefGrid.getSpacing().columns;
                            float y2 = y1 + (indy2 - indy1) * evalImgOnRefGrid.getSpacing().rows;
                            float x2 = x1 + (indx2 - indx1) * evalImgOnRefGrid.getSpacing().columns;
                            // float y2 = evalImgOnRefGrid.getOffset().rows + indy2 * evalImgOnRefGrid.getSpacing().rows;
                            // float x2 = evalImgOnRefGrid.getOffset().columns + indx2 * evalImgOnRefGrid.getSpacing().columns;

                            // bilinear interpolation on the fly
                            float doseEval = interpFactor * (
                                evalImgOnRefGrid.get(0, indy1, indx1) * (x2 - xe) * (y2 - ye) +
                                evalImgOnRefGrid.get(0, indy1, indx2) * (xe - x1) * (y2 - ye) +
                                evalImgOnRefGrid.get(0, indy2, indx1) * (x2 - xe) * (ye - y1) +
                                evalImgOnRefGrid.get(0, indy2, indx2) * (xe - x1) * (ye - y1));

                            // ALTERNATIVE WAY
                            // // ye = yr + point2.first;
                            // // y1 = yr + std::floor(point2.first / evalImgOnRefGrid.getSpacing().rows) * evalImgOnRefGrid.getSpacing().rows;
                            // // ye - y1 = (yr + point2.first) - (yr + floor(point2.first / evalImgOnRefGrid.getSpacing().rows) * evalImgOnRefGrid.getSpacing().rows);
                            // //         = point2.first - floor(point2.first / evalImgOnRefGrid.getSpacing().rows) * evalImgOnRefGrid.getSpacing().rows;
                            // float ye_y1 = point2.first - std::floor(point2.second / evalImgOnRefGrid.getSpacing().rows) * evalImgOnRefGrid.getSpacing().rows;
                            // float xe_x1 = point2.second - std::floor(point2.second / evalImgOnRefGrid.getSpacing().rows) * evalImgOnRefGrid.getSpacing().rows;
                            // float y2_ye = (indy2 - indy1) * (evalImgOnRefGrid.getSpacing().rows - ye_y1);
                            // float x2_xe = (indx2 - indx1) * (evalImgOnRefGrid.getSpacing().columns - xe_x1);
                            // // float y2_ye = indy1 != indy2 ? evalImgOnRefGrid.getSpacing().rows - ye_y1 : 0;
                            // // float x2_xe = indx1 != indx2 ? evalImgOnRefGrid.getSpacing().columns - xe_x1 : 0;
                            // // bilinear interpolation on the fly
                            // float doseEval = interpFactor * (
                            //     evalImgOnRefGrid.get(0, indy1, indx1) * x2_xe * y2_ye +
                            //     evalImgOnRefGrid.get(0, indy1, indx2) * xe_x1 * y2_ye +
                            //     evalImgOnRefGrid.get(0, indy2, indx1) * x2_xe * ye_y1 +
                            //     evalImgOnRefGrid.get(0, indy2, indx2) * xe_x1 * ye_y1);

                            // calculate squared gamma
                            float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + point.second * dtaInvSq;
                            if(gammaValSq < minGammaVal){
                                minGammaVal = gammaValSq;
                            }
                        }
                    }
                }

                if(atLeastOneInRange){
                    gammaVals.emplace_back(std::sqrt(minGammaVal));
                }
                else{
                    gammaVals.emplace_back(Nan);
                }
            }
            xr += refImg2D.getSpacing().columns;
            indRef++;
        }
        yr += refImg2D.getSpacing().rows;
    }

    return GammaResult(std::move(gammaVals), refImg2D.getSize(), refImg2D.getOffset(), refImg2D.getSpacing());
}

GammaResult gammaIndex2_5DWendling(const ImageData& refImg3D, const ImageData& evalImg3D, const GammaParameters& gammaParams){
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg3D.size());

    float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);

    float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const ImageData evalImgOnRefGrid = Interpolation::trilinear(evalImg3D, refImg3D);

    using YXPos = std::pair<float, float>;
    using PosWithDistSq = std::pair<YXPos, float>;
    std::vector<PosWithDistSq> pointsInCircle;
    float r = gammaParams.maxSearchDistance;
    pointsInCircle.reserve(static_cast<int>(r / refImg3D.getSpacing().frames) *
                           static_cast<int>(r / refImg3D.getSpacing().rows));
    float rSq = r * r;
    for(float y = 0; y <= r; y += gammaParams.stepSize){
        for(float x = 0; x <= r; x += gammaParams.stepSize){
            float distSq = y*y + x*x;
            if(distSq <= rSq){
                pointsInCircle.push_back({{y,x}, distSq});
            }
        }
    }
    std::sort(pointsInCircle.begin(), pointsInCircle.end(), [](const auto& lhs, const auto& rhs){
        return lhs.second < rhs.second;
    });

    const int kDiff = static_cast<int>((evalImgOnRefGrid.getOffset().frames - refImg3D.getOffset().frames) / evalImgOnRefGrid.getSpacing().frames);
    const float interpFactor = 1 / (refImg3D.getSpacing().rows * refImg3D.getSpacing().columns);

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        float yr = refImg3D.getOffset().rows;
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            float xr = refImg3D.getOffset().columns;
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                // assign a value of NaN to points on frames which have no counterpart in the eval image
                if(kr + kDiff < 0 || kr + kDiff >= evalImgOnRefGrid.getSize().frames){
                    gammaVals.emplace_back(Nan);
                    continue;
                }
                float doseRef = refImg3D.get(indRef);
                // assign a value of NaN to points that are below the dose cutoff
                // or where the gamma index calculation is not possible due to division by zero
                // (i.e. where reference dose used for local normalization is zero)
                if(doseRef < gammaParams.doseCutoff || (gammaParams.normalization == yagit::GammaNormalization::Local && doseRef == 0)){
                    gammaVals.emplace_back(Nan);
                }
                else{
                    float minGammaVal = Inf;
                    // set inversed normalized dd based on the type of normalization, whether it is global or local
                    float ddNormInvSq = ddInvSq * (
                        gammaParams.normalization == yagit::GammaNormalization::Global ? globalNormDoseInvSq
                                                                                       : (1 / (doseRef * doseRef)));
            
                    bool atLeastOneInRange = false;
                    for(const auto& point : pointsInCircle){
                        if(point.second * dtaInvSq > minGammaVal){
                            break;
                        }

                        auto [y, x] = point.first;
                        std::vector<YXPos> pointVariants;
                        pointVariants.reserve(4);
                        pointVariants.emplace_back(y, x);
                        if(y != 0 && x != 0){
                            pointVariants.emplace_back(-y, -x);
                        }
                        if(y != 0){
                            pointVariants.emplace_back(-y, x);
                        }
                        if(x != 0){
                            pointVariants.emplace_back(y, -x);
                        }
                        
                        for(const auto& point2 : pointVariants){
                            float ye = yr + point2.first;
                            float xe = xr + point2.second;

                            const int32_t indy1 = std::floor((ye - evalImgOnRefGrid.getOffset().rows) / evalImgOnRefGrid.getSpacing().rows);
                            const int32_t indx1 = std::floor((xe - evalImgOnRefGrid.getOffset().columns) / evalImgOnRefGrid.getSpacing().columns);
                            int32_t indy2 = indy1 + 1;
                            int32_t indx2 = indx1 + 1;

                            if(indy1 >= 0 && indy2 <= evalImgOnRefGrid.getSize().rows &&
                               indx1 >= 0 && indx2 <= evalImgOnRefGrid.getSize().columns){
                                atLeastOneInRange = true;

                                if(indy2 == evalImgOnRefGrid.getSize().rows){
                                    indy2 = indy1;
                                }
                                if(indx2 == evalImgOnRefGrid.getSize().columns){
                                    indx2 = indx1;
                                }

                                float y1 = evalImgOnRefGrid.getOffset().rows + indy1 * evalImgOnRefGrid.getSpacing().rows;
                                float x1 = evalImgOnRefGrid.getOffset().columns + indx1 * evalImgOnRefGrid.getSpacing().columns;
                                float y2 = y1 + (indy2 - indy1) * evalImgOnRefGrid.getSpacing().rows;
                                float x2 = x1 + (indx2 - indx1) * evalImgOnRefGrid.getSpacing().columns;

                                // bilinear interpolation on the fly
                                float doseEval = interpFactor * (
                                    evalImgOnRefGrid.get(kr + kDiff, indy1, indx1) * (x2 - xe) * (y2 - ye) +
                                    evalImgOnRefGrid.get(kr + kDiff, indy1, indx2) * (xe - x1) * (y2 - ye) +
                                    evalImgOnRefGrid.get(kr + kDiff, indy2, indx1) * (x2 - xe) * (ye - y1) +
                                    evalImgOnRefGrid.get(kr + kDiff, indy2, indx2) * (xe - x1) * (ye - y1));

                                // calculate squared gamma
                                float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + point.second * dtaInvSq;
                                if(gammaValSq < minGammaVal){
                                    minGammaVal = gammaValSq;
                                }
                            }
                        }
                    }

                    if(atLeastOneInRange){
                        gammaVals.emplace_back(std::sqrt(minGammaVal));
                    }
                    else{
                        gammaVals.emplace_back(Nan);
                    }
                }
                xr += refImg3D.getSpacing().columns;
                indRef++;
            }
            yr += refImg3D.getSpacing().rows;
        }
    }

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

GammaResult gammaIndex3DWendling(const ImageData& refImg3D, const ImageData& evalImg3D, const GammaParameters& gammaParams){
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg3D.size());

    float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);

    float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    // auto begin = std::chrono::steady_clock::now();
    const ImageData evalImgOnRefGrid = Interpolation::trilinear(evalImg3D, refImg3D);
    // auto end = std::chrono::steady_clock::now();
    // auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    // std::cout << "Interp time: " << timeMs << " [ms]" << std::endl;

    using ZYXPos = std::tuple<float, float, float>;
    using PosWithDistSq = std::pair<ZYXPos, float>;
    std::vector<PosWithDistSq> pointsInSphere;
    float r = gammaParams.maxSearchDistance;
    pointsInSphere.reserve(static_cast<int>(r / refImg3D.getSpacing().frames) *
                           static_cast<int>(r / refImg3D.getSpacing().rows) *
                           static_cast<int>(r / refImg3D.getSpacing().columns));
    float rSq = r * r;
    for(float z = 0; z <= r; z += gammaParams.stepSize){
        for(float y = 0; y <= r; y += gammaParams.stepSize){
            for(float x = 0; x <= r; x += gammaParams.stepSize){
                float distSq = z*z + y*y + x*x;
                if(distSq <= rSq){
                    pointsInSphere.push_back({{z,y,x}, distSq});
                }
            }
        }
    }
    std::sort(pointsInSphere.begin(), pointsInSphere.end(), [](const auto& lhs, const auto& rhs){
        return lhs.second < rhs.second;
    });

    const float interpFactor = 1 / (refImg3D.getSpacing().frames * refImg3D.getSpacing().rows * refImg3D.getSpacing().columns);

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    float zr = refImg3D.getOffset().frames;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        float yr = refImg3D.getOffset().rows;
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            float xr = refImg3D.getOffset().columns;
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                // if(kr == 150 && jr == 0){
                //     std::cout << ir << " ";
                // }
                float doseRef = refImg3D.get(indRef);
                // std::cout << indRef << " ";
                // assign a value of NaN to points that are below the dose cutoff
                // or where the gamma index calculation is not possible due to division by zero
                // (i.e. where reference dose used for local normalization is zero)
                if(doseRef < gammaParams.doseCutoff || (gammaParams.normalization == yagit::GammaNormalization::Local && doseRef == 0)){
                    gammaVals.emplace_back(Nan);
                }
                else{
                    float minGammaVal = Inf;
                    // set inversed normalized dd based on the type of normalization, whether it is global or local
                    float ddNormInvSq = ddInvSq * (
                        gammaParams.normalization == yagit::GammaNormalization::Global ? globalNormDoseInvSq
                                                                                       : (1 / (doseRef * doseRef)));
            
                    bool atLeastOneInRange = false;
                    for(const auto& point : pointsInSphere){
                        if(point.second * dtaInvSq > minGammaVal){
                            break;
                        }

                        auto [z, y, x] = point.first;
                        std::vector<ZYXPos> pointVariants;
                        pointVariants.reserve(8);
                        pointVariants.emplace_back(z,y,x);
                        if(z != 0 && y != 0 && x != 0){
                            pointVariants.emplace_back(-z, -y, -x);
                        }
                        if(z != 0 && y != 0){
                            pointVariants.emplace_back(-z, -y, x);
                        }
                        if(z != 0 && x != 0){
                            pointVariants.emplace_back(-z, y, -x);
                        }
                        if(y != 0 && x != 0){
                            pointVariants.emplace_back(z, -y, -x);
                        }
                        if(z != 0){
                            pointVariants.emplace_back(-z, y, x);
                        }
                        if(y != 0){
                            pointVariants.emplace_back(z, -y, x);
                        }
                        if(x != 0){
                            pointVariants.emplace_back(z, y, -x);
                        }
                        
                        for(const auto& point2 : pointVariants){
                            auto [dz, dy, dx] = point2;
                            float ze = zr + dz;
                            float ye = yr + dy;
                            float xe = xr + dx;

                            const int32_t indz0 = std::floor((ze - evalImgOnRefGrid.getOffset().frames) / evalImgOnRefGrid.getSpacing().frames);
                            const int32_t indy0 = std::floor((ye - evalImgOnRefGrid.getOffset().rows) / evalImgOnRefGrid.getSpacing().rows);
                            const int32_t indx0 = std::floor((xe - evalImgOnRefGrid.getOffset().columns) / evalImgOnRefGrid.getSpacing().columns);
                            int32_t indz1 = indz0 + 1;
                            int32_t indy1 = indy0 + 1;
                            int32_t indx1 = indx0 + 1;

                            // trilinear interpolation on the fly
                            if(indz0 >= 0 && indz1 <= evalImgOnRefGrid.getSize().frames &&
                               indy0 >= 0 && indy1 <= evalImgOnRefGrid.getSize().rows &&
                               indx0 >= 0 && indx1 <= evalImgOnRefGrid.getSize().columns){
                                atLeastOneInRange = true;

                                if(indz1 == evalImgOnRefGrid.getSize().frames){
                                    indz1 = indz0;
                                }
                                if(indy1 == evalImgOnRefGrid.getSize().rows){
                                    indy1 = indy0;
                                }
                                if(indx1 == evalImgOnRefGrid.getSize().columns){
                                    indx1 = indx0;
                                }

                                float z0 = evalImgOnRefGrid.getOffset().frames + indz0 * evalImgOnRefGrid.getSpacing().frames;
                                float y0 = evalImgOnRefGrid.getOffset().rows + indy0 * evalImgOnRefGrid.getSpacing().rows;
                                float x0 = evalImgOnRefGrid.getOffset().columns + indx0 * evalImgOnRefGrid.getSpacing().columns;
                                float z1 = z0 + (indz1 - indz0) * evalImgOnRefGrid.getSpacing().frames;
                                float y1 = y0 + (indy1 - indy0) * evalImgOnRefGrid.getSpacing().rows;
                                float x1 = x0 + (indx1 - indx0) * evalImgOnRefGrid.getSpacing().columns;

                                float c000 = evalImgOnRefGrid.get(indz0, indy0, indx0);
                                float c001 = evalImgOnRefGrid.get(indz1, indy0, indx0);
                                float c010 = evalImgOnRefGrid.get(indz0, indy1, indx0);
                                float c011 = evalImgOnRefGrid.get(indz1, indy1, indx0);
                                float c100 = evalImgOnRefGrid.get(indz0, indy0, indx1);
                                float c101 = evalImgOnRefGrid.get(indz1, indy0, indx1);
                                float c110 = evalImgOnRefGrid.get(indz0, indy1, indx1);
                                float c111 = evalImgOnRefGrid.get(indz1, indy1, indx1);

                                float zd = (ze - z0) / evalImgOnRefGrid.getSpacing().frames;
                                float yd = (ye - y0) / evalImgOnRefGrid.getSpacing().rows;
                                float xd = (xe - x0) / evalImgOnRefGrid.getSpacing().columns;

                                float c00 = c000*(1 - xd) + c100*xd;
                                float c01 = c001*(1 - xd) + c101*xd;
                                float c10 = c010*(1 - xd) + c110*xd;
                                float c11 = c011*(1 - xd) + c111*xd;

                                float c0 = c00*(1 - yd) + c10*yd;
                                float c1 = c01*(1 - yd) + c11*yd;

                                float doseEval = c0*(1 - zd) + c1*zd;

                                // ALTERNATIVE VERSION
                                // float a0 = -c000*x1*y1*z1 + c001*x1*y1*z0 + c010*x1*y0*z1 - c011*x1*y0*z0 +
                                //            c100*x0*y1*z1 - c101*x0*y1*z0 - c110*x0*y0*z1 + c111*x0*y0*z0;
                                // float a1 = c000*y1*z1 - c001*y1*z0 - c010*y0*z1 + c011*y0*z0 -
                                //            c100*y1*z1 + c101*y1*z0 + c110*y0*z1 + c111*y0*z0;
                                // float a2 = c000*x1*z1 - c001*x1*z0 - c010*x1*z1 + c011*x1*z0 -
                                //            c100*x0*z1 + c101*x0*z0 + c110*x0*z1 - c111*x0*z0;
                                // float a3 = c000*x1*y1 - c001*x1*y1 - c010*x1*y0 + c011*x1*y0 -
                                //            c100*x0*y1 + c101*x0*y1 + c110*x0*y0 - c111*x0*y0;
                                // float a4 = -c000*z1 + c001*z0 + c010*z1 - c011*z0 + c100*z1 - c101*z0 - c110*z1 + c111*z0;
                                // float a5 = -c000*y1 + c001*y1 + c010*y0 - c011*y0 + c100*y1 - c101*y1 - c110*y0 + c111*y0;
                                // float a6 = -c000*x1 + c001*x1 + c010*x1 - c011*x1 + c100*x0 - c101*x0 - c110*x0 + c111*x0;
                                // float a7 = c000 - c001 - c010 + c011 - c100 + c101 + c110 - c111;

                                // float doseEval = interpFactor * (
                                //     a0 + a1*xe + a2*ye + a3*ze + a4*xe*ye + a5*xe*ze + a6*ye*ze + a7*xe*ye*ze
                                // );

                                // calculate squared gamma
                                float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + point.second * dtaInvSq;
                                if(gammaValSq < minGammaVal){
                                    minGammaVal = gammaValSq;
                                }
                            }
                        }
                    }

                    if(atLeastOneInRange){
                        gammaVals.emplace_back(std::sqrt(minGammaVal));
                    }
                    else{
                        gammaVals.emplace_back(Nan);
                    }
                }
                xr += refImg3D.getSpacing().columns;
                indRef++;
            }
            yr += refImg3D.getSpacing().rows;
        }
        zr += refImg3D.getSpacing().frames;
    }

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

}
