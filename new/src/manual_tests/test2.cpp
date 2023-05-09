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


template <typename T>
void printDataInfo(const yagit::ImageData<T>& data){
    yagit::DataSize size = data.getSize();
    yagit::DataOffset offset = data.getOffset();
    yagit::DataSpacing spacing = data.getSpacing();

    std::cout << "size: " << size.frames << " " << size.rows << " " << size.columns << "\n"
              << "offset: " << offset.framesOffset << " " << offset.rowsOffset << " " << offset.columnsOffset << "\n"
              << "spacing: " << spacing.framesSpacing << " " << spacing.rowsSpacing << " " << spacing.columnsSpacing << "\n";
}

int main(){
    const std::string refImgFilename{"original_dose_beam_4.dcm"};
    const std::string evalImgFilename{"logfile_dose_beam_4.dcm"};

    try{
        const yagit::DoseData refImg = yagit::DataReader::readRTDoseDicom(refImgFilename, false);
        const yagit::DoseData evalImg = yagit::DataReader::readRTDoseDicom(evalImgFilename, false);

        yagit::DataSize size = refImg.getSize();
        int yframe = size.rows / 2;

        const yagit::DoseData refImg2D = refImg.getImageData2D(yframe, yagit::ImagePlane::Coronal);
        const yagit::DoseData evalImg2D = evalImg.getImageData2D(yframe, yagit::ImagePlane::Coronal);

        printDataInfo(refImg2D);
        std::cout << "---------------\n";
        printDataInfo(evalImg2D);
        std::cout << "----------------------------------\n";

        const auto refMaxDose = refImg2D.max();
        float dd = 3;  // [%]
        float dta = 3;  // [mm]
        auto normalization = yagit::GammaNormalization::Global;
        float globalNormDose = refMaxDose;
        float doseCutoff = 0.02 * refMaxDose;
        yagit::GammaParameters gammaParams{dd, dta, normalization, globalNormDose, doseCutoff};


        std::vector<double> times;

        for(int i=0; i < 3; i++){
            auto begin = std::chrono::steady_clock::now();

            const yagit::GammaResult gammaRes = yagit::gammaIndex2D(refImg2D, evalImg2D, gammaParams);

            auto end = std::chrono::steady_clock::now();
            auto timeSec = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0;
            std::cout << "Time: " << timeSec << "[s]" << std::endl;
            times.push_back(timeSec);

            std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n"
                      << "Gamma min: " << gammaRes.minGamma() << "\n"
                      << "Gamma max: " << gammaRes.maxGamma() << "\n"
                      << "Gamma mean: " << gammaRes.meanGamma() << "\n"
                      << "Size: " << gammaRes.size() << "\n"
                      << "Contains NAN: " << gammaRes.containsNan() << "\n"
                      << "Number of NANs: " << gammaRes.size() - gammaRes.nansize() << "\n";

            std::cout << "---------------------\n";
        }

        std::cout << "\nMean time: " << std::accumulate(times.begin(), times.end(), 0.0) / times.size() << "[s]\n";
    }
    catch(const std::exception& e){
        std::cerr << "EXCEPTION: " << e.what() <<std::endl;
    }
}
