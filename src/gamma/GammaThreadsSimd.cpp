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

#include <thread>
#include <tuple>
#include <cmath>
#include <algorithm>
#include <functional>

#include "yagit/Interpolation.hpp"
#include "GammaCommonSimd.hpp"
#include "GammaThreadsUtils.hpp"

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

namespace{
void gammaIndex2DClassicInternal(const ImageData& refImg2D, const ImageData& evalImg2D,
                                 const GammaParameters& gammaParams,
                                 const std::vector<float>& yr, const std::vector<float>& xr,
                                 const std::vector<float>& ye, const aligned_vector<float>& xe,
                                 size_t startIndex, size_t endIndex, std::vector<float>& gammaVals){
    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const size_t evalSimdSize = evalImg2D.getSize().columns - evalImg2D.getSize().columns % SimdElementCount;
    const xsimd::batch<float> dtaInvSqVec(dtaInvSq);

    const auto [jStart, iStart] = indexTo2Dindex(startIndex, refImg2D.getSize());

    // iterate over each row and column of reference image
    size_t indRef = startIndex;
    for(uint32_t jr = jStart; jr < refImg2D.getSize().rows && indRef < endIndex; jr++){
        xsimd::batch<float> yrVec(yr[jr]);
        const uint32_t iStart2 = (jr != jStart ? 0 : iStart);

        for(uint32_t ir = iStart2; ir < refImg2D.getSize().columns && indRef < endIndex; ir++){
            if(gammaVals[indRef] == Inf){
                float doseRef = refImg2D.get(indRef);
                xsimd::batch<float> doseRefVec(doseRef);

                // set squared inversed normalized dd based on the type of normalization (global or local)
                float ddNormInvSq = (isGlobal ? ddGlobalNormInvSq : (ddInvSq / (doseRef * doseRef)));
                xsimd::batch<float> ddNormInvSqVec(ddNormInvSq);

                float minGammaValSq = Inf;
                xsimd::batch<float> minGammaValSqVec(Inf);

                xsimd::batch<float> xrVec(xr[ir]);

                // iterate over each row and column of evaluated image
                size_t indEval = 0;
                for(uint32_t je = 0; je < evalImg2D.getSize().rows; je++){
                    xsimd::batch<float> yeVec(ye[je]);

                    uint32_t ie = 0;
                    for(; ie < evalSimdSize; ie += SimdElementCount){
                        auto doseEvalVec = xsimd::load_unaligned(&evalImg2D.get(indEval));
                        auto xeVec = xsimd::load_aligned(&xe[ie]);

                        // calculate squared gamma
                        // not using distSq1D and distSq2D functions, because this inlined version on simd vectors is faster
                        auto gammaValSqVec = (doseRefVec - doseEvalVec) * (doseRefVec - doseEvalVec) * ddNormInvSqVec +
                                             ((xrVec - xeVec) * (xrVec - xeVec) + (yrVec - yeVec) * (yrVec - yeVec)) * dtaInvSqVec;

                        minGammaValSqVec = xsimd::min(gammaValSqVec, minGammaValSqVec);

                        indEval += SimdElementCount;
                    }
                    for(; ie < evalImg2D.getSize().columns; ie++){
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

                float minGammaValSqVecMin = xsimd::reduce_min(minGammaValSqVec);
                if(minGammaValSqVecMin < minGammaValSq){
                    minGammaValSq = minGammaValSqVecMin;
                }
                gammaVals[indRef] = std::sqrt(minGammaValSq);
            }

            indRef++;
        }
    }
}

void gammaIndex2_5DClassicInternal(const ImageData& refImg3D, const ImageData& evalImg3D,
                                   const GammaParameters& gammaParams,
                                   const std::vector<float>& zr, const std::vector<float>& yr,
                                   const std::vector<float>& xr, const std::vector<float>& ze,
                                   const std::vector<float>& ye, const aligned_vector<float>& xe,
                                   size_t startIndex, size_t endIndex, std::vector<float>& gammaVals){
    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const size_t evalSimdSize = evalImg3D.getSize().columns - evalImg3D.getSize().columns % SimdElementCount;
    const xsimd::batch<float> dtaInvSqVec(dtaInvSq);

    const auto [kStart, jStart, iStart] = indexTo3Dindex(startIndex, refImg3D.getSize());

    // iterate over each frame, row and column of reference image
    size_t indRef = startIndex;
    for(uint32_t kr = kStart; kr < refImg3D.getSize().frames && indRef < endIndex; kr++){
        xsimd::batch<float> zrVec(zr[kr]);
        xsimd::batch<float> zeVec(ze[kr]);

        const uint32_t jStart2 = (kr != kStart ? 0 : jStart);
        for(uint32_t jr = jStart2; jr < refImg3D.getSize().rows && indRef < endIndex; jr++){
            xsimd::batch<float> yrVec(yr[jr]);

            const uint32_t iStart2 = (kr != kStart || jr != jStart ? 0 : iStart);
            for(uint32_t ir = iStart2; ir < refImg3D.getSize().columns && indRef < endIndex; ir++){
                if(gammaVals[indRef] == Inf){
                    float doseRef = refImg3D.get(indRef);
                    xsimd::batch<float> doseRefVec(doseRef);

                    // set squared inversed normalized dd based on the type of normalization (global or local)
                    float ddNormInvSq = (isGlobal ? ddGlobalNormInvSq : (ddInvSq / (doseRef * doseRef)));
                    xsimd::batch<float> ddNormInvSqVec(ddNormInvSq);

                    float minGammaValSq = Inf;
                    xsimd::batch<float> minGammaValSqVec(Inf);

                    xsimd::batch<float> xrVec(xr[ir]);

                    // iterate over each row and column of evaluated image
                    size_t indEval = kr * evalImg3D.getSize().rows * refImg3D.getSize().columns;
                    for(uint32_t je = 0; je < evalImg3D.getSize().rows; je++){
                        xsimd::batch<float> yeVec(ye[je]);

                        uint32_t ie = 0;
                        for(; ie < evalSimdSize; ie += SimdElementCount){
                            auto doseEvalVec = xsimd::load_unaligned(&evalImg3D.get(indEval));
                            auto xeVec = xsimd::load_aligned(&xe[ie]);

                            // calculate squared gamma
                            // not using distSq1D and distSq2D functions, because this inlined version on simd vectors is faster
                            auto gammaValSqVec = (doseRefVec - doseEvalVec) * (doseRefVec - doseEvalVec) * ddNormInvSqVec +
                                                 ((xrVec - xeVec) * (xrVec - xeVec) + (yrVec - yeVec) * (yrVec - yeVec) + (zrVec - zeVec) * (zrVec - zeVec)) * dtaInvSqVec;

                            minGammaValSqVec = xsimd::min(gammaValSqVec, minGammaValSqVec);

                            indEval += SimdElementCount;
                        }
                        for(; ie < evalImg3D.getSize().columns; ie++){
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

                    float minGammaValSqVecMin = xsimd::reduce_min(minGammaValSqVec);
                    if(minGammaValSqVecMin < minGammaValSq){
                        minGammaValSq = minGammaValSqVecMin;
                    }
                    gammaVals[indRef] = std::sqrt(minGammaValSq);
                }

                indRef++;
            }
        }
    }
}

void gammaIndex3DClassicInternal(const ImageData& refImg3D, const ImageData& evalImg3D,
                                 const GammaParameters& gammaParams,
                                 const std::vector<float>& zr, const std::vector<float>& yr,
                                 const std::vector<float>& xr, const std::vector<float>& ze,
                                 const std::vector<float>& ye, const aligned_vector<float>& xe,
                                 size_t startIndex, size_t endIndex, std::vector<float>& gammaVals){
    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const size_t evalSimdSize = evalImg3D.getSize().columns - evalImg3D.getSize().columns % SimdElementCount;
    const xsimd::batch<float> dtaInvSqVec(dtaInvSq);

    const auto [kStart, jStart, iStart] = indexTo3Dindex(startIndex, refImg3D.getSize());

    // iterate over each frame, row and column of reference image
    size_t indRef = startIndex;
    for(uint32_t kr = kStart; kr < refImg3D.getSize().frames && indRef < endIndex; kr++){
        xsimd::batch<float> zrVec(zr[kr]);

        const uint32_t jStart2 = (kr != kStart ? 0 : jStart);
        for(uint32_t jr = jStart2; jr < refImg3D.getSize().rows && indRef < endIndex; jr++){
            xsimd::batch<float> yrVec(yr[jr]);

            const uint32_t iStart2 = (kr != kStart || jr != jStart ? 0 : iStart);
            for(uint32_t ir = iStart2; ir < refImg3D.getSize().columns && indRef < endIndex; ir++){
                if(gammaVals[indRef] == Inf){
                    float doseRef = refImg3D.get(indRef);
                    xsimd::batch<float> doseRefVec(doseRef);

                    // set squared inversed normalized dd based on the type of normalization (global or local)
                    float ddNormInvSq = (isGlobal ? ddGlobalNormInvSq : (ddInvSq / (doseRef * doseRef)));
                    xsimd::batch<float> ddNormInvSqVec(ddNormInvSq);

                    float minGammaValSq = Inf;
                    xsimd::batch<float> minGammaValSqVec(Inf);

                    xsimd::batch<float> xrVec(xr[ir]);

                    // iterate over each frame, row and column of evaluated image
                    size_t indEval = 0;
                    for(uint32_t ke = 0; ke < evalImg3D.getSize().frames; ke++){
                        xsimd::batch<float> zeVec(ze[ke]);

                        for(uint32_t je = 0; je < evalImg3D.getSize().rows; je++){
                            xsimd::batch<float> yeVec(ye[je]);

                            uint32_t ie = 0;
                            for(; ie < evalSimdSize; ie += SimdElementCount){
                                auto doseEvalVec = xsimd::load_unaligned(&evalImg3D.get(indEval));
                                auto xeVec = xsimd::load_aligned(&xe[ie]);

                                // calculate squared gamma
                                // not using distSq1D and distSq2D functions, because this inlined version on simd vectors is faster
                                auto gammaValSqVec = (doseRefVec - doseEvalVec) * (doseRefVec - doseEvalVec) * ddNormInvSqVec +
                                                     ((xrVec - xeVec) * (xrVec - xeVec) + (yrVec - yeVec) * (yrVec - yeVec) + (zrVec - zeVec) * (zrVec - zeVec)) * dtaInvSqVec;

                                minGammaValSqVec = xsimd::min(gammaValSqVec, minGammaValSqVec);

                                indEval += SimdElementCount;
                            }
                            for(; ie < evalImg3D.getSize().columns; ie++){
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

                    float minGammaValSqVecMin = xsimd::reduce_min(minGammaValSqVec);
                    if(minGammaValSqVecMin < minGammaValSq){
                        minGammaValSq = minGammaValSqVecMin;
                    }
                    gammaVals[indRef] = std::sqrt(minGammaValSq);
                }

                indRef++;
            }
        }
    }
}
}

GammaResult gammaIndex2DClassic(const ImageData& refImg2D, const ImageData& evalImg2D,
                                const GammaParameters& gammaParams){
    validateImages2D(refImg2D, evalImg2D);
    validateGammaParameters(gammaParams);

    const std::vector<float> yr = generateCoordinates(refImg2D, ImageAxis::Y);
    const std::vector<float> xr = generateCoordinates(refImg2D, ImageAxis::X);
    const std::vector<float> ye = generateCoordinates(evalImg2D, ImageAxis::Y);
    const aligned_vector<float> xe = generateCoordinatesAligned(evalImg2D, ImageAxis::X);

    std::vector<float> gammaVals =
        multithreadedGammaIndex(refImg2D, gammaParams, gammaIndex2DClassicInternal,
                                std::cref(refImg2D), std::cref(evalImg2D), std::cref(gammaParams),
                                std::cref(yr), std::cref(xr),
                                std::cref(ye), std::cref(xe));

    return GammaResult(std::move(gammaVals), refImg2D.getSize(), refImg2D.getOffset(), refImg2D.getSpacing());
}

GammaResult gammaIndex2_5DClassic(const ImageData& refImg3D, const ImageData& evalImg3D,
                                  const GammaParameters& gammaParams){
    if(evalImg3D.getSize().frames < refImg3D.getSize().frames){
        throw std::invalid_argument("evaluated image must have at least the same number of frames as the reference image");
    }
    validateGammaParameters(gammaParams);

    const std::vector<float> zr = generateCoordinates(refImg3D, ImageAxis::Z);
    const std::vector<float> yr = generateCoordinates(refImg3D, ImageAxis::Y);
    const std::vector<float> xr = generateCoordinates(refImg3D, ImageAxis::X);
    const std::vector<float> ze = generateCoordinates(evalImg3D, ImageAxis::Z);
    const std::vector<float> ye = generateCoordinates(evalImg3D, ImageAxis::Y);
    const aligned_vector<float> xe = generateCoordinatesAligned(evalImg3D, ImageAxis::X);

    std::vector<float> gammaVals =
        multithreadedGammaIndex(refImg3D, gammaParams, gammaIndex2_5DClassicInternal,
                                std::cref(refImg3D), std::cref(evalImg3D), std::cref(gammaParams),
                                std::cref(zr), std::cref(yr), std::cref(xr),
                                std::cref(ze), std::cref(ye), std::cref(xe));

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

GammaResult gammaIndex3DClassic(const ImageData& refImg3D, const ImageData& evalImg3D,
                                const GammaParameters& gammaParams){
    validateGammaParameters(gammaParams);

    const std::vector<float> zr = generateCoordinates(refImg3D, ImageAxis::Z);
    const std::vector<float> yr = generateCoordinates(refImg3D, ImageAxis::Y);
    const std::vector<float> xr = generateCoordinates(refImg3D, ImageAxis::X);
    const std::vector<float> ze = generateCoordinates(evalImg3D, ImageAxis::Z);
    const std::vector<float> ye = generateCoordinates(evalImg3D, ImageAxis::Y);
    const aligned_vector<float> xe = generateCoordinatesAligned(evalImg3D, ImageAxis::X);

    std::vector<float> gammaVals =
        multithreadedGammaIndex(refImg3D, gammaParams, gammaIndex3DClassicInternal,
                                std::cref(refImg3D), std::cref(evalImg3D), std::cref(gammaParams),
                                std::cref(zr), std::cref(yr), std::cref(xr),
                                std::cref(ze), std::cref(ye), std::cref(xe));

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

// Wendling method of gamma index is not vectorized, because there were two unsuccessful attempts which worked
// worse than sequential version (in some cases it was even several times slower).
// The first attempt was to vectorize loop that iterates over sorted points. It turned out that the problem with its
// performance is stopping condition of loop that in vectorized version was met later than in sequential version
// (for example sequential version met stopping condition after 1 point, but vectorized version in the same case
// must process several points).
// The second attempt was to vectorize only interpolation after evaluation of doses values at adjacent 4/8 points.
// There were used two methods for calculating this optimally with vectorization (1. horizontall add,
// 2. calculations on low and high halves of vector), but it turned out to be slower than sequential version.

namespace{
void gammaIndex2DWendlingInternal(const ImageData& refImg2D, const ImageData& evalImg2D,
                                  const GammaParameters& gammaParams,
                                  const std::vector<Point2D>& sortedPoints,
                                  size_t startIndex, size_t endIndex, std::vector<float>& gammaVals){
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

    const auto [jStart, iStart] = indexTo2Dindex(startIndex, refImg2D.getSize());

    // iterate over each row and column of reference image
    size_t indRef = startIndex;
    float yr = refImg2D.getOffset().rows + jStart * refImg2D.getSpacing().rows;
    for(uint32_t jr = jStart; jr < refImg2D.getSize().rows && indRef < endIndex; jr++){
        const uint32_t iStart2 = (jr != jStart ? 0 : iStart);
        float xr = refImg2D.getOffset().columns + iStart2 * refImg2D.getSpacing().columns;

        for(uint32_t ir = iStart2; ir < refImg2D.getSize().columns && indRef < endIndex; ir++){
            float doseRef = refImg2D.get(indRef);

            bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
            bool divisionByZero = !isGlobal && doseRef == 0;
            if(doseBelowCutoff || divisionByZero){
                gammaVals[indRef] = NaN;
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
                    gammaVals[indRef] = std::sqrt(minGammaValSq);
                }
                else{
                    gammaVals[indRef] = NaN;
                }
            }
            xr += refImg2D.getSpacing().columns;
            indRef++;
        }
        yr += refImg2D.getSpacing().rows;
    }
}

void gammaIndex2_5DWendlingInternal(const ImageData& refImg3D, const ImageData& evalImg3D,
                                    const GammaParameters& gammaParams,
                                    const std::vector<Point2D>& sortedPoints,
                                    size_t startIndex, size_t endIndex, std::vector<float>& gammaVals){
    const float ddInvSq = (100 * 100) / (gammaParams.ddThreshold * gammaParams.ddThreshold);
    const float dtaInvSq = 1 / (gammaParams.dtaThreshold * gammaParams.dtaThreshold);
    const float ddGlobalNormInvSq = ddInvSq / (gammaParams.globalNormDose * gammaParams.globalNormDose);

    const bool isGlobal = gammaParams.normalization == GammaNormalization::Global;

    const float rowsSpInv = 1 / evalImg3D.getSpacing().rows;
    const float columnsSpInv = 1 / evalImg3D.getSpacing().columns;

    const float yeMin = evalImg3D.getOffset().rows - Tolerance;
    const float xeMin = evalImg3D.getOffset().columns - Tolerance;
    const float yeMax = evalImg3D.getOffset().rows + (evalImg3D.getSize().rows - 1) * evalImg3D.getSpacing().rows + Tolerance;
    const float xeMax = evalImg3D.getOffset().columns + (evalImg3D.getSize().columns - 1) * evalImg3D.getSpacing().columns + Tolerance;

    const int kDiff = static_cast<int>((refImg3D.getOffset().frames - evalImg3D.getOffset().frames) / refImg3D.getSpacing().frames);

    const auto [kStart, jStart, iStart] = indexTo3Dindex(startIndex, refImg3D.getSize());

    // iterate over each frame, row and column of reference image
    size_t indRef = startIndex;
    int ke = kStart + kDiff;
    for(uint32_t kr = kStart; kr < refImg3D.getSize().frames && indRef < endIndex; kr++){
        const uint32_t jStart2 = (kr != kStart ? 0 : jStart);
        float yr = refImg3D.getOffset().rows + jStart2 * refImg3D.getSpacing().rows;

        for(uint32_t jr = jStart2; jr < refImg3D.getSize().rows && indRef < endIndex; jr++){

            const uint32_t iStart2 = (kr != kStart || jr != jStart ? 0 : iStart);
            float xr = refImg3D.getOffset().columns + iStart2 * refImg3D.getSpacing().columns;

            for(uint32_t ir = iStart2; ir < refImg3D.getSize().columns && indRef < endIndex; ir++){
                float doseRef = refImg3D.get(indRef);

                bool evalFrameOutsideImage = ke < 0 || ke >= static_cast<int>(evalImg3D.getSize().frames);
                bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
                bool divisionByZero = !isGlobal && doseRef == 0;
                if(evalFrameOutsideImage || doseBelowCutoff || divisionByZero){
                    gammaVals[indRef] = NaN;
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

                        float tempy = (ye - evalImg3D.getOffset().rows) * rowsSpInv;
                        float tempx = (xe - evalImg3D.getOffset().columns) * columnsSpInv;

                        const uint32_t indy0 = static_cast<uint32_t>(tempy);
                        const uint32_t indx0 = static_cast<uint32_t>(tempx);
                        uint32_t indy1 = indy0 + 1;
                        uint32_t indx1 = indx0 + 1;

                        if(indy1 == evalImg3D.getSize().rows){
                            indy1 = indy0;
                        }
                        if(indx1 == evalImg3D.getSize().columns){
                            indx1 = indx0;
                        }

                        float yd = tempy - static_cast<float>(indy0);
                        float xd = tempx - static_cast<float>(indx0);

                        float c00 = evalImg3D.get(ke, indy0, indx0);
                        float c01 = evalImg3D.get(ke, indy1, indx0);
                        float c10 = evalImg3D.get(ke, indy0, indx1);
                        float c11 = evalImg3D.get(ke, indy1, indx1);

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
                        gammaVals[indRef] = std::sqrt(minGammaValSq);
                    }
                    else{
                        gammaVals[indRef] = NaN;
                    }
                }
                xr += refImg3D.getSpacing().columns;
                indRef++;
            }
            yr += refImg3D.getSpacing().rows;
        }
        ke++;
    }
}

void gammaIndex3DWendlingInternal(const ImageData& refImg3D, const ImageData& evalImg3D,
                                  const GammaParameters& gammaParams,
                                  const std::vector<Point3D>& sortedPoints,
                                  size_t startIndex, size_t endIndex, std::vector<float>& gammaVals){
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

    const auto [kStart, jStart, iStart] = indexTo3Dindex(startIndex, refImg3D.getSize());

    // iterate over each frame, row and column of reference image
    size_t indRef = startIndex;
    float zr = refImg3D.getOffset().frames + kStart * refImg3D.getSpacing().frames;
    for(uint32_t kr = kStart; kr < refImg3D.getSize().frames && indRef < endIndex; kr++){

        const uint32_t jStart2 = (kr != kStart ? 0 : jStart);
        float yr = refImg3D.getOffset().rows + jStart2 * refImg3D.getSpacing().rows;

        for(uint32_t jr = jStart2; jr < refImg3D.getSize().rows && indRef < endIndex; jr++){

            const uint32_t iStart2 = (kr != kStart || jr != jStart ? 0 : iStart);
            float xr = refImg3D.getOffset().columns + iStart2 * refImg3D.getSpacing().columns;

            for(uint32_t ir = iStart2; ir < refImg3D.getSize().columns && indRef < endIndex; ir++){
                float doseRef = refImg3D.get(indRef);

                bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
                bool divisionByZero = !isGlobal && doseRef == 0;
                if(doseBelowCutoff || divisionByZero){
                    gammaVals[indRef] = NaN;
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
                        gammaVals[indRef] = std::sqrt(minGammaValSq);
                    }
                    else{
                        gammaVals[indRef] = NaN;
                    }
                }
                xr += refImg3D.getSpacing().columns;
                indRef++;
            }
            yr += refImg3D.getSpacing().rows;
        }
        zr += refImg3D.getSpacing().frames;
    }
}
}

GammaResult gammaIndex2DWendling(const ImageData& refImg2D, const ImageData& evalImg2D,
                                 const GammaParameters& gammaParams){
    validateImages2D(refImg2D, evalImg2D);
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    const auto sortedPoints = sortedPointsInCircle(gammaParams.maxSearchDistance, gammaParams.stepSize);

    std::vector<float> gammaVals =
        loadBalancingMultithreadedGammaIndex(refImg2D.size(), gammaIndex2DWendlingInternal,
                                             std::cref(refImg2D), std::cref(evalImg2D),
                                             std::cref(gammaParams), std::cref(sortedPoints));

    return GammaResult(std::move(gammaVals), refImg2D.getSize(), refImg2D.getOffset(), refImg2D.getSpacing());
}

GammaResult gammaIndex2_5DWendling(const ImageData& refImg3D, const ImageData& evalImg3D,
                                   const GammaParameters& gammaParams){
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    const ImageData evalImgInterpolatedZ = Interpolation::linearAlongAxis(evalImg3D, refImg3D, ImageAxis::Z);
    const auto sortedPoints = sortedPointsInCircle(gammaParams.maxSearchDistance, gammaParams.stepSize);

    std::vector<float> gammaVals =
        loadBalancingMultithreadedGammaIndex(refImg3D.size(), gammaIndex2_5DWendlingInternal,
                                             std::cref(refImg3D), std::cref(evalImgInterpolatedZ),
                                             std::cref(gammaParams), std::cref(sortedPoints));

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

GammaResult gammaIndex3DWendling(const ImageData& refImg3D, const ImageData& evalImg3D,
                                 const GammaParameters& gammaParams){
    validateGammaParameters(gammaParams);
    validateWendlingGammaParameters(gammaParams);

    const auto sortedPoints = sortedPointsInSphere(gammaParams.maxSearchDistance, gammaParams.stepSize);

    std::vector<float> gammaVals =
        loadBalancingMultithreadedGammaIndex(refImg3D.size(), gammaIndex3DWendlingInternal,
                                             std::cref(refImg3D), std::cref(evalImg3D),
                                             std::cref(gammaParams), std::cref(sortedPoints));

    return GammaResult(std::move(gammaVals), refImg3D.getSize(), refImg3D.getOffset(), refImg3D.getSpacing());
}

}
