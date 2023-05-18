/******************************************************************************************
 * This file is part of 'yet Another Gamma Index Tool'.
 *
 * 'yet Another Gamma Index Tool' is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'yet Another Gamma Index Tool'; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 ******************************************************************************************/
/**
 * @file
 * @brief This file provides a simple example of using yagit - 2D gamma index.
 * 
 * @example{lineno}
 * This file provides a simple example of using yagit - 2D gamma index.
 * - First, it reads reference image and evaluated image from DICOM files.
 * - Next, it takes 2D frames from the middle of images in coronal plane.
 * - Then it calculates 2%L/2mm 2D gamma index of those 2D images using classic method.
 * Also, it is set to not take into account voxels with dose below 1% of max reference dose -
 * in this case, NaN value will be set.
 * - At the end it prints gamma index passing rate and other info.
 */

#include <string>
#include <iostream>

#include <yagit.hpp>

int main(int argc, char** argv){
    if(argc <= 2){
        std::cerr << "too few arguments\n";
        std::cerr << "Usage: gamma2D refImgPath evalImgPath\n";
    }

    const std::string refImgPath{argv[1]};
    const std::string evalImgPath{argv[2]};

    try{
        yagit::ImageData refImg = yagit::DataReader::readRTDoseDicom(refImgPath);
        yagit::ImageData evalImg = yagit::DataReader::readRTDoseDicom(evalImgPath);

        // get 2D coronal frame from the middle of the images
        uint32_t yframe = refImg.getSize().rows / 2;
        refImg = refImg.getImageData2D(yframe, yagit::ImagePlane::Coronal);
        evalImg = refImg.getImageData2D(yframe, yagit::ImagePlane::Coronal);

        float refMaxDose = refImg.max();
        yagit::GammaParameters gammaParams;
        gammaParams.ddThreshold = 2.0;   // [%]
        gammaParams.dtaThreshold = 2.0;  // [mm]
        gammaParams.normalization = yagit::GammaNormalization::Local;
        gammaParams.doseCutoff = 0.01 * refMaxDose;  // 1% * ref_max

        const yagit::GammaResult gammaRes = yagit::gammaIndex2D(refImg, evalImg, gammaParams);

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
