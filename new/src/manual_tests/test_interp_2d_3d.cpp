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

#include "Interpolation.hpp"
#include "DataReader.hpp"

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

void printImage3D(const yagit::Image3D<float>& img){
    for(const auto& v : img){
        if(img.size() > 1){
            std::cout << "[\n";
        }
        for(const auto& v2 : v){
            std::cout << "[";
            for(size_t i = 0; i < v2.size()-1; i++){
                std::cout << v2[i] << ", ";
            }
            std::cout << v2.back() << "]\n";
        }
        if(img.size() > 1){
            std::cout << "]";
        }
    }
}

void printDataStats(const yagit::DoseData& data){
    yagit::DataSize size = data.getSize();
    yagit::DataOffset offset = data.getOffset();
    yagit::DataSpacing spacing = data.getSpacing();

    std::cout << "size(" << size.frames << ", " << size.rows << ", " << size.columns << "); "
              << "offset(" << offset.framesOffset << ", " << offset.rowsOffset << ", " << offset.columnsOffset << "); "
              << "spacing(" << spacing.framesSpacing << ", " << spacing.rowsSpacing << ", " << spacing.columnsSpacing << ")\n";
}

void printDataInfo(const yagit::DoseData& data){
    printDataStats(data);
    printImage3D(data.getImage3D());;
    std::cout << "-----------------------------\n";
}

int main(){
    yagit::Image2D<float> img{
        {0,1,2,3,4},
        {5,6,7,8,9},
        {10,11,12,13,14}
    };

    yagit::DoseData d(img, {0,0,0}, {1,1,1});
    printDataInfo(d);

    auto dyx = yagit::Interpolation::bilinearOnPlane(d, 0.5, 0.5, yagit::ImagePlane::Axial);
    printDataInfo(dyx);

    auto dzx = yagit::Interpolation::bilinearOnPlane(d, 0.5, 0.5, yagit::ImagePlane::Coronal);
    printDataInfo(dzx);

    auto dzy = yagit::Interpolation::bilinearOnPlane(d, 0.5, 0.5, yagit::ImagePlane::Sagittal);
    printDataInfo(dzy);

    std::cout << "_______________\n";

    printDataInfo(yagit::Interpolation::linearAlongAxis(d, 0.1, 0.5, yagit::ImageAxis::Y));
    printDataInfo(yagit::Interpolation::linearAlongAxis(d, 0.1, 0.5, yagit::ImageAxis::X));

    printDataInfo(yagit::Interpolation::bilinearOnPlane(d, 0.1, 0.1, 0.5, 0.5, yagit::ImagePlane::Axial));
    printDataInfo(yagit::Interpolation::bilinearOnPlane(d, 0.1, 0.1, 0.5, 0.5, yagit::ImagePlane::Coronal));
    printDataInfo(yagit::Interpolation::bilinearOnPlane(d, 0.1, 0.1, 0.5, 0.5, yagit::ImagePlane::Sagittal));

    std::cout << "============================================\n";

    yagit::Image3D<float> img3d{
        {
            {0,1,2,3},
            {4,5,6,7},
            {8,9,10,11}
        },{
            {12,13,14,15},
            {16,17,18,19},
            {20,21,22,23}
        }
    };

    yagit::DoseData d3d(img3d, {0,0,0}, {1,1,1});
    printDataInfo(d3d);

    printDataInfo(yagit::Interpolation::trilinear(d3d, {0.5,0.5,0.5}));

    std::cout << "_______________\n";

    printDataInfo(yagit::Interpolation::trilinear(d3d, {0.1,0.1,0.1}, {0.5,0.5,0.5}));

    std::cout << "============================================\n";
    const std::string refImgFilename{"original_dose_beam_4.dcm"};
    yagit::DoseData refImg = yagit::DataReader::readRTDoseDicom(refImgFilename);
    
    printDataStats(refImg);

    // printDataStats(yagit::Interpolation::linearAlongAxis(refImg, 1, yagit::ImageAxis::Z));
    // printDataStats(yagit::Interpolation::trilinear(refImg, {1,1,1}));
    printDataStats(yagit::Interpolation::trilinear(refImg, {0.1,0.1,0.1}, {1,1,1}));

    std::cout << "============================================\n";
    const std::string evalImgFilename{"logfile_dose_beam_4.dcm"};
    yagit::DoseData evalImg = yagit::DataReader::readRTDoseDicom(evalImgFilename);
    auto eOffset = evalImg.getOffset();
    evalImg.setOffset(yagit::DataOffset(eOffset.framesOffset + 0.3, eOffset.rowsOffset - 0.2, eOffset.columnsOffset + 0.1));
    printDataStats(refImg);
    printDataStats(evalImg);
    printDataStats(yagit::Interpolation::trilinear(evalImg, refImg));
}
