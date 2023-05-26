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
#include <iostream>
#include <chrono>

#include <yagit.hpp>

void printImage2D(const yagit::Image2D<float>& img){
    std::cout << "[";
    for(size_t i = 0; i < img.size(); i++){
        std::cout << (i == 0 ? "[" : " [");
        for(size_t j = 0; j < img[i].size() - 1; j++){
            std::cout << img[i][j] << ", ";
        }
        std::cout << img[i].back() << "]";
        if(i < img.size() - 1){
            std::cout << ",\n";
        }
    }
    std::cout << "]\n";
}

void printImageData(const yagit::ImageData& imageData){
    // printImage2D(imageData.getImage2D(0));

    yagit::DataSize size = imageData.getSize();
    yagit::DataOffset offset = imageData.getOffset();
    yagit::DataSpacing spacing = imageData.getSpacing();
    std::cout << "size: (" << size.frames << ", " << size.rows << ", " << size.columns << ")\n"
              << "offset: (" << offset.frames << ", " << offset.rows << ", " << offset.columns << ")\n"
              << "spacing: (" << spacing.frames << ", " << spacing.rows << ", " << spacing.columns << ")\n";
}

void printGammaResult(const yagit::GammaResult& gammaRes){
    std::cout << "GIPR: " << gammaRes.passingRate() * 100 << "%\n"
              << "Gamma mean: " << gammaRes.meanGamma() << "\n"
              << "Gamma min: " << gammaRes.minGamma() << "\n"
              << "Gamma max: " << gammaRes.maxGamma() << "\n"
              << "NaN values: " << gammaRes.size() - gammaRes.nansize() << " / " << gammaRes.size() << "\n";
}

void printDifferent(const yagit::ImageData& img1, const yagit::ImageData& img2){
    int cnt = 0;
    std::cout << "DIFFERENT:\n";
    for(size_t i = 0; i < img1.size(); i++){
        if(std::abs(img1.get(i) - img2.get(i)) > std::max(std::abs(img1.get(i)), std::abs(img2.get(i))) * 2 * std::numeric_limits<float>::epsilon()){
            cnt++;
            // yagit::DataSize size = img1.getSize();
            // uint32_t rcSize = size.rows * size.columns;
            // uint32_t f = i / rcSize;
            // uint32_t r = (i % rcSize) / size.columns;
            // uint32_t c = (i % rcSize) % size.columns;
            // std::cout << "img[" << f << "," << r << "," << c << "] : " << img1.get(i) << ", " << img2.get(i) << "\n";
        }
    }
    std::cout << cnt << " / " << img1.size() << "\n";
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

        // yagit::DataOffset off = refImg.getOffset();
        // off.rows += 1;
        // refImg.setOffset(off);

        // uint32_t zframe = refImg.getSize().frames / 2 + 2;
        // refImg = refImg.getImageData2D(zframe, yagit::ImagePlane::Axial);
        // evalImg = evalImg.getImageData2D(zframe, yagit::ImagePlane::Axial);

        printImageData(refImg);
        std::cout << "------------------\n";
        printImageData(evalImg);
        std::cout << "------------------\n";

        float refMaxDose = refImg.max();
        // std::cout << "refMax: " << refMaxDose << "\n";
        yagit::GammaParameters gammaParams;
        gammaParams.ddThreshold = 3.0;   // [%]
        gammaParams.dtaThreshold = 3.0;  // [mm]
        gammaParams.normalization = yagit::GammaNormalization::Global;
        gammaParams.globalNormDose = refMaxDose;
        gammaParams.doseCutoff = 0;
        gammaParams.maxSearchDistance = 10;  // [mm]
        gammaParams.stepSize = gammaParams.dtaThreshold / 10;
        // gammaParams.stepSize = refImg.getSpacing().rows;
        
        auto begin = std::chrono::steady_clock::now();
        yagit::GammaResult gammaRes = yagit::gammaIndex3DWendling(refImg, evalImg, gammaParams);
        auto end = std::chrono::steady_clock::now();
        auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        printGammaResult(gammaRes);
        std::cout << "Time: " << timeMs << " [ms]" << std::endl;
        yagit::DataWriter::writeToMetaImage(gammaRes, "testWendling_wendling3.mha");

        // std::cout << "------------------\n";

        // begin = std::chrono::steady_clock::now();
        // yagit::GammaResult gammaRes2 = yagit::gammaIndex3D(refImg, evalImg, gammaParams);
        // end = std::chrono::steady_clock::now();
        // timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        // printGammaResult(gammaRes2);
        // std::cout << "Time: " << timeMs << " [ms]" << std::endl;
        // yagit::DataWriter::writeToMetaImage(gammaRes2, "testWendling_classic3.mha");

        // std::cout << "------------------\n";

        // std::cout << "THE SAME: " << (gammaRes == gammaRes2) << "\n";

        // printDifferent(gammaRes, gammaRes2);
        

    }
    catch(const std::exception &e){
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}
