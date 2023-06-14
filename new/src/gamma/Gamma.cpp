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

#include <cmath>
#include <limits>

#include "GammaCommon.hpp"
#include "Interpolation.hpp"

namespace yagit{

GammaResult gammaIndex2D(const ImageData& refImg2D, const ImageData& evalImg2D, const GammaParameters& gammaParams){
    validateImages2D(refImg2D, evalImg2D);
    validateGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg2D.size());

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    // iterate over each row and column of reference image
    size_t indRef = 0;
    float yr = refImg2D.getOffset().rows;
    for(uint32_t jr = 0; jr < refImg2D.getSize().rows; jr++){
        float xr = refImg2D.getOffset().columns;
        for(uint32_t ir = 0; ir < refImg2D.getSize().columns; ir++){
            float doseRef = refImg2D.get(indRef);

            bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
            bool divisionByZero = !isGlobal && doseRef == 0;
            if(doseBelowCutoff || divisionByZero){
                gammaVals.emplace_back(Nan);
            }
            else{
                float minGammaValSq = Inf;
                // set squared inversed normalized dd based on the type of normalization, whether it is global or local
                float ddNormInvSq = ddInvSq * (isGlobal ? globalNormDoseInvSq : (1 / (doseRef * doseRef)));

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

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    float zr = refImg3D.getOffset().frames;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        float yr = refImg3D.getOffset().rows;
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            float xr = refImg3D.getOffset().columns;
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                float doseRef = refImg3D.get(indRef);

                bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
                bool divisionByZero = !isGlobal && doseRef == 0;
                if(doseBelowCutoff || divisionByZero){
                    gammaVals.emplace_back(Nan);
                }
                else{
                    // set squared inversed normalized dd based on the type of normalization, whether it is global or local
                    float ddNormInvSq = ddInvSq * (isGlobal ? globalNormDoseInvSq : (1 / (doseRef * doseRef)));
                    float minGammaValSq = Inf;

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
                            if(gammaValSq < minGammaValSq){
                                minGammaValSq = gammaValSq;
                            }

                            xe += evalImg3D.getSpacing().columns;
                            indEval++;
                        }
                        ye += evalImg3D.getSpacing().rows;
                    }

                    gammaVals.emplace_back(std::sqrt(minGammaValSq));
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

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    float zr = refImg3D.getOffset().frames;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        float yr = refImg3D.getOffset().rows;
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            float xr = refImg3D.getOffset().columns;
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                float doseRef = refImg3D.get(indRef);

                bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
                bool divisionByZero = !isGlobal && doseRef == 0;
                if(doseBelowCutoff || divisionByZero){
                    gammaVals.emplace_back(Nan);
                }
                else{
                    // set squared inversed normalized dd based on the type of normalization, whether it is global or local
                    float ddNormInvSq = ddInvSq * (isGlobal ? globalNormDoseInvSq : (1 / (doseRef * doseRef)));
                    float minGammaValSq = Inf;

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
                                if(gammaValSq < minGammaValSq){
                                    minGammaValSq = gammaValSq;
                                }

                                xe += evalImg3D.getSpacing().columns;
                                indEval++;
                            }
                            ye += evalImg3D.getSpacing().rows;
                        }
                        ze += evalImg3D.getSpacing().frames;
                    }

