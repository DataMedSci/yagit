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

#include <cmath>
#include <limits>

#include "yagit/Interpolation.hpp"
#include "GammaCommon.hpp"

namespace yagit{

GammaResult gammaIndex2D(const ImageData& refImg2D, const ImageData& evalImg2D,
                         const GammaParameters& gammaParams, GammaMethod method){
    if(method == GammaMethod::Wendling){
        return gammaIndex2DWendling(refImg2D, evalImg2D, gammaParams);
    }
    else if(method == GammaMethod::Classic){
        return gammaIndex2DClassic(refImg2D, evalImg2D, gammaParams);
    }
    else{
        throw std::invalid_argument("invalid method");
    }
}

GammaResult gammaIndex2_5D(const ImageData& refImg3D, const ImageData& evalImg3D,
                           const GammaParameters& gammaParams, GammaMethod method){
    if(method == GammaMethod::Wendling){
        return gammaIndex2_5DWendling(refImg3D, evalImg3D, gammaParams);
    }
    else if(method == GammaMethod::Classic){
        return gammaIndex2_5DClassic(refImg3D, evalImg3D, gammaParams);
    }
    else{
        throw std::invalid_argument("invalid method");
    }
}

GammaResult gammaIndex3D(const ImageData& refImg3D, const ImageData& evalImg3D,
                         const GammaParameters& gammaParams, GammaMethod method){
    if(method == GammaMethod::Wendling){
        return gammaIndex3DWendling(refImg3D, evalImg3D, gammaParams);
    }
    else if(method == GammaMethod::Classic){
        return gammaIndex3DClassic(refImg3D, evalImg3D, gammaParams);
    }
    else{
        throw std::invalid_argument("invalid method");
    }
}

GammaResult gammaIndex2DClassic(const ImageData& refImg2D, const ImageData& evalImg2D,
                                const GammaParameters& gammaParams){
    validateImages2D(refImg2D, evalImg2D);
    validateGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg2D.size());

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const std::vector<float> yr = generateCoordinates(refImg2D, ImageAxis::Y);
    const std::vector<float> xr = generateCoordinates(refImg2D, ImageAxis::X);
    const std::vector<float> ye = generateCoordinates(evalImg2D, ImageAxis::Y);
    const std::vector<float> xe = generateCoordinates(evalImg2D, ImageAxis::X);

    // iterate over each row and column of reference image
    size_t indRef = 0;
    for(uint32_t jr = 0; jr < refImg2D.getSize().rows; jr++){
        for(uint32_t ir = 0; ir < refImg2D.getSize().columns; ir++){
            float doseRef = refImg2D.get(indRef);

            bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
            bool divisionByZero = !isGlobal && doseRef == 0;
            if(doseBelowCutoff || divisionByZero){
                gammaVals.emplace_back(NaN);
            }
            else{
                float minGammaValSq = Inf;
                // set squared inversed normalized dd based on the type of normalization (global or local)
                float ddNormInvSq = (isGlobal ? ddGlobalNormInvSq : (ddInvSq / (doseRef * doseRef)));

                // iterate over each row and column of evaluated image
                size_t indEval = 0;
                for(uint32_t je = 0; je < evalImg2D.getSize().rows; je++){
                    for(uint32_t ie = 0; ie < evalImg2D.getSize().columns; ie++){
                        float doseEval = evalImg2D.get(indEval);
                        // calculate squared gamma
                        float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq +
                                           distSq2D(xe[ie], ye[je], xr[ir], yr[jr]) * dtaInvSq;
                        if(gammaValSq < minGammaValSq){
                            minGammaValSq = gammaValSq;
                        }

                        indEval++;
                    }
                }

                gammaVals.emplace_back(std::sqrt(minGammaValSq));
            }

            indRef++;
        }
    }

    return GammaResult(std::move(gammaVals), refImg2D.getSize(), refImg2D.getOffset(), refImg2D.getSpacing());
}

