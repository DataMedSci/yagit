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
 * @brief This file provides a simple example of using yagit - 2.5D gamma index.
 * 
 * @example{lineno}
 * This file provides a simple example of using yagit - 2.5D gamma index.
 * - First, it reads reference image and evaluated image from DICOM files.
 * - Then it calculates 3%G/3mm 2.5D gamma index of those 2 images using classic method.
 * Global normalization dose is set to maximum value of reference image.
 * Also, it is set to not take into account voxels with dose below 5% of max reference dose - 
 * in this case, NaN value will be set.
 * - At the end it prints gamma index passing rate and other info.
 */

#include <string>
#include <iostream>

#include <yagit.hpp>

int main(int argc, char** argv){
    if(argc <= 2){
        std::cerr << "too few arguments\n";
        std::cerr << "Usage: gamma25D refImgPath evalImgPath\n";
    }

    const std::string refImgPath{argv[1]};
    const std::string evalImgPath{argv[2]};

    try{
        const yagit::ImageData refImg = yagit::DataReader::readRTDoseDicom(refImgPath);
        const yagit::ImageData evalImg = yagit::DataReader::readRTDoseDicom(evalImgPath);

        float refMaxDose = refImg.max();
        yagit::GammaParameters gammaParams;
        gammaParams.ddThreshold = 3.0;   // [%]
        gammaParams.dtaThreshold = 3.0;  // [mm]
        gammaParams.normalization = yagit::GammaNormalization::Global;
        gammaParams.globalNormDose = refMaxDose;
        gammaParams.doseCutoff = 0.05 * refMaxDose;  // 5% * ref_max

        const yagit::GammaResult gammaRes = yagit::gammaIndex2_5D(refImg, evalImg, gammaParams);

        std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n"
                  << "Gamma mean: " << gammaRes.meanGamma() << "\n"
                  << "Gamma min: " << gammaRes.minGamma() << "\n"
                  << "Gamma max: " << gammaRes.maxGamma() << "\n"
                  << "NaN values: " << gammaRes.size() - gammaRes.nansize() << " / " << gammaRes.size() << "\n";

        // TODO: save gamma result to file
    }
    catch(const std::exception &e){
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}
