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

#include <iostream>
#include <numeric>
#include <algorithm>

#include "DataReader.hpp"
#include "ImageData.hpp"

#include "GammaParameters.hpp"
#include "GammaResult.hpp"
#include "Gamma.hpp"

void printDataInfo(const yagit::ImageData& data){
    yagit::DataSize size = data.getSize();
    yagit::DataOffset offset = data.getOffset();
    yagit::DataSpacing spacing = data.getSpacing();

    std::cout << "size: " << size.frames << " " << size.rows << " " << size.columns << "\n"
              << "offset: " << offset.framesOffset << " " << offset.rowsOffset << " " << offset.columnsOffset << "\n"
              << "spacing: " << spacing.framesSpacing << " " << spacing.rowsSpacing << " " << spacing.columnsSpacing << "\n";
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    if(!v.empty()){
        out << '[';
        for(const auto& el : v){
            out << el << ", ";
        }
        out << "\b\b]"; // use two ANSI backspace characters '\b' to overwrite final ", "
    }
    return out;
}

void printVecInfo(const std::vector<float>& vec){
    std::cout << "vec: " << vec << "\n";
    std::cout << "min: " << *std::min_element(vec.begin(), vec.end()) << "\n";
    std::cout << "max: " << *std::max_element(vec.begin(), vec.end()) << "\n";
    std::cout << "sum: " << std::accumulate(vec.begin(), vec.end(), 0.0) << "\n";
}

void printDataStatsInfo(const yagit::ImageData& data){
    std::cout << "size: " << data.size() << "\n"
              << "min: " << data.min() << "\n"
              << "max: " << data.max() << "\n"
              << "sum: " << data.sum() << "\n"
              << "mean: " << data.mean() << "\n"
              << "var: " << data.var() << "\n";
}

void printDataStatsNanInfo(const yagit::ImageData& data){
    std::cout << "size: " << data.size() << "\n"
              << "nansize: " << data.nansize() << "\n"
              << "nanmin: " << data.nanmin() << "\n"
              << "nanmax: " << data.nanmax() << "\n"
              << "nansum: " << data.nansum() << "\n"
              << "nanmean: " << data.nanmean() << "\n"
              << "nanvar: " << data.nanvar() << "\n"
              << "conatinsNan: " << data.containsNan() << "\n"
              << "containsInf: " << data.containsInf() << "\n";
}

