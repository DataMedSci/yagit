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

#include <string>
#include <limits>
#include <cmath>

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
        throw std::invalid_argument("DD threshold is not positive (dd <= 0)");
    }
    if(gammaParams.dtaThreshold <= 0){
        throw std::invalid_argument("DTA threshold is not positive (dta <= 0)");
    }
    if(gammaParams.normalization == yagit::GammaNormalization::Global && gammaParams.globalNormDose <= 0){
        throw std::invalid_argument("global normalization dose is not positive (global normalization dose <= 0)");
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

}