GammaResult gammaIndex2_5DClassic(const ImageData& refImg3D, const ImageData& evalImg3D,
                                  const GammaParameters& gammaParams){
    if(refImg3D.getSize().frames != evalImg3D.getSize().frames){
        throw std::invalid_argument("reference image and evaluated image don't have the same number of frames");
    }
    validateGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg3D.size());

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const std::vector<float> zr = generateCoordinates(refImg3D, ImageAxis::Z);
    const std::vector<float> yr = generateCoordinates(refImg3D, ImageAxis::Y);
    const std::vector<float> xr = generateCoordinates(refImg3D, ImageAxis::X);
    const std::vector<float> ze = generateCoordinates(evalImg3D, ImageAxis::Z);
    const std::vector<float> ye = generateCoordinates(evalImg3D, ImageAxis::Y);
    const std::vector<float> xe = generateCoordinates(evalImg3D, ImageAxis::X);

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                float doseRef = refImg3D.get(indRef);

                bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
                bool divisionByZero = !isGlobal && doseRef == 0;
                if(doseBelowCutoff || divisionByZero){
                    gammaVals.emplace_back(NaN);
                }
                else{
                    // set squared inversed normalized dd based on the type of normalization (global or local)
                    float ddNormInvSq = (isGlobal ? ddGlobalNormInvSq : (ddInvSq / (doseRef * doseRef)));
                    float minGammaValSq = Inf;

                    // iterate over each row and column of evaluated image
                    size_t indEval = kr * evalImg3D.getSize().rows * refImg3D.getSize().columns;
                    for(uint32_t je = 0; je < evalImg3D.getSize().rows; je++){
                        for(uint32_t ie = 0; ie < evalImg3D.getSize().columns; ie++){
                            float doseEval = evalImg3D.get(indEval);
                            // calculate squared gamma
                            float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq +
                                               distSq3D(xe[ie], ye[je], ze[kr], xr[ir], yr[jr], zr[kr]) * dtaInvSq;
                            if(gammaValSq < minGammaValSq){
                                minGammaValSq = gammaValSq;
                            }

                            indEval++;
                        }
                    }

                    gammaVals.emplace_back(std::sqrt(minGammaValSq));
                }

                indRef++;
            }
        }
    }

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

GammaResult gammaIndex3DClassic(const ImageData& refImg3D, const ImageData& evalImg3D,
                                const GammaParameters& gammaParams){
    validateGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg3D.size());

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const std::vector<float> zr = generateCoordinates(refImg3D, ImageAxis::Z);
    const std::vector<float> yr = generateCoordinates(refImg3D, ImageAxis::Y);
    const std::vector<float> xr = generateCoordinates(refImg3D, ImageAxis::X);
    const std::vector<float> ze = generateCoordinates(evalImg3D, ImageAxis::Z);
    const std::vector<float> ye = generateCoordinates(evalImg3D, ImageAxis::Y);
    const std::vector<float> xe = generateCoordinates(evalImg3D, ImageAxis::X);

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                float doseRef = refImg3D.get(indRef);

                bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
                bool divisionByZero = !isGlobal && doseRef == 0;
                if(doseBelowCutoff || divisionByZero){
                    gammaVals.emplace_back(NaN);
                }
                else{
                    // set squared inversed normalized dd based on the type of normalization (global or local)
                    float ddNormInvSq = (isGlobal ? ddGlobalNormInvSq : (ddInvSq / (doseRef * doseRef)));
                    float minGammaValSq = Inf;

                    // iterate over each frame, row and column of evaluated image
                    size_t indEval = 0;
                    for(uint32_t ke = 0; ke < evalImg3D.getSize().frames; ke++){
                        for(uint32_t je = 0; je < evalImg3D.getSize().rows; je++){
                            for(uint32_t ie = 0; ie < evalImg3D.getSize().columns; ie++){
                                float doseEval = evalImg3D.get(indEval);
                                // calculate squared gamma
                                float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq +
                                                   distSq3D(xe[ie], ye[je], ze[ke], xr[ir], yr[jr], zr[kr]) * dtaInvSq;
                                if(gammaValSq < minGammaValSq){
                                    minGammaValSq = gammaValSq;
                                }

                                indEval++;
                            }
                        }
                    }

                    gammaVals.emplace_back(std::sqrt(minGammaValSq));
                }

                indRef++;
            }
        }
    }

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

