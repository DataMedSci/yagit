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
/**
 * @file
 * @brief This file provides a simple example of using yagit - 2D gamma index for any image.
 * 
 * @example{lineno}
 * This file provides a simple example of using yagit - 2D gamma index for any image.
 * - First, it creates two 2D images - reference and evaluated.
 * - Then it calculates 3%G/3mm 2D gamma index of those 2 images using classic method.
 * Global normalization dose is set to maximum value of reference image.
 * - At the end it prints gamma index passing rate and other info.
 */

#include <string>
#include <iostream>

#include <yagit/yagit.hpp>

void printImage2D(const yagit::Image2D& img){
    std::cout << "[";
    for(size_t i = 0; i < img.size(); i++){
        std::cout << (i == 0 ? "[" : " [");
        for(size_t j = 0; j < img[i].size() - 1; j++){
            std::cout << img[i][j] << ", ";
        }
        std::cout << img[i].back() << "]";
        if(i < img.size() - 1){
            std::cout << ",\n";
        }
    }
    std::cout << "]\n";
}

void printImageData(const yagit::ImageData& imageData){
    printImage2D(imageData.getImage2D(0));

    yagit::DataSize size = imageData.getSize();
    yagit::DataOffset offset = imageData.getOffset();
    yagit::DataSpacing spacing = imageData.getSpacing();
    std::cout << "size: (" << size.frames << ", " << size.rows << ", " << size.columns << ")\n"
              << "offset: (" << offset.frames << ", " << offset.rows << ", " << offset.columns << ")\n"
              << "spacing: (" << spacing.frames << ", " << spacing.rows << ", " << spacing.columns << ")\n";
}

int main(){

    yagit::Image2D refImg = {
        {0.93, 0.95},
        {0.97, 1.00}
    };
    yagit::Image2D evalImg = {
        {0.95, 0.97},
        {1.00, 1.03}
    };

    yagit::DataOffset refOffset{0, 0, -1};
    yagit::DataOffset evalOffset{0, -1, 0};
    yagit::DataSpacing refSpacing{0, 1, 1};
    yagit::DataSpacing evalSpacing{0, 1, 1};

    yagit::ImageData refImgDose(refImg, refOffset, refSpacing);
    yagit::ImageData evalImgDose(evalImg, evalOffset, evalSpacing);

    std::cout << "Reference image:\n";
    printImageData(refImgDose);
    std::cout << "------------------------------\n";
    std::cout << "Evaluated image:\n";
    printImageData(evalImgDose);
    std::cout << "------------------------------\n";

    float refMaxDose = refImgDose.max();
    yagit::GammaParameters gammaParams;
    gammaParams.ddThreshold = 3.0;   // [%]
    gammaParams.dtaThreshold = 3.0;  // [mm]
    gammaParams.normalization = yagit::GammaNormalization::Global;
    gammaParams.globalNormDose = refMaxDose;
    gammaParams.doseCutoff = 0;

    yagit::GammaResult gammaRes = yagit::gammaIndex2DClassic(refImgDose, evalImgDose, gammaParams);

    std::cout << "Gamma index image:\n";
    // expected:
    // [[0.816496, 0.333333],
    //  [0.942809, 0.333333]]
    printImage2D(gammaRes.getImage2D(0));

    std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n"
              << "Gamma mean: " << gammaRes.meanGamma() << "\n"
              << "Gamma min: " << gammaRes.minGamma() << "\n"
              << "Gamma max: " << gammaRes.maxGamma() << "\n";
}