int main(){

    try{
        const std::string filename{"original_dose_beam_4.dcm"};
        yagit::ImageData data = yagit::DataReader::readRTDoseDicom(filename, true);

        std::cout << "--------------------------------------------------\n";
        std::cout << "DATA from DICOM RT Dose\n";

        printDataInfo(data);

        yagit::DataSize size = data.getSize();
        std::cout << data.get(80, 70, 100) << "\n"
                  << data.get(80*size.rows*size.columns + 70*size.columns + 100) << "\n"
                  << data.at(80, 70, 100) << "\n";

        std::cout << "--------------------------------------------------\n";
        std::cout << "DATA from manual Image2d\n";

        yagit::Image2D<float> img2d = {
            {1.2, 4.2, 6.3, 0.3, 0.1, 0.9},
            {0.4, 5.5, 1.3, 2.3, 0.0, 0.1},
            {3.3, 0.1, 0.4, 9.3, 9.9, 2.2},
            {0.1, 5.3, 0.1, 9.9, 2.2, 3.3}
        };
        yagit::ImageData data2d(img2d, {0,1,20}, {2,2,2});

        std::cout << data2d.get(0, 1, 1) << "\n";

        const yagit::ImageData constData2d(data2d);
        std::cout << constData2d.get(0, 1, 1) << "\n";


        std::cout << "--------------------------------------------------\n";
        std::cout << "DATA from manual Image3d\n";

        yagit::Image3D<float> img3d = {
            {
                {1.0, 2.1},
                {3.3, 4.2}
            }, {
                {4.3, 0.1},
                {7.1, 0.3}
            }
        };
        yagit::ImageData data3d(img3d, {10,23,45}, {2,5,7});

        std::cout << data3d.get(0, 1, 1) << "\n";
        std::cout << data3d.get(0, 0, 1) << "\n";
        std::cout << data3d.get(1, 0, 0) << "\n";

        std::cout << "--------------------------------------------------\n";
        std::cout << "DATA 3D from manual Image3d\n";

        std::cout << "Axial: " << data3d.getImage3D() << "\n"
                  << "Coronal: " << data3d.getImage3D(yagit::ImagePlane::Coronal) << "\n"
                  << "Sagittal: " << data3d.getImage3D(yagit::ImagePlane::Sagittal) << "\n";

        std::cout << "--------------------------------------------------\n";
        std::cout << "DATA 2D from manual Image3d\n";

        std::cout << "Axial 0: " << data3d.getImage2D(0) << "\n"
                  << "Axial 1: " << data3d.getImage2D(1) << "\n"
                  << "Coronal 0: " << data3d.getImage2D(0, yagit::ImagePlane::Coronal) << "\n"
                  << "Coronal 1: " << data3d.getImage2D(1, yagit::ImagePlane::Coronal) << "\n"
                  << "Sagittal 0: " << data3d.getImage2D(0, yagit::ImagePlane::Sagittal) << "\n"
                  << "Sagittal 1: " << data3d.getImage2D(1, yagit::ImagePlane::Sagittal) << "\n";


        std::cout << "--------------------------------------------------\n";
        std::cout << "min max dose\n";

        std::cout << "min: " << data2d.min() << "\n"
                  << "max: " << data2d.max() << "\n";

        //===========================================================================================================
        std::cout << "==================================================\n";
    
        // yagit::ImageData d0;
        yagit::ImageData d1(img2d, {0,0,0}, {1,1,1});
        
        yagit::ImageData d1c(d1);
        yagit::ImageData d1c2 = d1;

        // yagit::ImageData d1m(std::move(d1));
        // yagit::ImageData d1m2 = std::move(d1);

        std::cout << std::boolalpha << (d1 == d1) << " ";

        yagit::ImageData id1(img2d, {0,0,0}, {1,1,1});
        std::cout << std::boolalpha << (id1 == id1) << "\n";

        std::cout << std::boolalpha << (yagit::DataSize{1,1,1} == yagit::DataSize{3,3,3}) << " "
                                    << (yagit::DataOffset{1,1,1} == yagit::DataOffset{3,3,3}) << " "
                                    << (yagit::DataSpacing{1,1,1} == yagit::DataSpacing{3,3,3}) << "\n";

        // std::cout << "==================================================\n";
        // std::cout << "Convert img float to int\n";
        // yagit::Image2D<int> img2dint = {
        //     {1,2,3},
        //     {4,5,6}
        // };
        // yagit::ImageData idint(img2dint, {0,0,0}, {1,1,1});

        // std::vector<float> vf{1.5, 2.23, 3.12, 9.0};
        // yagit::ImageData idint2(vf, {1,1,4}, {0,0,0}, {1,1,1});

        // for(int i=0; i < idint2.size(); i++){
        //     std::cout << idint2.get(i) << " ";
        // }
        // std::cout << "\n";

        std::cout << "==================================================\n";
        std::cout << "IMAGE DATA 2D\n";
        printDataInfo(data3d);
        std::cout << "--------\n";
        printDataInfo(data3d.getImageData2D(0));
        std::cout << "----\n";
        printDataInfo(data3d.getImageData2D(1));
        std::cout << "--------\n";
        printDataInfo(data3d.getImageData2D(0, yagit::ImagePlane::Coronal));
        std::cout << "----\n";
        printDataInfo(data3d.getImageData2D(1, yagit::ImagePlane::Coronal));
        std::cout << "--------\n";
        printDataInfo(data3d.getImageData2D(0, yagit::ImagePlane::Sagittal));
        std::cout << "----\n";
        printDataInfo(data3d.getImageData2D(1, yagit::ImagePlane::Sagittal));

        std::cout << "==================================================\n";
        std::cout << "IMAGE DATA 3D\n";
        printDataInfo(data3d.getImageData3D(yagit::ImagePlane::Axial));
        std::cout << "--------\n";
        printDataInfo(data3d.getImageData3D(yagit::ImagePlane::Coronal));
        std::cout << "--------\n";
        printDataInfo(data3d.getImageData3D(yagit::ImagePlane::Sagittal));

        //===========================================================================================================
        std::cout << "==================================================\n";
        std::cout << "GAMMA RESULT\n";

        yagit::Image2D<float> img2dgr = {
            {0.9, 0.5, 0.1, 0.3, 1.2},
            {0.0, 1.2, 2.3, 0.9, 0.1}
        };
        yagit::GammaResult gr(img2dgr, {0,0,0}, {1,1,1});

        std::cout << "GIPR: " << gr.passingRate() << "\n";

        //===========================================================================================================
        std::cout << "==================================================\n";
        std::cout << "GAMMA\n";
        yagit::GammaParameters gp;

        yagit::Image2D<float> img1{
            {1,1}, {3,2}
        };
        yagit::Image2D<float> img2{
            {2,1}, {2,3}
        };
        yagit::ImageData dr(img1, {0,0,0}, {0,2,2});
        yagit::ImageData de(img2, {0,0,0}, {0,2,2});

        // expected:
        // [[2/3, 0], [2/3, 2/3]]
        std::cout << yagit::gammaIndex2D(dr, de, {3,3, yagit::GammaNormalization::Global, dr.max(), 0}).getImage2D(0);

        std::cout << "\n-----------------\n";

        img1 = {{0.93, 0.95}, {0.97, 1.00}};
        img2 = {{0.95, 0.97}, {1.00, 1.03}};
        yagit::ImageData dr2(img1, {0,0,-1}, {0,1,1});
        yagit::ImageData de2(img2, {0,-1,0}, {0,1,1});
        // expected:
        // [[0.816496, 0.333333], [0.942809, 0.333333]]
        std::cout << yagit::gammaIndex2D(dr2, de2, {3,3, yagit::GammaNormalization::Global, dr2.max(), 0}).getImage2D(0);


        std::cout << "==================================================\n";
        std::cout << "MIN, MAX, MEAN AND OTHER\n";

        printVecInfo({2, 5, 4, std::numeric_limits<float>::quiet_NaN()});
        printVecInfo({2, std::numeric_limits<float>::quiet_NaN(), 8, 1});
        printVecInfo({std::numeric_limits<float>::quiet_NaN(), 2, 5, 4});
        printVecInfo({std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN()});
        printVecInfo({std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN(),3});
        printVecInfo({2, 5, 4, std::numeric_limits<float>::infinity()});

        std::cout << "\n-----------------\n";

        yagit::ImageData statsData(std::vector<float>{2,7,3,9,4,1,0,4,3,6,10,2,6,1,5}, {1,1,15}, {0,0,0}, {1,1,1});
        printDataStatsInfo(statsData);

        std::cout << "\n-----------------\n";

        const float nan = std::numeric_limits<float>::quiet_NaN();
        yagit::ImageData statsData2(std::vector<float>{nan,2,7,3,9,4,1,0,nan,4,3,6,10,2,6,1,5,nan}, {1,1,15+3}, {0,0,0}, {1,1,1});
        printDataStatsInfo(statsData2);
        std::cout << "\n-------\n";
        printDataStatsNanInfo(statsData2);

        std::cout << "\n-----------------\n";
        const float inf = std::numeric_limits<float>::infinity();
        std::cout << std::isinf(inf) << std::isinf(-inf) << std::isinf(nan) << std::isinf(2.0f) << "\n";

    }
    catch(const std::exception& e){
        std::cerr << "EXCEPTION: " << e.what() <<std::endl;
    }
}
