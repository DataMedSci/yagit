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
/**
 * @file
 * @brief This file provides a simple example of using yagit - 2D gamma index with interpolation of eval img.
 * 
 * @example{lineno}
 * This file provides a simple example of using yagit - 2D gamma index with interpolation of eval img.
 * - First, it reads reference image and evaluated image from DICOM files.
 * - Next, it takes 2D frames from the middle of images in coronal plane.
 * - After that, it interpolates eval image to be on the same grid as ref image.
 * - Then it calculates 2%L/2mm 2D gamma index of those 2D images using classic method.
 * Also, it is set to not take into account voxels with dose below 1% of max reference dose -
 * in this case, NaN value will be set.
 * - After that, it prints gamma index passing rate and other info.
 * - At the end, it saves result to MetaImage file.
 */

#include <string>
#include <iostream>

#include <yagit.hpp>

int main(int argc, char** argv){
    if(argc <= 2){
        std::cerr << "too few arguments\n";
        std::cerr << "Usage: gamma2DInterp refImgPath evalImgPath\n";
        return 1;
    }

    const std::string refImgPath{argv[1]};
    const std::string evalImgPath{argv[2]};

    try{
        yagit::ImageData refImg = yagit::DataReader::readRTDoseDicom(refImgPath);
        yagit::ImageData evalImg = yagit::DataReader::readRTDoseDicom(evalImgPath);

        // get 2D coronal frame from the middle of the images
        uint32_t yframe = refImg.getSize().rows / 2;
        refImg = refImg.getImageData2D(yframe, yagit::ImagePlane::Coronal);
        evalImg = evalImg.getImageData2D(yframe, yagit::ImagePlane::Coronal);

        // interpolate eval image to have values on the same grid as ref image
        evalImg = yagit::Interpolation::bilinearOnPlane(evalImg, refImg, yagit::ImagePlane::Axial);

        float refMaxDose = refImg.max();
        yagit::GammaParameters gammaParams;
        gammaParams.ddThreshold = 2.0;   // [%]
        gammaParams.dtaThreshold = 2.0;  // [mm]
        gammaParams.normalization = yagit::GammaNormalization::Local;
        gammaParams.doseCutoff = 0.01 * refMaxDose;  // 1% * ref_max

        const yagit::GammaResult gammaRes = yagit::gammaIndex2D(refImg, evalImg, gammaParams,
                                                                yagit::GammaMethod::Classic);

        std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n"
                  << "Gamma mean: " << gammaRes.meanGamma() << "\n"
                  << "Gamma min: " << gammaRes.minGamma() << "\n"
                  << "Gamma max: " << gammaRes.maxGamma() << "\n"
                  << "NaN values: " << gammaRes.size() - gammaRes.nansize() << " / " << gammaRes.size() << "\n";

        yagit::DataWriter::writeToMetaImage(gammaRes, "gamma_index_2d.mha");
    }
    catch(const std::exception &e){
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}
