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

#include <iostream>
#include <iomanip>
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
    // for(uint32_t i = 0; i < imageData.getSize().frames; i++){
    //     printImage2D(imageData.getImage2D(i));
    // }

    yagit::DataSize size = imageData.getSize();
    yagit::DataOffset offset = imageData.getOffset();
    yagit::DataSpacing spacing = imageData.getSpacing();
    std::cout << "size: (" << size.frames << ", " << size.rows << ", " << size.columns << ")\n"
                << "offset: (" << offset.frames << ", " << offset.rows << ", " << offset.columns << ")\n"
                << "spacing: (" << spacing.frames << ", " << spacing.rows << ", " << spacing.columns << ")\n";
}

int main(){
    // yagit::Image3D<float> img3d = {
    //     {
    //         {1,2},
    //         {0,0}
    //     },
    //     {
    //         {0,1},
    //         {2,2}
    //     },
    //     {
    //         {0.1, 0.2},
    //         {0.3, 0.4}
    //     }
    // };

    // yagit::ImageData img(img3d, {2,3,4}, {1,1,1});
    // yagit::DataWriter::writeToMetaImage(img, "image.mha");

    try{
        yagit::ImageData d = yagit::DataReader::readMetaImage("image.mha", true);
        std::cout << "---------------------\n";
        printImageData(d);
    }
    catch(const std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