GammaResult gammaIndex2DWendling(const ImageData& refImg2D, const ImageData& evalImg2D,
                                 const GammaParameters& gammaParams){
    validateImages2D(refImg2D, evalImg2D);
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg2D.size());

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const float rowsSpInv = 1 / evalImg2D.getSpacing().rows;
    const float columnsSpInv = 1 / evalImg2D.getSpacing().columns;

    const float yeMin = evalImg2D.getOffset().rows - Tolerance;
    const float xeMin = evalImg2D.getOffset().columns - Tolerance;
    const float yeMax = evalImg2D.getOffset().rows + (evalImg2D.getSize().rows - 1) * evalImg2D.getSpacing().rows + Tolerance;
    const float xeMax = evalImg2D.getOffset().columns + (evalImg2D.getSize().columns - 1) * evalImg2D.getSpacing().columns + Tolerance;

    const auto sortedPoints = sortedPointsInCircle(gammaParams.maxSearchDistance, gammaParams.stepSize);

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
                gammaVals.emplace_back(NaN);
            }
            else{
                // set squared inversed normalized dd based on the type of normalization (global or local)
                float ddNormInvSq = (isGlobal ? ddGlobalNormInvSq : (ddInvSq / (doseRef * doseRef)));

                float minGammaValSq = Inf;

                for(const auto& point : sortedPoints){
                    const float normalizedDistSq = point.distSq * dtaInvSq;
                    if(normalizedDistSq >= minGammaValSq){
                        break;
                    }

                    float ye = yr + point.y;
                    float xe = xr + point.x;

                    // instead of calling Interpolate::bilinearAtPoint function,
                    // here is an inlined, optimized version. It gives 5-10% speedup

                    if(ye < yeMin || ye > yeMax ||
                       xe < xeMin || xe > xeMax){
                        continue;
                    }

                    float tempy = (ye - evalImg2D.getOffset().rows) * rowsSpInv;
                    float tempx = (xe - evalImg2D.getOffset().columns) * columnsSpInv;

                    const uint32_t indy0 = static_cast<uint32_t>(tempy);
                    const uint32_t indx0 = static_cast<uint32_t>(tempx);
                    uint32_t indy1 = indy0 + 1;
                    uint32_t indx1 = indx0 + 1;

                    if(indy1 == evalImg2D.getSize().rows){
                        indy1 = indy0;
                    }
                    if(indx1 == evalImg2D.getSize().columns){
                        indx1 = indx0;
                    }

                    float yd = tempy - static_cast<float>(indy0);
                    float xd = tempx - static_cast<float>(indx0);

                    float c00 = evalImg2D.get(0, indy0, indx0);
                    float c01 = evalImg2D.get(0, indy1, indx0);
                    float c10 = evalImg2D.get(0, indy0, indx1);
                    float c11 = evalImg2D.get(0, indy1, indx1);

                    float c0 = c00*(1 - xd) + c10*xd;
                    float c1 = c01*(1 - xd) + c11*xd;

                    float doseEval = c0*(1 - yd) + c1*yd;

                    // calculate squared gamma
                    float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + normalizedDistSq;
                    if(gammaValSq < minGammaValSq){
                        minGammaValSq = gammaValSq;
                    }
                }

                if(minGammaValSq != Inf){
                    gammaVals.emplace_back(std::sqrt(minGammaValSq));
                }
                else{
                    gammaVals.emplace_back(NaN);
                }
            }
            xr += refImg2D.getSpacing().columns;
            indRef++;
        }
        yr += refImg2D.getSpacing().rows;
    }

    return GammaResult(std::move(gammaVals), refImg2D.getSize(), refImg2D.getOffset(), refImg2D.getSpacing());
}

