/********************************************************************************************
 * Copyright (C) 2023 'Yet Another Gamma Index Tool' Developers.
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

#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <functional>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <yagit/yagit.hpp>

const auto GLOBAL = yagit::GammaNormalization::Global;
const auto LOCAL = yagit::GammaNormalization::Local;

const float MAX_REF_DOSE = -1;  // set automatically max reference dose
const float DCO1 = -1;          // set automatically 1% of max ref dose
const float DCO5 = -5;          // set automatically 5% of max ref dose
const float DCO10 = -10;        // set automatically 10% of max ref dose

using GammaFunc = std::function<yagit::GammaResult(const yagit::ImageData&, const yagit::ImageData&,
                                                   const yagit::GammaParameters&)>;

struct Config{
    std::string method;
    std::string dimensions;
    yagit::GammaParameters gammaParams;
    uint32_t nrOfTests;
};

const std::vector<Config> configs = {
    // classic method
    {"classic",  "2D",   {3, 3, GLOBAL, MAX_REF_DOSE, 0,    0, 0},   20},
    {"classic",  "2D",   {3, 3, LOCAL,  0,            0,    0, 0},   20},
    {"classic",  "2D",   {3, 3, GLOBAL, MAX_REF_DOSE, DCO5, 0, 0},   20},
    {"classic",  "2D",   {3, 3, LOCAL,  0,            DCO5, 0, 0},   20},

    {"classic",  "2.5D", {3, 3, GLOBAL, MAX_REF_DOSE, 0,    0, 0},   4},
    {"classic",  "2.5D", {3, 3, LOCAL,  0,            0,    0, 0},   8},
    {"classic",  "2.5D", {3, 3, GLOBAL, MAX_REF_DOSE, DCO5, 0, 0},   10},
    {"classic",  "2.5D", {3, 3, LOCAL,  0,            DCO5, 0, 0},   10},

    // wendling method
    {"wendling", "2D",   {3, 3, GLOBAL, MAX_REF_DOSE, 0,    9, 0.3}, 1000},
    {"wendling", "2D",   {3, 3, LOCAL,  0,            0,    9, 0.3}, 200},
    {"wendling", "2D",   {3, 3, GLOBAL, MAX_REF_DOSE, DCO5, 9, 0.3}, 1000},
    {"wendling", "2D",   {3, 3, LOCAL,  0,            DCO5, 9, 0.3}, 1000},

    {"wendling", "2.5D", {3, 3, GLOBAL, MAX_REF_DOSE, 0,    9, 0.3}, 15},
    {"wendling", "2.5D", {3, 3, LOCAL,  0,            0,    9, 0.3}, 6},
    {"wendling", "2.5D", {3, 3, GLOBAL, MAX_REF_DOSE, DCO5, 9, 0.3}, 15},
    {"wendling", "2.5D", {3, 3, LOCAL,  0,            DCO5, 9, 0.3}, 6},

    {"wendling", "3D",   {3, 3, GLOBAL, MAX_REF_DOSE, 0,    9, 0.3}, 10},
    {"wendling", "3D",   {3, 3, LOCAL,  0,            0,    9, 0.3}, 3},
    {"wendling", "3D",   {3, 3, GLOBAL, MAX_REF_DOSE, DCO5, 9, 0.3}, 10},
    {"wendling", "3D",   {3, 3, LOCAL,  0,            DCO5, 9, 0.3}, 10},

    // {"wendling", "3D",   {3, 3, GLOBAL, MAX_REF_DOSE, DCO5, 9, 0.3}, 50},
    // {"wendling", "3D",   {2, 2, GLOBAL, MAX_REF_DOSE, DCO5, 6, 0.2}, 20},
    // {"wendling", "3D",   {3, 3, LOCAL,  0,            DCO5, 9, 0.3}, 50},
    // {"wendling", "3D",   {2, 2, LOCAL,  0,            DCO5, 6, 0.2}, 20}
};

std::string csvHeader(){
    return "method,dims,dd[%],dta[mm],norm,normDose,dco,maxSearchDist[mm],stepSize[mm],nrOfTests,"
           "meanTime[ms],stdTime[ms],minTime[ms],maxTime[ms],"
           "GIPR[%],meanGamma,minGamma,maxGamma,gammaSize,NaNvalues";
}

std::string configToCsv(const Config& config){
    const auto [method, dims, gammaParams, nrOfTests] = config;
    std::stringstream ss;
    ss << method << "," << dims << ","
       << gammaParams.ddThreshold << "," << gammaParams.dtaThreshold << ","
       << (gammaParams.normalization == GLOBAL ? "G" : "L") << "," << gammaParams.globalNormDose << ","
       << gammaParams.doseCutoff << "," << gammaParams.maxSearchDistance << "," << gammaParams.stepSize << ","
       << nrOfTests;
    return ss.str();
}

std::string timeStatsToCsv(const std::vector<double>& timesMs){
    double mean = std::accumulate(timesMs.begin(), timesMs.end(), 0.0) / timesMs.size();
    double var = 0;
    for(const auto& e : timesMs){
        var += (e - mean) * (e - mean);
    }
    var /= timesMs.size();
    double sd = std::sqrt(var);
    auto [min, max] = std::minmax_element(timesMs.begin(), timesMs.end());

    std::stringstream ss;
    std::streamsize ssPrec = ss.precision();
    ss << std::fixed << std::setprecision(6)
       << mean << "," << sd << ","
       << std::fixed << std::setprecision(3)
       << *min << "," << *max
       << std::defaultfloat << std::setprecision(ssPrec);

    std::cout << " - mean time: " << mean << " ms";

    return ss.str();
}

std::string gammaResultToCsv(const yagit::GammaResult& gammaRes){
    std::stringstream ss;
    ss << gammaRes.passingRate() * 100 << ","
       << gammaRes.meanGamma() << "," << gammaRes.minGamma() << "," << gammaRes.maxGamma() << ","
       << gammaRes.size() << "," << (gammaRes.size() - gammaRes.nansize());
    return ss.str();
}

void measureGamma(GammaFunc gammaFunc, const yagit::ImageData& refImg, const yagit::ImageData& evalImg,
                  const yagit::GammaParameters& gammaParams, uint32_t nrOfTests, std::ofstream& csvFile){
    std::vector<double> timesMs;
    yagit::GammaResult gammaRes;

    for(uint32_t i = 0; i < nrOfTests; i++){
        auto begin = std::chrono::steady_clock::now();

        gammaRes = gammaFunc(refImg, evalImg, gammaParams);

        auto end = std::chrono::steady_clock::now();
        auto timeMs = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0;
        timesMs.push_back(timeMs);
    }

    csvFile << timeStatsToCsv(timesMs) << "," << gammaResultToCsv(gammaRes) << "\n";
    csvFile.flush();
}


int main(int argc, char** argv){
    if(argc <= 2){
        std::cerr << "too few arguments\n";
        std::cerr << "Usage: gammaPerf refImgPath evalImgPath [outputCsvPath]\n";
        return 1;
    }

    const std::string refImgPath{argv[1]};
    const std::string evalImgPath{argv[2]};
    const std::string outputCsvPath = (argc > 3 ? argv[3] : "output.csv");

    try{
        const yagit::ImageData refImg3D = yagit::DataReader::readRTDoseDicom(refImgPath);
        const yagit::ImageData evalImg3D = yagit::DataReader::readRTDoseDicom(evalImgPath);

        uint32_t zframe = refImg3D.getSize().frames / 2 + 2;
        const yagit::ImageData refImg2D = refImg3D.getImageData2D(zframe, yagit::ImagePlane::Axial);
        const yagit::ImageData evalImg2D = evalImg3D.getImageData2D(zframe, yagit::ImagePlane::Axial);

        const float refMaxDose3D = refImg3D.max();
        const float refMaxDose2D = refImg2D.max();

        std::ofstream csvFile(outputCsvPath);
        if(!csvFile.is_open()){
            throw std::runtime_error("Cannot open " + outputCsvPath + " file");
        }
        csvFile << csvHeader() << "\n";

        for(size_t i = 0; i < configs.size(); i++){
            std::cout << i+1 << "/" << configs.size();

            auto [method, dims, gammaParams, nrOfTests] = configs[i];
            if(gammaParams.globalNormDose == MAX_REF_DOSE){
                gammaParams.globalNormDose = (dims == "2D" ? refMaxDose2D : refMaxDose3D);
            }

            if(gammaParams.doseCutoff == DCO1){
                gammaParams.doseCutoff = 0.01 * (dims == "2D" ? refMaxDose2D : refMaxDose3D);
            }
            else if(gammaParams.doseCutoff == DCO5){
                gammaParams.doseCutoff = 0.05 * (dims == "2D" ? refMaxDose2D : refMaxDose3D);
            }
            else if(gammaParams.doseCutoff == DCO10){
                gammaParams.doseCutoff = 0.10 * (dims == "2D" ? refMaxDose2D : refMaxDose3D);
            }

            csvFile << configToCsv({method, dims, gammaParams, nrOfTests}) << ",";

            if(method == "classic"){
                if(dims == "2D"){
                    measureGamma(yagit::gammaIndex2DClassic, refImg2D, evalImg2D, gammaParams, nrOfTests, csvFile);
                }
                else if(dims == "2.5D"){
                    measureGamma(yagit::gammaIndex2_5DClassic, refImg3D, evalImg3D, gammaParams, nrOfTests, csvFile);
                }
                else if(dims == "3D"){
                    measureGamma(yagit::gammaIndex3DClassic, refImg3D, evalImg3D, gammaParams, nrOfTests, csvFile);
                }
            }
            else if(method == "wendling"){
                if(dims == "2D"){
                    measureGamma(yagit::gammaIndex2DWendling, refImg2D, evalImg2D, gammaParams, nrOfTests, csvFile);
                }
                else if(dims == "2.5D"){
                    measureGamma(yagit::gammaIndex2_5DWendling, refImg3D, evalImg3D, gammaParams, nrOfTests, csvFile);
                }
                else if(dims == "3D"){
                    measureGamma(yagit::gammaIndex3DWendling, refImg3D, evalImg3D, gammaParams, nrOfTests, csvFile);
                }
            }

            std::cout << "\n";
        }

        csvFile.close();
    }
    catch(const std::exception& e){
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}
