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

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <chrono>

#include <yagit/yagit.hpp>

void printImageData(const yagit::ImageData& imageData){
    yagit::DataSize size = imageData.getSize();
    yagit::DataOffset offset = imageData.getOffset();
    yagit::DataSpacing spacing = imageData.getSpacing();
    std::cout << "size: (" << size.frames << ", " << size.rows << ", " << size.columns << ")\n"
              << "offset: (" << offset.frames << ", " << offset.rows << ", " << offset.columns << ")\n"
              << "spacing: (" << spacing.frames << ", " << spacing.rows << ", " << spacing.columns << ")\n";
}

void printTimesStats(const std::vector<double>& timesMs){
    double mean = std::accumulate(timesMs.begin(), timesMs.end(), 0.0) / timesMs.size();
    double var = 0;
    for(const auto& e : timesMs){
        var += (e - mean) * (e - mean);
    }
    var /= timesMs.size();
    double sd = std::sqrt(var);
    auto [min, max] = std::minmax_element(timesMs.begin(), timesMs.end());

    std::cout << "mean time: " << mean << " [ms]\n"
              << "std time:  " << sd << " [ms]\n"
              << "min time:  " << *min << " [ms]\n"
              << "max time:  " << *max << " [ms]\n";
}

template <typename Function, typename... Args>
void measureInterp(uint32_t nrOfTests, Function&& func, Args&&... args){
    std::vector<double> timesMs;
    yagit::ImageData result;

    for(uint32_t i = 0; i < nrOfTests; i++){
        auto begin = std::chrono::steady_clock::now();

        result = func(std::forward<Args>(args)...);

        auto end = std::chrono::steady_clock::now();
        auto timeMs = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0;
        timesMs.push_back(timeMs);
    }

    printImageData(result);
    std::cout << "----------------\n";
    printTimesStats(timesMs);
}

template <typename Function, typename... Args>
void measureInterpAtPoint(uint32_t nrOfTests, Function&& func, Args&&... args){
    std::vector<double> timesMs;
    std::optional<float> result;

    for(uint32_t i = 0; i < nrOfTests; i++){
        auto begin = std::chrono::steady_clock::now();

        // in each test do it 100000 times, because 1 time takes too little time
        for(uint32_t j = 0; j < 100000; j++){
            result = func(std::forward<Args>(args)...);
        }

        auto end = std::chrono::steady_clock::now();
        auto timeMs = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0;
        timesMs.push_back(timeMs);
    }

    if(result != std::nullopt){
        std::cout << "result: " << *result << "\n";
    }
    else{
        std::cout << "result: null\n";
    }
    std::cout << "----------------\n";
    printTimesStats(timesMs);
}

int main(int argc, char** argv){
    if(argc <= 1){
        std::cerr << "too few arguments\n";
        std::cerr << "Usage: interpPerf imgPath\n";
        return 1;
    }

    const std::string imgPath{argv[1]};

    try{
        const yagit::ImageData img3D = yagit::DataReader::readRTDoseDicom(imgPath);

        std::cout << "ORIGINAL IMAGE:\n";
        printImageData(img3D);

        const float gridOffsetZ = -151.5;
        const float gridOffsetY = -399.9;
        const float gridOffsetX = -261.2;
        const float newSpacing = 0.4;

        std::cout << "\n====================================\n";
        std::cout << "LINEAR INTERPOLATION WITH SPACING:\n";
        yagit::ImageData (&linearAlongAxisPtr)(const yagit::ImageData&, float, yagit::ImageAxis) = yagit::Interpolation::linearAlongAxis;
        measureInterp(10, linearAlongAxisPtr, img3D, newSpacing, yagit::ImageAxis::Z);

        std::cout << "\n====================================\n";
        std::cout << "LINEAR INTERPOLATION WITH OFFSET AND SPACING:\n";
        yagit::ImageData (&linearAlongAxis2Ptr)(const yagit::ImageData&, float, float, yagit::ImageAxis) = yagit::Interpolation::linearAlongAxis;
        measureInterp(10, linearAlongAxis2Ptr, img3D, gridOffsetZ, newSpacing, yagit::ImageAxis::Z);

        std::cout << "\n====================================\n";
        std::cout << "BILINEAR INTERPOLATION WITH SPACING:\n";
        yagit::ImageData (&bilinearOnPlanePtr)(const yagit::ImageData&, float, float, yagit::ImagePlane) = yagit::Interpolation::bilinearOnPlane;
        measureInterp(10, bilinearOnPlanePtr, img3D, newSpacing, newSpacing, yagit::ImagePlane::Axial);

        std::cout << "\n====================================\n";
        std::cout << "BILINEAR INTERPOLATION WITH OFFSET AND SPACING:\n";
        yagit::ImageData (&bilinearOnPlane2Ptr)(const yagit::ImageData&, float, float, float, float, yagit::ImagePlane) = yagit::Interpolation::bilinearOnPlane;
        measureInterp(10, bilinearOnPlane2Ptr, img3D, gridOffsetY, gridOffsetX, newSpacing, newSpacing, yagit::ImagePlane::Axial);

        std::cout << "\n====================================\n";
        std::cout << "TRILINEAR INTERPOLATION WITH SPACING:\n";
        // zmien 5 na 10
        yagit::ImageData (&trilinearPtr)(const yagit::ImageData&, const yagit::DataSpacing&) = yagit::Interpolation::trilinear;
        measureInterp(5, trilinearPtr, img3D, yagit::DataSpacing{newSpacing, newSpacing, newSpacing});

        std::cout << "\n====================================\n";
        std::cout << "TRILINEAR INTERPOLATION WITH OFFSET AND SPACING:\n";
        yagit::ImageData (&trilinear2Ptr)(const yagit::ImageData&, const yagit::DataOffset&, const yagit::DataSpacing&) = yagit::Interpolation::trilinear;
        measureInterp(5, trilinear2Ptr, img3D, yagit::DataOffset{gridOffsetZ, gridOffsetY, gridOffsetX}, yagit::DataSpacing{newSpacing, newSpacing, newSpacing});

        std::cout << "\n####################################\n";
        std::cout << "BILINEAR INTERPOLATION AT POINT (x10000):\n";
        std::optional<float> (&bilinearAtPointPtr)(const yagit::ImageData&, uint32_t, float, float) = yagit::Interpolation::bilinearAtPoint;
        measureInterpAtPoint(1000, bilinearAtPointPtr, img3D, img3D.getSize().frames / 2, -300.5, 140.4);

        std::cout << "\n====================================\n";
        std::cout << "BILINEAR INTERPOLATION AT POINT OUTSIDE IMAGE (x10000):\n";
        measureInterpAtPoint(1000, bilinearAtPointPtr, img3D, img3D.getSize().frames / 2, -300, 1000);

        std::cout << "\n====================================\n";
        std::cout << "TRILINEAR INTERPOLATION AT POINT (x10000):\n";
        std::optional<float> (&trilinearAtPointPtr)(const yagit::ImageData&, float, float, float) = yagit::Interpolation::trilinearAtPoint;
        measureInterpAtPoint(1000, trilinearAtPointPtr, img3D, 0.1, -300.5, 140.4);

        std::cout << "\n====================================\n";
        std::cout << "TRILINEAR INTERPOLATION AT POINT OUTSIDE IMAGE (x10000):\n";
        measureInterpAtPoint(1000, trilinearAtPointPtr, img3D, 0.1, -300, 1000);
    }
    catch(const std::exception& e){
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}