                    gammaVals.emplace_back(std::sqrt(minGammaValSq));
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
    validateImages2D(refImg2D, evalImg2D);
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg2D.size());

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);
    const float interpFactor = 1 / (refImg2D.getSpacing().rows * refImg2D.getSpacing().columns);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const auto sortedPoints = sortedPointsInQuarterCircle(gammaParams.maxSearchDistance, gammaParams.stepSize);

    // iterate over each row and column of reference image
    size_t indRef = 0;
    float yr = refImg2D.getOffset().rows;
    for(uint32_t jr = 0; jr < refImg2D.getSize().rows; jr++){
        float xr = refImg2D.getOffset().columns;
        for(uint32_t ir = 0; ir < refImg2D.getSize().columns; ir++){
            float doseRef = refImg2D.get(indRef);

            bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
            bool divisionByZero = !isGlobal && doseRef == 0;
            if(doseBelowCutoff || divisionByZero){
                gammaVals.emplace_back(Nan);
            }
            else{
                float minGammaValSq = Inf;
                // set squared inversed normalized dd based on the type of normalization, whether it is global or local
                float ddNormInvSq = ddInvSq * (isGlobal ? globalNormDoseInvSq : (1 / (doseRef * doseRef)));

                bool atLeastOneInRange = false;
                for(const auto& point : sortedPoints){
                    const float normalizedDistSq = point.second * dtaInvSq;
                    if(normalizedDistSq >= minGammaValSq){
                        break;
                    }

                    auto [dy, dx] = point.first;
                    float ye = yr + dy;
                    float xe = xr + dx;

                    // instead of calling Interpolate::bilinearAtPoint function,
                    // here is an inlined, optimized version. It gives 5-10% speedup

                    const int32_t indy1 = std::floor((ye - evalImg2D.getOffset().rows) / evalImg2D.getSpacing().rows);
                    const int32_t indx1 = std::floor((xe - evalImg2D.getOffset().columns) / evalImg2D.getSpacing().columns);
                    int32_t indy2 = indy1 + 1;
                    int32_t indx2 = indx1 + 1;

                    if(indy1 >= 0 && indy2 <= static_cast<int32_t>(evalImg2D.getSize().rows) &&
                        indx1 >= 0 && indx2 <= static_cast<int32_t>(evalImg2D.getSize().columns)){
                        atLeastOneInRange = true;

                        if(indy2 == static_cast<int32_t>(evalImg2D.getSize().rows)){
                            indy2 = indy1;
                        }
                        if(indx2 == static_cast<int32_t>(evalImg2D.getSize().columns)){
                            indx2 = indx1;
                        }

                        float y1 = evalImg2D.getOffset().rows + indy1 * evalImg2D.getSpacing().rows;
                        float x1 = evalImg2D.getOffset().columns + indx1 * evalImg2D.getSpacing().columns;
                        float y2 = y1 + (indy2 - indy1) * evalImg2D.getSpacing().rows;
                        float x2 = x1 + (indx2 - indx1) * evalImg2D.getSpacing().columns;

                        // bilinear interpolation on the fly
                        float doseEval = interpFactor * (
                            evalImg2D.get(0, indy1, indx1) * (x2 - xe) * (y2 - ye) +
                            evalImg2D.get(0, indy1, indx2) * (xe - x1) * (y2 - ye) +
                            evalImg2D.get(0, indy2, indx1) * (x2 - xe) * (ye - y1) +
                            evalImg2D.get(0, indy2, indx2) * (xe - x1) * (ye - y1));

                        // calculate squared gamma
                        float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + normalizedDistSq;
                        if(gammaValSq < minGammaValSq){
                            minGammaValSq = gammaValSq;
                        }
                    }
                }

                if(atLeastOneInRange){
                    gammaVals.emplace_back(std::sqrt(minGammaValSq));
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

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);
    const float interpFactor = 1 / (refImg3D.getSpacing().rows * refImg3D.getSpacing().columns);
    const float rowsSpInv = 1 / evalImg3D.getSpacing().rows;
    const float columnsSpInv = 1 / evalImg3D.getSpacing().columns;

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const ImageData evalImgInterpolatedZ = Interpolation::linearAlongAxis(evalImg3D, refImg3D, ImageAxis::Z);
    const int kDiff = static_cast<int>((evalImgInterpolatedZ.getOffset().frames - refImg3D.getOffset().frames) / evalImgInterpolatedZ.getSpacing().frames);

    const auto sortedPoints = sortedPointsInQuarterCircle(gammaParams.maxSearchDistance, gammaParams.stepSize);

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        float yr = refImg3D.getOffset().rows;
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            float xr = refImg3D.getOffset().columns;
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                float doseRef = refImg3D.get(indRef);
                const int ke = static_cast<int>(kr) + kDiff;

                bool evalFrameOutsideImage = ke < 0 || ke >= static_cast<int>(evalImgInterpolatedZ.getSize().frames);
                bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
                bool divisionByZero = !isGlobal && doseRef == 0;
                if(evalFrameOutsideImage || doseBelowCutoff || divisionByZero){
                    gammaVals.emplace_back(Nan);
                }
                else{
                    float minGammaValSq = Inf;
                    // set squared inversed normalized dd based on the type of normalization, whether it is global or local
                    float ddNormInvSq = ddInvSq * (isGlobal ? globalNormDoseInvSq : (1 / (doseRef * doseRef)));

                    bool atLeastOneInRange = false;
                    for(const auto& point : sortedPoints){
                        const float normalizedDistSq = point.second * dtaInvSq;
                        if(normalizedDistSq >= minGammaValSq){
                            break;
                        }

                        auto [dy, dx] = point.first;
                        float ye = yr + dy;
                        float xe = xr + dx;

                        // instead of calling Interpolate::bilinearAtPoint function,
                        // here is an inlined, optimized version. It gives 5-10% speedup

                        const int32_t indy1 = std::floor((ye - evalImgInterpolatedZ.getOffset().rows) * rowsSpInv);
                        const int32_t indx1 = std::floor((xe - evalImgInterpolatedZ.getOffset().columns) * columnsSpInv);
                        int32_t indy2 = indy1 + 1;
                        int32_t indx2 = indx1 + 1;

                        if(indy1 >= 0 && indy2 <= static_cast<int32_t>(evalImgInterpolatedZ.getSize().rows) &&
                            indx1 >= 0 && indx2 <= static_cast<int32_t>(evalImgInterpolatedZ.getSize().columns)){
                            atLeastOneInRange = true;

                            if(indy2 == static_cast<int32_t>(evalImgInterpolatedZ.getSize().rows)){
                                indy2 = indy1;
                            }
                            if(indx2 == static_cast<int32_t>(evalImgInterpolatedZ.getSize().columns)){
                                indx2 = indx1;
                            }

                            float y1 = evalImgInterpolatedZ.getOffset().rows + indy1 * evalImgInterpolatedZ.getSpacing().rows;
                            float x1 = evalImgInterpolatedZ.getOffset().columns + indx1 * evalImgInterpolatedZ.getSpacing().columns;
                            float y2 = y1 + (indy2 - indy1) * evalImgInterpolatedZ.getSpacing().rows;
                            float x2 = x1 + (indx2 - indx1) * evalImgInterpolatedZ.getSpacing().columns;

                            // bilinear interpolation on the fly
                            float doseEval = interpFactor * (
                                evalImgInterpolatedZ.get(ke, indy1, indx1) * (x2 - xe) * (y2 - ye) +
                                evalImgInterpolatedZ.get(ke, indy1, indx2) * (xe - x1) * (y2 - ye) +
                                evalImgInterpolatedZ.get(ke, indy2, indx1) * (x2 - xe) * (ye - y1) +
                                evalImgInterpolatedZ.get(ke, indy2, indx2) * (xe - x1) * (ye - y1));

                            // calculate squared gamma
                            float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + normalizedDistSq;
                            if(gammaValSq < minGammaValSq){
                                minGammaValSq = gammaValSq;
                            }
                        }
                    }

                    if(atLeastOneInRange){
                        gammaVals.emplace_back(std::sqrt(minGammaValSq));
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

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float globalNormDoseInvSq = 1 / (gammaParams.globalNormDose * gammaParams.globalNormDose);
    const float framesSpInv = 1 / evalImg3D.getSpacing().frames;
    const float rowsSpInv = 1 / evalImg3D.getSpacing().rows;
    const float columnsSpInv = 1 / evalImg3D.getSpacing().columns;

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    // TODO: check if interpolating evalImg on the grid of refImg
    // and precalculating interpolation factors (for on-the-fly interpolation) will be much faster.
    // note that result will be less accurate due to interpolating twice

    const auto sortedPoints = sortedPointsInEighthOfSphere(gammaParams.maxSearchDistance, gammaParams.stepSize);

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    float zr = refImg3D.getOffset().frames;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        float yr = refImg3D.getOffset().rows;
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            float xr = refImg3D.getOffset().columns;
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                float doseRef = refImg3D.get(indRef);
                
                bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
                bool divisionByZero = !isGlobal && doseRef == 0;
                if(doseBelowCutoff || divisionByZero){
                    gammaVals.emplace_back(Nan);
                }
                else{
                    float minGammaValSq = Inf;
                    // set squared inversed normalized dd based on the type of normalization, whether it is global or local
                    float ddNormInvSq = ddInvSq * (isGlobal ? globalNormDoseInvSq : (1 / (doseRef * doseRef)));

                    bool atLeastOneInRange = false;
                    for(const auto& point : sortedPoints){
                        const float normalizedDistSq = point.second * dtaInvSq;
                        if(normalizedDistSq >= minGammaValSq){
                            break;
                        }

                        auto [dz, dy, dx] = point.first;
                        float ze = zr + dz;
                        float ye = yr + dy;
                        float xe = xr + dx;

                        // instead of calling Interpolate::trilinearAtPoint function,
                        // here is an inlined, optimized version. It gives 5-10% speedup

                        const int32_t indz0 = std::floor((ze - evalImg3D.getOffset().frames) * framesSpInv);
                        const int32_t indy0 = std::floor((ye - evalImg3D.getOffset().rows) * rowsSpInv);
                        const int32_t indx0 = std::floor((xe - evalImg3D.getOffset().columns) * columnsSpInv);
                        int32_t indz1 = indz0 + 1;
                        int32_t indy1 = indy0 + 1;
                        int32_t indx1 = indx0 + 1;

                        if(indz0 >= 0 && indz1 <= static_cast<int32_t>(evalImg3D.getSize().frames) &&
                            indy0 >= 0 && indy1 <= static_cast<int32_t>(evalImg3D.getSize().rows) &&
                            indx0 >= 0 && indx1 <= static_cast<int32_t>(evalImg3D.getSize().columns)){
                            atLeastOneInRange = true;

                            if(indz1 == static_cast<int32_t>(evalImg3D.getSize().frames)){
                                indz1 = indz0;
                            }
                            if(indy1 == static_cast<int32_t>(evalImg3D.getSize().rows)){
                                indy1 = indy0;
                            }
                            if(indx1 == static_cast<int32_t>(evalImg3D.getSize().columns)){
                                indx1 = indx0;
                            }

                            float z0 = evalImg3D.getOffset().frames + indz0 * evalImg3D.getSpacing().frames;
                            float y0 = evalImg3D.getOffset().rows + indy0 * evalImg3D.getSpacing().rows;
                            float x0 = evalImg3D.getOffset().columns + indx0 * evalImg3D.getSpacing().columns;

                            float c000 = evalImg3D.get(indz0, indy0, indx0);
                            float c001 = evalImg3D.get(indz1, indy0, indx0);
                            float c010 = evalImg3D.get(indz0, indy1, indx0);
                            float c011 = evalImg3D.get(indz1, indy1, indx0);
                            float c100 = evalImg3D.get(indz0, indy0, indx1);
                            float c101 = evalImg3D.get(indz1, indy0, indx1);
                            float c110 = evalImg3D.get(indz0, indy1, indx1);
                            float c111 = evalImg3D.get(indz1, indy1, indx1);

                            float zd = (ze - z0) * framesSpInv;
                            float yd = (ye - y0) * rowsSpInv;
                            float xd = (xe - x0) * columnsSpInv;

                            float c00 = c000*(1 - xd) + c100*xd;
                            float c01 = c001*(1 - xd) + c101*xd;
                            float c10 = c010*(1 - xd) + c110*xd;
                            float c11 = c011*(1 - xd) + c111*xd;

                            float c0 = c00*(1 - yd) + c10*yd;
                            float c1 = c01*(1 - yd) + c11*yd;

                            // trilinear interpolation on the fly
                            float doseEval = c0*(1 - zd) + c1*zd;

                            // calculate squared gamma
                            float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + normalizedDistSq;
                            if(gammaValSq < minGammaValSq){
                                minGammaValSq = gammaValSq;
                            }
                        }
                    }

                    if(atLeastOneInRange){
                        gammaVals.emplace_back(std::sqrt(minGammaValSq));
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
