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
 * @brief This file provides a simple example of using yagit - 3D gamma index with interpolation of eval img.
 * 
 * @example{lineno}
 * This file provides a simple example of using yagit - 3D gamma index with interpolation of eval img.
 * - First, it reads reference image and evaluated image from DICOM files.
 * - After that, it interpolates eval image to be on the same grid as ref image.
 * - Then it calculates 3%G/3mm 3D gamma index of those images using classic method.
 * Also, it is set to not take into account voxels with dose below 10% of max reference dose -
 * in this case, NaN value will be set.
 * - At the end it prints gamma index passing rate and other info.
 */

#include <string>
#include <iostream>
#include <sstream>

#include <yagit.hpp>

std::string doubleToString(double d){
    std::ostringstream oss;
    oss << d;
    return oss.str(); 
}

std::string gammaParametersToString(const yagit::GammaParameters& gammaParams){
    std::string dd = doubleToString(gammaParams.ddThreshold);
    std::string dta = doubleToString(gammaParams.dtaThreshold);
    char norm = gammaParams.normalization == yagit::GammaNormalization::Global ? 'G' : 'L';
    return dd + "%" + norm + "/" + dta + "mm";
}

int main(int argc, char** argv){
    if(argc <= 2){
        std::cerr << "too few arguments\n";
        std::cerr << "Usage: gammaInterp3D refImgPath evalImgPath\n";
    }

    const std::string refImgPath{argv[1]};
    const std::string evalImgPath{argv[2]};

    try{
        std::cout << "Reading reference image\n";
        yagit::DoseData refImg = yagit::DataReader::readRTDoseDicom(refImgPath, true);
        std::cout << "------------------------------\n";
        std::cout << "Reading evaluated image\n";
        yagit::DoseData evalImg = yagit::DataReader::readRTDoseDicom(evalImgPath, true);
        std::cout << "------------------------------\n";

        // interpolate eval image to have values on the same grid as ref image
        std::cout << "Interpolating evaluated image\n";
        evalImg = yagit::Interpolation::trilinear(evalImg, refImg);

        float refMaxDose = refImg.max();
        yagit::GammaParameters gammaParams;
        gammaParams.ddThreshold = 3.0;   // [%]
        gammaParams.dtaThreshold = 3.0;  // [mm]
        gammaParams.normalization = yagit::GammaNormalization::Global;
        gammaParams.globalNormDose = refMaxDose;
        gammaParams.doseCutoff = 0.1 * refMaxDose;  // 10% * ref_max

        std::cout << "Calculating 3D gamma index with parameters: " << gammaParametersToString(gammaParams) << "\n";
        const yagit::GammaResult gammaRes = yagit::gammaIndex3D(refImg, evalImg, gammaParams);

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
