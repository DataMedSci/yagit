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

// TEMPORARY FILE!!!
// FOR TESTING ONLY

#include <string>
#include <iostream>
#include <chrono>
#include <numeric>

// #include <yagit.hpp>
#include "DoseData.hpp"
#include "DataReader.hpp"
#include "Gamma.hpp"

int main(){
    const std::string refImgFilename{"original_dose_beam_4.dcm"};
    const std::string evalImgFilename{"logfile_dose_beam_4.dcm"};

    const yagit::DoseData refImg = yagit::DataReader::readRTDoseDicom(refImgFilename);
    const yagit::DoseData evalImg = yagit::DataReader::readRTDoseDicom(evalImgFilename);

    const auto refMaxDose = refImg.max();
    float dd = 3;  // [%]
    float dta = 3;  // [mm]
    auto normalization = yagit::GammaNormalization::Global;
    float globalNormDose = refMaxDose;
    float doseCutoff = 0.1 * refMaxDose;
    yagit::GammaParameters gammaParams{dd, dta, normalization, globalNormDose, doseCutoff};

    const yagit::GammaResult gammaRes = yagit::gammaIndex2_5D(refImg, evalImg, gammaParams);
    std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n";
}
