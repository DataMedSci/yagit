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

// TEMPORARY FILE!!!
// FOR TESTING ONLY

#include <string>
#include <optional>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <iostream>

#include <yagit.hpp>

void printTimesStats(const std::vector<long long>& timesMs){
	double mean = std::accumulate(timesMs.begin(), timesMs.end(), 0.0) / timesMs.size();
	double var = 0;
	for(const auto& e : timesMs){
		var += (e - mean) * (e - mean);
	}
	var /= timesMs.size();
	double sd = std::sqrt(var);
    long long min = *std::min_element(timesMs.begin(), timesMs.end());
    long long max = *std::max_element(timesMs.begin(), timesMs.end());
	std::cout << "mean time: " << mean << " [ms]\n"
            //   << "var time:  " << var << " [ms^2]\n"
              << "std time:  " << sd << " [ms]\n"
              << "min time:  " << min << " [ms]\n"
              << "max time:  " << max << " [ms]\n";
}

void printGammaResult(const yagit::GammaResult& gammaRes){
    std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n"
              << "Gamma mean: " << gammaRes.meanGamma() << "\n"
              << "Gamma min: " << gammaRes.minGamma() << "\n"
              << "Gamma max: " << gammaRes.maxGamma() << "\n"
              << "NaN values: " << gammaRes.size() - gammaRes.nansize() << " / " << gammaRes.size() << "\n";
}

int main(int argc, char** argv){
    if(argc <= 2){
        std::cerr << "too few arguments\n";
        std::cerr << "Usage: testWendling refImgPath evalImgPath\n";
    }

    const std::string refImgPath{argv[1]};
    const std::string evalImgPath{argv[2]};

    try{
        yagit::ImageData refImg = yagit::DataReader::readRTDoseDicom(refImgPath);
        yagit::ImageData evalImg = yagit::DataReader::readRTDoseDicom(evalImgPath);

        // uint32_t zframe = refImg.getSize().frames / 2 + 2;
        // refImg = refImg.getImageData2D(zframe, yagit::ImagePlane::Axial);
        // evalImg = evalImg.getImageData2D(zframe, yagit::ImagePlane::Axial);

        // yagit::DataOffset off = refImg.getOffset();
        // // off.rows -= 3;
        // off.frames += 3;
        // refImg.setOffset(off);

        float refMaxDose = refImg.max();
        yagit::GammaParameters gammaParams;
        gammaParams.ddThreshold = 3.0;   // [%]
        gammaParams.dtaThreshold = 3.0;  // [mm]
        gammaParams.normalization = yagit::GammaNormalization::Global;
        // gammaParams.normalization = yagit::GammaNormalization::Local;
        gammaParams.globalNormDose = refMaxDose;
        gammaParams.doseCutoff = 0;//1e-6;
        gammaParams.maxSearchDistance = 10;  // [mm]
        gammaParams.stepSize = gammaParams.dtaThreshold / 10;
        // gammaParams.stepSize = refImg.getSpacing().rows;
        
        constexpr uint32_t NUMBER_OF_TESTS = 10;
        std::vector<long long> times;

        for(int i = 0; i < NUMBER_OF_TESTS; i++){
            // std::cout << i+1 << ": ";
            auto begin = std::chrono::steady_clock::now();

            yagit::GammaResult gammaRes = yagit::gammaIndex3DWendling(refImg, evalImg, gammaParams);
            
            auto end = std::chrono::steady_clock::now();
            auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            
            // std::cout << "Time: " << timeMs << " [ms]\n";
            times.push_back(timeMs);
            std::cout << "-";

            if(i == NUMBER_OF_TESTS - 1){
                std::cout << "\n";
                printTimesStats(times);
                std::cout << "---------------------\n";
                printGammaResult(gammaRes);
                // yagit::DataWriter::writeToMetaImage(gammaRes, "testWendlingTime3_interp.mha");
                // yagit::DataWriter::writeToMetaImage(gammaRes, "testWendlingTime3_local.mha");
            }
        }
    }
    catch(const std::exception &e){
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}