GammaResult gammaIndex2_5DWendling(const ImageData& refImg3D, const ImageData& evalImg3D,
                                   const GammaParameters& gammaParams){
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    const ImageData evalImgInterpolatedZ = Interpolation::linearAlongAxis(evalImg3D, refImg3D, ImageAxis::Z);
    const int kDiff = static_cast<int>((refImg3D.getOffset().frames - evalImgInterpolatedZ.getOffset().frames) / refImg3D.getSpacing().frames);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg3D.size());

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const float rowsSpInv = 1 / evalImgInterpolatedZ.getSpacing().rows;
    const float columnsSpInv = 1 / evalImgInterpolatedZ.getSpacing().columns;

    const float yeMin = evalImgInterpolatedZ.getOffset().rows - Tolerance;
    const float xeMin = evalImgInterpolatedZ.getOffset().columns - Tolerance;
    const float yeMax = evalImgInterpolatedZ.getOffset().rows +
        (evalImgInterpolatedZ.getSize().rows - 1) * evalImgInterpolatedZ.getSpacing().rows + Tolerance;
    const float xeMax = evalImgInterpolatedZ.getOffset().columns +
        (evalImgInterpolatedZ.getSize().columns - 1) * evalImgInterpolatedZ.getSpacing().columns + Tolerance;

    const auto sortedPoints = sortedPointsInCircle(gammaParams.maxSearchDistance, gammaParams.stepSize);

    // iterate over each frame, row and column of reference image
    size_t indRef = 0;
    int ke = kDiff;
    for(uint32_t kr = 0; kr < refImg3D.getSize().frames; kr++){
        float yr = refImg3D.getOffset().rows;
        for(uint32_t jr = 0; jr < refImg3D.getSize().rows; jr++){
            float xr = refImg3D.getOffset().columns;
            for(uint32_t ir = 0; ir < refImg3D.getSize().columns; ir++){
                float doseRef = refImg3D.get(indRef);

                bool evalFrameOutsideImage = ke < 0 || ke >= static_cast<int>(evalImgInterpolatedZ.getSize().frames);
                bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
                bool divisionByZero = !isGlobal && doseRef == 0;
                if(evalFrameOutsideImage || doseBelowCutoff || divisionByZero){
                    gammaVals.emplace_back(NaN);
                }
                else{
                    // set squared inversed normalized dd based on the type of normalization (global or local)
                    float ddNormInvSq = (isGlobal ? ddGlobalNormInvSq : (ddInvSq / (doseRef * doseRef)));

                    float minGammaValSq = Inf;

                    for(const auto& point : sortedPoints){
                        const float normalizedDistSq = point.distSq * dtaInvSq;
                        if(normalizedDistSq >= minGammaValSq){
                            break;
                        }

                        float ye = yr + point.y;
                        float xe = xr + point.x;

                        // instead of calling Interpolate::bilinearAtPoint function,
                        // here is an inlined, optimized version. It gives 5-10% speedup

                        if(ye < yeMin || ye > yeMax ||
                           xe < xeMin || xe > xeMax){
                            continue;
                        }

                        float tempy = (ye - evalImgInterpolatedZ.getOffset().rows) * rowsSpInv;
                        float tempx = (xe - evalImgInterpolatedZ.getOffset().columns) * columnsSpInv;

                        const uint32_t indy0 = static_cast<uint32_t>(tempy);
                        const uint32_t indx0 = static_cast<uint32_t>(tempx);
                        uint32_t indy1 = indy0 + 1;
                        uint32_t indx1 = indx0 + 1;

                        if(indy1 == evalImgInterpolatedZ.getSize().rows){
                            indy1 = indy0;
                        }
                        if(indx1 == evalImgInterpolatedZ.getSize().columns){
                            indx1 = indx0;
                        }

                        float yd = tempy - static_cast<float>(indy0);
                        float xd = tempx - static_cast<float>(indx0);

                        float c00 = evalImgInterpolatedZ.get(ke, indy0, indx0);
                        float c01 = evalImgInterpolatedZ.get(ke, indy1, indx0);
                        float c10 = evalImgInterpolatedZ.get(ke, indy0, indx1);
                        float c11 = evalImgInterpolatedZ.get(ke, indy1, indx1);

                        float c0 = c00*(1 - xd) + c10*xd;
                        float c1 = c01*(1 - xd) + c11*xd;

                        float doseEval = c0*(1 - yd) + c1*yd;

                        // calculate squared gamma
                        float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + normalizedDistSq;
                        if(gammaValSq < minGammaValSq){
                            minGammaValSq = gammaValSq;
                        }
                    }

                    if(minGammaValSq != Inf){
                        gammaVals.emplace_back(std::sqrt(minGammaValSq));
                    }
                    else{
                        gammaVals.emplace_back(NaN);
                    }
                }
                xr += refImg3D.getSpacing().columns;
                indRef++;
            }
            yr += refImg3D.getSpacing().rows;
        }
        ke++;
    }

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

