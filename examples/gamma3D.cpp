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
 * @file gamma3D.cpp
 * @brief This file provides a simple example of using yagit - 3D gamma index.
 * 
 * @example{lineno}
 * Example demonstrating the 3D gamma index.
 * 1. Read a reference image and an evaluated image from DICOM files.
 * 2. Calculate the 3D gamma index of those images using the Wendling method.
 *    The parameters are: 3%G/3mm,
 *    normalization dose is set to max value of the reference image,
 *    and dose cutoff is set to 10% of max value of the reference image.
 * 3. Print gamma index passing rate and other statistics.
 * 4. Save the result to a MetaImage file.
 */

#include <string>
#include <iostream>
#include <sstream>

#include <yagit/yagit.hpp>

std::string floatToString(float d){
    std::ostringstream oss;
    oss << d;
    return oss.str();
}

std::string gammaParametersToString(const yagit::GammaParameters& gammaParams){
    std::string dd = floatToString(gammaParams.ddThreshold);
    std::string dta = floatToString(gammaParams.dtaThreshold);
    char norm = gammaParams.normalization == yagit::GammaNormalization::Global ? 'G' : 'L';
    return dd + "%" + norm + "/" + dta + "mm";
}

int main(int argc, char** argv){
    if(argc <= 2){
        std::cerr << "too few arguments\n";
        std::cerr << "Usage: gamma3D refImgPath evalImgPath\n";
        return 1;
    }

    const std::string refImgPath{argv[1]};
    const std::string evalImgPath{argv[2]};

    try{
        // read a reference image and an evaluated image from DICOM files
        const yagit::ImageData refImg = yagit::DataReader::readRTDoseDicom(refImgPath);
        const yagit::ImageData evalImg = yagit::DataReader::readRTDoseDicom(evalImgPath);

        // set gamma index parameters
        float refMaxDose = refImg.max();
        yagit::GammaParameters gammaParams;
        gammaParams.ddThreshold = 3.0;   // [%]
        gammaParams.dtaThreshold = 3.0;  // [mm]
        gammaParams.normalization = yagit::GammaNormalization::Global;
        gammaParams.globalNormDose = refMaxDose;
        gammaParams.doseCutoff = 0.1 * refMaxDose;  // 10% * ref_max
        // two parameters below are exclusively used by the Wendling method
        gammaParams.maxSearchDistance = 10;                    // [mm]
        gammaParams.stepSize = gammaParams.dtaThreshold / 10;  // [mm]

        // print the gamma index parameters
        std::cout << "Calculating 3D gamma index using Wendling method with parameters: "
                  << gammaParametersToString(gammaParams) << "\n";

        // calculate the 3D gamma index using the Wendling method
        const yagit::GammaResult gammaRes = yagit::gammaIndex3D(refImg, evalImg, gammaParams,
                                                                yagit::GammaMethod::Wendling);

        // print gamma index statistics
        std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n"
                  << "Gamma mean: " << gammaRes.meanGamma() << "\n"
                  << "Gamma min: " << gammaRes.minGamma() << "\n"
                  << "Gamma max: " << gammaRes.maxGamma() << "\n"
                  << "NaN values: " << gammaRes.size() - gammaRes.nansize() << " / " << gammaRes.size() << "\n";

        // save the result containing the gamma index image to a MetaImage file
        yagit::DataWriter::writeToMetaImage(gammaRes, "gamma_index_3d.mha");
    }
    catch(const std::exception &e){
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}
