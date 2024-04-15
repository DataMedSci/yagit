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
 * @file gammaWithInterp.cpp
 * @brief This file provides a simple example of using yagit - 2D gamma index with interpolation of eval image.
 * 
 * @example{lineno}
 * Example demonstrating the 2D gamma index with interpolation of an evaluated image.
 * 1. Read a reference image and an evaluated image from DICOM files.
 * 2. Take 2D frames from the middle of the images in the coronal plane.
 * 3. Interpolate the evaluated image to have a spacing set to 1/3
 *    of the DTA criterion or lower.
 * 4. Calculate the 2D gamma index of those 2D images using the classic method.
 *    The parameters are: 2%G/2mm,
 *    normalization dose is set to max value of the reference image,
 *    and dose cutoff is set to 1% of max value of the reference image.
 * 5. Print gamma index passing rate and other statistics.
 * 6. Save the result to a MetaImage file.
 */

#include <string>
#include <iostream>

#include <yagit/yagit.hpp>

int main(int argc, char** argv){
    if(argc <= 2){
        std::cerr << "too few arguments\n";
        std::cerr << "Usage: gammaWithInterp refImgPath evalImgPath\n";
        return 1;
    }

    const std::string refImgPath{argv[1]};
    const std::string evalImgPath{argv[2]};

    try{
        // read a reference image and an evaluated image from DICOM files
        yagit::ImageData refImg = yagit::DataReader::readRTDoseDicom(refImgPath);
        yagit::ImageData evalImg = yagit::DataReader::readRTDoseDicom(evalImgPath);

        // take 2D frames from the middle of the images in the coronal plane
        uint32_t yframe = refImg.getSize().rows / 2;
        refImg = refImg.getImageData2D(yframe, yagit::ImagePlane::Coronal);
        evalImg = evalImg.getImageData2D(yframe, yagit::ImagePlane::Coronal);    

        // set gamma index parameters
        float refMaxDose = refImg.max();
        yagit::GammaParameters gammaParams;
        gammaParams.ddThreshold = 2.0;   // [%]
        gammaParams.dtaThreshold = 2.0;  // [mm]
        gammaParams.normalization = yagit::GammaNormalization::Global;
        gammaParams.globalNormDose = refMaxDose;
        gammaParams.doseCutoff = 0.01 * refMaxDose;  // 1% * ref_max

        // interpolate the evaluated image to have a spacing set to 1/3 of the DTA criterion or lower
        float newSpacingY = std::min(gammaParams.dtaThreshold / 3, evalImg.getSpacing().rows);
        float newSpacingX = std::min(gammaParams.dtaThreshold / 3, evalImg.getSpacing().columns);
        evalImg = yagit::Interpolation::bilinearOnPlane(evalImg, newSpacingY, newSpacingX, yagit::ImagePlane::YX);

        // calculate the 2D gamma index using the classic method
        std::cout << "Calculating 2D gamma index using classic method with interpolated evaluated image\n";
        const yagit::GammaResult gammaRes = yagit::gammaIndex2D(refImg, evalImg, gammaParams,
                                                                yagit::GammaMethod::Classic);

        // print gamma index statistics
        std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n"
                  << "Gamma mean: " << gammaRes.meanGamma() << "\n"
                  << "Gamma min: " << gammaRes.minGamma() << "\n"
                  << "Gamma max: " << gammaRes.maxGamma() << "\n"
                  << "NaN values: " << gammaRes.size() - gammaRes.nansize() << " / " << gammaRes.size() << "\n";

        // save the result containing gamma index image to a MetaImage file
        yagit::DataWriter::writeToMetaImage(gammaRes, "gamma_index_2d.mha");
    }
    catch(const std::exception &e){
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}
