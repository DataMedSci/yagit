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

template <typename T>
void printDataInfo(const yagit::ImageData<T>& data){
    yagit::DataSize size = data.getSize();
    yagit::DataOffset offset = data.getOffset();
    yagit::DataSpacing spacing = data.getSpacing();

    std::cout << "size(" << size.frames << ", " << size.rows << ", " << size.columns << "); "
              << "offset(" << offset.framesOffset << ", " << offset.rowsOffset << ", " << offset.columnsOffset << "); "
              << "spacing(" << spacing.framesSpacing << ", " << spacing.rowsSpacing << ", " << spacing.columnsSpacing << ")\n";
    std::cout << data.getData() << "\n";
    std::cout << "-----------------------------\n";
}

int main(){
    std::cout << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ\n";
    std::vector<float> img{0,1,2,3,4,5};
    yagit::DoseData d(img, yagit::DataSize(img.size(),1,1), {0,0,0}, {1,1,1});
    printDataInfo(d);

    yagit::DoseData di2 = yagit::Interpolation::linear(d, 0.5, yagit::ImageAxis::Z);
    printDataInfo(di2);
    yagit::DoseData di3 = yagit::Interpolation::linear(d, 1/3.f, yagit::ImageAxis::Z);
    printDataInfo(di3);
    yagit::DoseData di03 = yagit::Interpolation::linear(d, 0.3, yagit::ImageAxis::Z);
    printDataInfo(di03);
    yagit::DoseData dib = yagit::Interpolation::linear(d, 2, yagit::ImageAxis::Z);
    printDataInfo(dib);

    std::cout << "===========================================\n";

    std::vector<float> img2{8,23,5,1};
    yagit::DoseData d2(img2, yagit::DataSize(img2.size(),1,1), {0,0,0}, {1,1,1});
    printDataInfo(d2);
    yagit::DoseData d2i = yagit::Interpolation::linear(d2, 0.4, yagit::ImageAxis::Z);
    printDataInfo(d2i);
    yagit::DoseData d2i20 = yagit::Interpolation::linear(d2, 20, yagit::ImageAxis::Z);
    printDataInfo(d2i20);

    std::cout << "===========================================\n";
    std::cout << "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n";

    d.setSize({1,uint32_t(img.size()),1});
    printDataInfo(d);

    yagit::DoseData di2y = yagit::Interpolation::linear(d, 0.5, yagit::ImageAxis::Y);
    printDataInfo(di2y);
    yagit::DoseData di3y = yagit::Interpolation::linear(d, 1/3.f, yagit::ImageAxis::Y);
    printDataInfo(di3y);
    yagit::DoseData di03y = yagit::Interpolation::linear(d, 0.3, yagit::ImageAxis::Y);
    printDataInfo(di03y);
    yagit::DoseData diby = yagit::Interpolation::linear(d, 2, yagit::ImageAxis::Y);
    printDataInfo(diby);

    std::cout << "===========================================\n";

    d2.setSize({1,uint32_t(img2.size()),1});
    printDataInfo(d2);

    yagit::DoseData d2iy = yagit::Interpolation::linear(d2, 0.4, yagit::ImageAxis::Y);
    printDataInfo(d2iy);
    yagit::DoseData d2i20y = yagit::Interpolation::linear(d2, 20, yagit::ImageAxis::Y);
    printDataInfo(d2i20y);

    std::cout << "===========================================\n";
    std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";

    d.setSize({1,1,uint32_t(img.size())});
    printDataInfo(d);

    yagit::DoseData di2x = yagit::Interpolation::linear(d, 0.5, yagit::ImageAxis::X);
    printDataInfo(di2x);
    yagit::DoseData di3x = yagit::Interpolation::linear(d, 1/3.f, yagit::ImageAxis::X);
    printDataInfo(di3x);
    yagit::DoseData di03x = yagit::Interpolation::linear(d, 0.3, yagit::ImageAxis::X);
    printDataInfo(di03x);
    yagit::DoseData dibx = yagit::Interpolation::linear(d, 2, yagit::ImageAxis::X);
    printDataInfo(dibx);

    std::cout << "===========================================\n";

    d2.setSize({1,1,uint32_t(img2.size())});
    printDataInfo(d2);

    yagit::DoseData d2ix = yagit::Interpolation::linear(d2, 0.4, yagit::ImageAxis::X);
    printDataInfo(d2ix);
    yagit::DoseData d2i20x = yagit::Interpolation::linear(d2, 20, yagit::ImageAxis::X);
    printDataInfo(d2i20x);

    std::cout << "===========================================\n";
    std::cout << "OFFSET\n";
    std::cout << "ZZZZZZZZZZZZ\n";

    d.setSize({uint32_t(img.size()),1,1});
    printDataInfo(d);

    auto doi = yagit::Interpolation::linear(d, 0.1, 0.5, yagit::ImageAxis::Z);
    printDataInfo(doi);

    d2.setSize({uint32_t(img2.size()),1,1});
    printDataInfo(d2);

    auto doi2 = yagit::Interpolation::linear(d2, 0.1, 0.5, yagit::ImageAxis::Z);
    printDataInfo(doi2);

    std::cout << "YYYYYYYYYYYY\n";

    d.setSize({1,uint32_t(img.size()),1});
    printDataInfo(d);

    doi = yagit::Interpolation::linear(d, 0.1, 0.5, yagit::ImageAxis::Y);
    printDataInfo(doi);

    d2.setSize({1,uint32_t(img2.size()),1});
    printDataInfo(d2);

    doi2 = yagit::Interpolation::linear(d2, 0.1, 0.5, yagit::ImageAxis::Y);
    printDataInfo(doi2);

    std::cout << "XXXXXXXXXXXX\n";

    d.setSize({1,1,uint32_t(img.size())});
    printDataInfo(d);

    doi = yagit::Interpolation::linear(d, 0.1, 0.5, yagit::ImageAxis::X);
    printDataInfo(doi);

    d2.setSize({1,1,uint32_t(img2.size())});
    printDataInfo(d2);

    doi2 = yagit::Interpolation::linear(d2, 0.1, 0.5, yagit::ImageAxis::X);
    printDataInfo(doi2);
}
