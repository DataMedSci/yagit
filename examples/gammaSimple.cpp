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
/**
 * @file
 * @brief This file provides a simple example of using yagit - 2D gamma index for any images.
 * 
 * @example{lineno}
 * Example demonstrating the 2D gamma index for any images.
 * 1. Create two 2D images - a reference image and an evaluated image.
 * 2. Calculate 2D gamma index of those two images using classic method.
 *    The parameters are: 3%G/3mm,
 *    normalization dose is set to max value of the reference image,
 *    and dose cutoff is set to 10% of max value of the reference image.
 * 3. Print gamma index image, gamma index passing rate and other statistics.
 */

#include <string>
#include <iostream>
#include <sstream>

#include <yagit/yagit.hpp>

void printImage2D(const yagit::Image2D& img){
    std::ostringstream oss;
    oss << "[";
    for(size_t i = 0; i < img.size(); i++){
        oss << (i == 0 ? "[" : " [");
        for(size_t j = 0; j < img[i].size() - 1; j++){
            oss << img[i][j] << ", ";
        }
        oss << img[i].back();
        oss << (i < img.size() - 1 ? "],\n" : "]");
    }
    oss << "]\n";
    std::cout << oss.str();
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
    // set the values of the images
    yagit::Image2D refImg = {
        {0.93, 0.95},
        {0.97, 1.00}
    };
    yagit::Image2D evalImg = {
        {0.93, 0.96},
        {0.90, 1.02}
    };

    // set the offset and the spacing of the images
    yagit::DataOffset refOffset{0, 0, -1};
    yagit::DataOffset evalOffset{0, 1, 0};
    yagit::DataSpacing refSpacing{2, 2, 2};
    yagit::DataSpacing evalSpacing{2, 2, 2};

    // create the reference image and the evaluated image
    yagit::ImageData refImgDose(refImg, refOffset, refSpacing);
    yagit::ImageData evalImgDose(evalImg, evalOffset, evalSpacing);

    // print info about the reference image and the evaluated image
    std::cout << "Reference image:\n";
    printImageData(refImgDose);
    std::cout << "------------------------------\n";
    std::cout << "Evaluated image:\n";
    printImageData(evalImgDose);
    std::cout << "------------------------------\n";

    // set gamma index parameters
    yagit::GammaParameters gammaParams;
    gammaParams.ddThreshold = 3.0;   // [%]
    gammaParams.dtaThreshold = 3.0;  // [mm]
    gammaParams.normalization = yagit::GammaNormalization::Global;
    gammaParams.globalNormDose = refImgDose.max();
    gammaParams.doseCutoff = 0;

    // calculate 2D gamma index using classic method
    const yagit::GammaResult gammaRes = yagit::gammaIndex2D(refImgDose, evalImgDose, gammaParams,
                                                            yagit::GammaMethod::Classic);

    // print gamma index image
    // expected:
    // [[0.471405, 0.57735],
    //  [1.10554, 0.816496]]
    std::cout << "Gamma index image:\n";
    printImage2D(gammaRes.getImage2D(0));

    // print gamma index statistics
    std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n"
              << "Gamma mean: " << gammaRes.meanGamma() << "\n"
              << "Gamma min: " << gammaRes.minGamma() << "\n"
              << "Gamma max: " << gammaRes.maxGamma() << "\n";
}