GammaResult gammaIndex3DWendling(const ImageData& refImg3D, const ImageData& evalImg3D,
                                 const GammaParameters& gammaParams){
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    std::vector<float> gammaVals;
    gammaVals.reserve(refImg3D.size());

    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const float framesSpInv = 1 / evalImg3D.getSpacing().frames;
    const float rowsSpInv = 1 / evalImg3D.getSpacing().rows;
    const float columnsSpInv = 1 / evalImg3D.getSpacing().columns;

    const float zeMin = evalImg3D.getOffset().frames - Tolerance;
    const float yeMin = evalImg3D.getOffset().rows - Tolerance;
    const float xeMin = evalImg3D.getOffset().columns - Tolerance;
    const float zeMax = evalImg3D.getOffset().frames + (evalImg3D.getSize().frames - 1) * evalImg3D.getSpacing().frames + Tolerance;
    const float yeMax = evalImg3D.getOffset().rows + (evalImg3D.getSize().rows - 1) * evalImg3D.getSpacing().rows + Tolerance;
    const float xeMax = evalImg3D.getOffset().columns + (evalImg3D.getSize().columns - 1) * evalImg3D.getSpacing().columns + Tolerance;

    // TODO: check if interpolating evalImg on the grid of refImg
    // and precalculating interpolation factors (for on-the-fly interpolation) will be much faster.
    // note that result will be less accurate due to interpolating twice

    const auto sortedPoints = sortedPointsInSphere(gammaParams.maxSearchDistance, gammaParams.stepSize);

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
                    gammaVals.emplace_back(NaN);
                }
                else{
                    // set squared inversed normalized dd based on the type of normalization (global or local)
                    float ddNormInvSq = (isGlobal ? ddGlobalNormInvSq : (ddInvSq / (doseRef * doseRef)));

                    float minGammaValSq = Inf;

                    for(const auto& point : sortedPoints){
                        const float normalizedDistSq = point.distSq * dtaInvSq;
                        if(normalizedDistSq >= minGammaValSq){
                            break;
                        }

                        float ze = zr + point.z;
                        float ye = yr + point.y;
                        float xe = xr + point.x;

                        // instead of calling Interpolate::trilinearAtPoint function,
                        // here is an inlined, optimized version. It gives 5-10% speedup

                        if(ze < zeMin || ze > zeMax ||
                           ye < yeMin || ye > yeMax ||
                           xe < xeMin || xe > xeMax){
                            continue;
                        }

                        float tempz = (ze - evalImg3D.getOffset().frames) * framesSpInv;
                        float tempy = (ye - evalImg3D.getOffset().rows) * rowsSpInv;
                        float tempx = (xe - evalImg3D.getOffset().columns) * columnsSpInv;

                        const uint32_t indz0 = static_cast<uint32_t>(tempz);
                        const uint32_t indy0 = static_cast<uint32_t>(tempy);
                        const uint32_t indx0 = static_cast<uint32_t>(tempx);
                        uint32_t indz1 = indz0 + 1;
                        uint32_t indy1 = indy0 + 1;
                        uint32_t indx1 = indx0 + 1;

                        if(indz1 == evalImg3D.getSize().frames){
                            indz1 = indz0;
                        }
                        if(indy1 == evalImg3D.getSize().rows){
                            indy1 = indy0;
                        }
                        if(indx1 == evalImg3D.getSize().columns){
                            indx1 = indx0;
                        }

                        float zd = tempz - static_cast<float>(indz0);
                        float yd = tempy - static_cast<float>(indy0);
                        float xd = tempx - static_cast<float>(indx0);

                        float c000 = evalImg3D.get(indz0, indy0, indx0);
                        float c001 = evalImg3D.get(indz1, indy0, indx0);
                        float c010 = evalImg3D.get(indz0, indy1, indx0);
                        float c011 = evalImg3D.get(indz1, indy1, indx0);
                        float c100 = evalImg3D.get(indz0, indy0, indx1);
                        float c101 = evalImg3D.get(indz1, indy0, indx1);
                        float c110 = evalImg3D.get(indz0, indy1, indx1);
                        float c111 = evalImg3D.get(indz1, indy1, indx1);

                        float c00 = c000*(1 - xd) + c100*xd;
                        float c01 = c001*(1 - xd) + c101*xd;
                        float c10 = c010*(1 - xd) + c110*xd;
                        float c11 = c011*(1 - xd) + c111*xd;

                        float c0 = c00*(1 - yd) + c10*yd;
                        float c1 = c01*(1 - yd) + c11*yd;

                        float doseEval = c0*(1 - zd) + c1*zd;

                        // calculate squared gamma
                        float gammaValSq = distSq1D(doseEval, doseRef) * ddNormInvSq + normalizedDistSq;
                        if(gammaValSq < minGammaValSq){
                            minGammaValSq = gammaValSq;
                        }
                    }

                    if(minGammaValSq != Inf){
                        gammaVals.emplace_back(std::sqrt(minGammaValSq));
                    }
                    else{
                        gammaVals.emplace_back(NaN);
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
