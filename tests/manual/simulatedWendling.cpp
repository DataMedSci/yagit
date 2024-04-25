/********************************************************************************************
 * Copyright (C) 2024 'Yet Another Gamma Index Tool' Developers.
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

// This program simulates the Wendling method using the classic method with interpolation of the evaluated image.
// Useful for manual testing of the Wendling method.

// By default, the classic method doesn't restrict its calculations to a limited area,
// unlike Wendling, which employs the maxSearchDistance parameter.
// To use the maxSearchDistance parameter in the classic method,
// add the following code to the innermost loop of the classic method implementation.

// for 2D version
// if(distSq2D(xe[ie], ye[je], xr[ir], yr[jr]) > gammaParams.maxSearchDistance * gammaParams.maxSearchDistance){
//     indEval++;
//     continue;
// }

// for 2.5D and 3D versions
// if(distSq3D(xe[ie], ye[je], ze[ke], xr[ir], yr[jr], zr[kr]) > gammaParams.maxSearchDistance * gammaParams.maxSearchDistance){
//     indEval++;
//     continue;
// }

#include <yagit/yagit.hpp>
#include <functional>
#include <limits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>


const yagit::Image2D REF_IMAGE_2D = {
    {0.93, 0.95},
    {0.97, 1.00}
};
const yagit::Image2D EVAL_IMAGE_2D = {
    {0.93, 0.96},
    {0.90, 1.02}
};
const yagit::ImageData REF_2D(REF_IMAGE_2D, {0, 0, -1}, {2, 2, 2});
const yagit::ImageData EVAL_2D(EVAL_IMAGE_2D, {0, 1, 0}, {2, 2, 2});


const yagit::Image3D REF_IMAGE_3D = {
    {{0.2, 0.64, 0.3},
     {0.5, 0.43, 0.6}},
    {{0.4, 0.7, 0.28},
     {1.4, 0.8, 0.9}}
};
const yagit::Image3D EVAL_IMAGE_3D = {
    {{0.24, 0.68, 0.2},
     {0.67, 0.9, 0.6}},
    {{1.0, 0.8, 0.34},
     {0.8, 0.99, 0.83}}
};
const yagit::ImageData REF_3D(REF_IMAGE_3D, {-0.2, -5.8, 4.4}, {1.5, 2, 2.5});
const yagit::ImageData EVAL_3D(EVAL_IMAGE_3D, {-0.3, -6.0, 4.5}, {1.5, 2, 2.5});

// ====================================================================================================

void printImage(const yagit::ImageData& gammaRes){
    if(gammaRes.getSize().frames == 1){
        std::cout << yagit::image2DToString(gammaRes.getImage2D(0), 6) << "\n";
    }
    else{
        std::cout << yagit::image3DToString(gammaRes.getImage3D(), 6) << "\n";
    }
}

void printImageDataInfo(const yagit::ImageData& imageData){
    yagit::DataOffset offset = imageData.getOffset();
    yagit::DataSpacing spacing = imageData.getSpacing();
    std::cout << "offset: (" << offset.frames << ", " << offset.rows << ", " << offset.columns << "),"
              << "  spacing: (" << spacing.frames << ", " << spacing.rows << ", " << spacing.columns << ")\n";
}

// ====================================================================================================

yagit::GammaResult wendlingUsingClassic2D(const yagit::ImageData& refImg, const yagit::ImageData& evalImg,
                                          const yagit::GammaParameters& gammaParams){
    const float newSpacing = gammaParams.stepSize;

    yagit::Image3D values(refImg.getSize().frames,
                          yagit::Image2D(refImg.getSize().rows,
                                         std::vector<float>(refImg.getSize().columns, 0.0f)));

    for(uint32_t k = 0; k < refImg.getSize().frames; k++){
        for(uint32_t j = 0; j < refImg.getSize().rows; j++){
            for(uint32_t i = 0; i < refImg.getSize().columns; i++){
                float posY = refImg.getOffset().rows + j * refImg.getSpacing().rows;
                float posX = refImg.getOffset().columns + i * refImg.getSpacing().columns;

                yagit::ImageData refImgOneEl({refImg.get(k, j, i)}, {1, 1, 1},
                                             {refImg.getOffset().frames, posY, posX}, refImg.getSpacing());

                auto interpEval = yagit::Interpolation::bilinearOnPlane(evalImg,
                                                                        posY, posX, newSpacing, newSpacing,
                                                                        yagit::ImagePlane::Axial);

                values[k][j][i] = yagit::gammaIndex2DClassic(refImgOneEl, interpEval, gammaParams).get(0);

                // printImageDataInfo(interpEval);
            }
        }
    }

    return {values, refImg.getOffset(), refImg.getSpacing()};
}

yagit::GammaResult wendlingUsingClassic2_5D(const yagit::ImageData& refImg, const yagit::ImageData& evalImg,
                                            const yagit::GammaParameters& gammaParams){
    const float newSpacing = gammaParams.stepSize;

    yagit::ImageData evalImg2 = yagit::Interpolation::linearAlongAxis(evalImg, refImg, yagit::ImageAxis::Z);
    const int kDiff = static_cast<int>((refImg.getOffset().frames - evalImg2.getOffset().frames) / refImg.getSpacing().frames);    

    yagit::Image3D values(refImg.getSize().frames,
                          yagit::Image2D(refImg.getSize().rows,
                                         std::vector<float>(refImg.getSize().columns, 0.0f)));

    for(uint32_t k = 0; k < refImg.getSize().frames; k++){
        for(uint32_t j = 0; j < refImg.getSize().rows; j++){
            for(uint32_t i = 0; i < refImg.getSize().columns; i++){
                int ke = static_cast<int>(k) + kDiff;
                if(ke < 0 || ke >= static_cast<int>(evalImg2.getSize().frames)){
                    values[k][j][i] = std::numeric_limits<float>::quiet_NaN();
                    continue;
                }

                float posZ = refImg.getOffset().frames + k * refImg.getSpacing().frames;
                float posY = refImg.getOffset().rows + j * refImg.getSpacing().rows;
                float posX = refImg.getOffset().columns + i * refImg.getSpacing().columns;

                yagit::ImageData refImgOneEl({refImg.get(k, j, i)}, {1, 1, 1},
                                             {posZ, posY, posX}, refImg.getSpacing());
 
                auto interpEval2D = yagit::Interpolation::bilinearOnPlane(evalImg2.getImageData2D(ke),
                                                                          posY, posX, newSpacing, newSpacing,
                                                                          yagit::ImagePlane::Axial);

                values[k][j][i] = yagit::gammaIndex2DClassic(refImgOneEl, interpEval2D, gammaParams).get(0);

                // printImageDataInfo(interpEval2D);
            }
        }
    }

    return {values, refImg.getOffset(), refImg.getSpacing()};
}

yagit::GammaResult wendlingUsingClassic3D(const yagit::ImageData& refImg, const yagit::ImageData& evalImg,
                                          const yagit::GammaParameters& gammaParams){
    const float newSpacing = gammaParams.stepSize;

    yagit::Image3D values(refImg.getSize().frames,
                          yagit::Image2D(refImg.getSize().rows,
                                         std::vector<float>(refImg.getSize().columns, 0.0f)));

    for(uint32_t k = 0; k < refImg.getSize().frames; k++){
        for(uint32_t j = 0; j < refImg.getSize().rows; j++){
            for(uint32_t i = 0; i < refImg.getSize().columns; i++){
                float posZ = refImg.getOffset().frames + k * refImg.getSpacing().frames;
                float posY = refImg.getOffset().rows + j * refImg.getSpacing().rows;
                float posX = refImg.getOffset().columns + i * refImg.getSpacing().columns;

                yagit::ImageData refImgOneEl({refImg.get(k, j, i)}, {1, 1, 1},
                                             {posZ, posY, posX}, refImg.getSpacing());

                auto interpEval = yagit::Interpolation::trilinear(evalImg,
                                                                  {posZ, posY, posX},
                                                                  {newSpacing, newSpacing, newSpacing});

                values[k][j][i] = yagit::gammaIndex3DClassic(refImgOneEl, interpEval, gammaParams).get(0);

                // printImageDataInfo(interpEval);
            }
        }
    }

    return {values, refImg.getOffset(), refImg.getSpacing()};
}

// ====================================================================================================

double absDiff(const yagit::ImageData& img1, const yagit::ImageData& img2){
    assert(img1.size() == img2.size());

    double absDiff = 0.0f;
    for(size_t i = 0; i < img1.size(); i++){
        const double c = img1.get(i);
        const double w = img2.get(i);
        if(c != w && !(std::isnan(c) && std::isnan(w))){
            absDiff += std::abs(c - w);
        }
    }
    return absDiff;
}

using GammaFunc = std::function<yagit::GammaResult(const yagit::ImageData& refImg, const yagit::ImageData& evalImg,
                                                   const yagit::GammaParameters& gammaParams)>;

void wendling(std::string dim,
              GammaFunc classicFunc, GammaFunc wendlingFunc,
              const yagit::ImageData& refImg, const yagit::ImageData& evalImg,
              const yagit::GammaParameters& gammaParams){
    std::cout << dim << " #############################################\n";

    std::cout << "SIMULATED WENDLING METHOD:\n";
    auto resClassic = classicFunc(refImg, evalImg, gammaParams);
    printImage(resClassic);

    std::cout << "==============================\n";
    std::cout << "REAL WENDLING METHOD:\n";
    auto resWendling = wendlingFunc(refImg, evalImg, gammaParams);
    printImage(resWendling);

    if(resClassic == resWendling){
        std::cout << "\nIDENTICAL IMAGES\n";
    }
    else{
        std::cout << "\nDIFFERENT IMAGES (abs diff = " << absDiff(resClassic, resWendling) << ")\n";
    }
}

void wendling2D(const yagit::ImageData& refImg, const yagit::ImageData& evalImg,
                const yagit::GammaParameters& gammaParams){
    wendling("2D", wendlingUsingClassic2D, yagit::gammaIndex2DWendling, refImg, evalImg, gammaParams);
}

void wendling2_5D(const yagit::ImageData& refImg, const yagit::ImageData& evalImg,
                  const yagit::GammaParameters& gammaParams){
    wendling("2.5D", wendlingUsingClassic2_5D, yagit::gammaIndex2_5DWendling, refImg, evalImg, gammaParams);
}

void wendling3D(const yagit::ImageData& refImg, const yagit::ImageData& evalImg,
                const yagit::GammaParameters& gammaParams){
    wendling("3D", wendlingUsingClassic3D, yagit::gammaIndex3DWendling, refImg, evalImg, gammaParams);
}

// ====================================================================================================

const float MAX_REF_DOSE = -1;  // set automatically max reference dose

int main(){
    yagit::GammaParameters gammaParams{3, 3, yagit::GammaNormalization::Global, MAX_REF_DOSE, 0, 10, 0.3};

    try{
        // 2D
        if(gammaParams.globalNormDose == MAX_REF_DOSE){
            gammaParams.globalNormDose = REF_2D.max();
        }
        wendling2D(REF_2D, EVAL_2D, gammaParams);

        // 2.5D
        // if(gammaParams.globalNormDose == MAX_REF_DOSE){
        //     gammaParams.globalNormDose = REF_3D.max();
        // }
        // wendling2_5D(REF_3D, EVAL_3D, gammaParams);

        // 3D
        // if(gammaParams.globalNormDose == MAX_REF_DOSE){
        //     gammaParams.globalNormDose = REF_3D.max();
        // }
        // wendling3D(REF_3D, EVAL_3D, gammaParams);
    }
    catch(const std::exception &e){
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}
