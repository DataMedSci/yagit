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

#include "Interpolation.hpp"

#include <cmath>
#include <stdexcept>

namespace yagit::Interpolation{

ImageData linearAlongAxis(const ImageData& img, float spacing, ImageAxis axis){
    if(axis == ImageAxis::Z){
        float oldSpacing = img.getSpacing().frames;
        if(spacing == oldSpacing){
            return img;
        }
        
        uint32_t newSize = static_cast<uint32_t>(oldSpacing * (img.getSize().frames - 1) / spacing) + 1;
        Image3D<float> newImg(newSize, Image2D<float>(img.getSize().rows, std::vector<float>(img.getSize().columns, 0.0f)));

        for(uint32_t j = 0; j < img.getSize().rows; j++){
            for(uint32_t i = 0; i < img.getSize().columns; i++){
                float z = 0;
                for(uint32_t k = 0; k < newSize; k++){
                    const uint32_t ind1 = static_cast<uint32_t>(z / oldSpacing);
                    const uint32_t ind2 = ind1 + 1;
                    const float val1 = img.get(ind1, j, i);
                    if(ind2 < img.getSize().frames){
                        const float val2 = img.get(ind2, j, i);
                        newImg[k][j][i] = val1 + (z - ind1 * oldSpacing) * (val2 - val1) / oldSpacing;
                    }
                    else{
                        newImg[k][j][i] = val1;
                    }
                    z += spacing;
                }
            }
        }
        DataSpacing newSpacing{spacing, img.getSpacing().rows, img.getSpacing().columns};
        return ImageData(newImg, img.getOffset(), newSpacing);
    }
    else if(axis == ImageAxis::Y){
        float oldSpacing = img.getSpacing().rows;
        if(spacing == oldSpacing){
            return img;
        }

        uint32_t newSize = static_cast<uint32_t>(oldSpacing * (img.getSize().rows - 1) / spacing) + 1;
        Image3D<float> newImg(img.getSize().frames, Image2D<float>(newSize, std::vector<float>(img.getSize().columns, 0.0f)));

        for(uint32_t k = 0; k < img.getSize().frames; k++){
            for(uint32_t i = 0; i < img.getSize().columns; i++){
                float y = 0;
                for(uint32_t j = 0; j < newSize; j++){
                    const uint32_t ind1 = static_cast<uint32_t>(y / oldSpacing);
                    const uint32_t ind2 = ind1 + 1;
                    const float val1 = img.get(k, ind1, i);
                    if(ind2 < img.getSize().rows){
                        const float val2 = img.get(k, ind2, i);
                        newImg[k][j][i] = val1 + (y - ind1 * oldSpacing) * (val2 - val1) / oldSpacing;
                    }
                    else{
                        newImg[k][j][i] = val1;
                    }
                    y += spacing;
                }
            }
        }
        DataSpacing newSpacing{img.getSpacing().frames, spacing, img.getSpacing().columns};
        return ImageData(newImg, img.getOffset(), newSpacing);
    }
    else if(axis == ImageAxis::X){
        float oldSpacing = img.getSpacing().columns;
        if(spacing == oldSpacing){
            return img;
        }

        uint32_t newSize = static_cast<uint32_t>(oldSpacing * (img.getSize().columns - 1) / spacing) + 1;
        Image3D<float> newImg(img.getSize().frames, Image2D<float>(img.getSize().rows, std::vector<float>(newSize, 0.0f)));

        for(uint32_t k = 0; k < img.getSize().frames; k++){
            for(uint32_t j = 0; j < img.getSize().rows; j++){
                float x = 0;
                for(uint32_t i = 0; i < newSize; i++){
                    const uint32_t ind1 = static_cast<uint32_t>(x / oldSpacing);
                    const uint32_t ind2 = ind1 + 1;
                    const float val1 = img.get(k, j, ind1);
                    if(ind2 < img.getSize().columns){
                        const float val2 = img.get(k, j, ind2);
                        newImg[k][j][i] = val1 + (x - ind1 * oldSpacing) * (val2 - val1) / oldSpacing;
                    }
                    else{
                        newImg[k][j][i] = val1;
                    }
                    x += spacing;
                }
            }
        }
        DataSpacing newSpacing{img.getSpacing().frames, img.getSpacing().rows, spacing};
        return ImageData(newImg, img.getOffset(), newSpacing);
    }
    else{
        throw std::invalid_argument("invalid axis");
    }
}

ImageData bilinearOnPlane(const ImageData& img, float firstAxisSpacing, float secondAxisSpacing, ImagePlane plane){
    if(plane == ImagePlane::YX){
        return linearAlongAxis(linearAlongAxis(img, firstAxisSpacing, ImageAxis::Y), secondAxisSpacing, ImageAxis::X);
    }
    else if(plane == ImagePlane::ZX){
        return linearAlongAxis(linearAlongAxis(img, firstAxisSpacing, ImageAxis::Z), secondAxisSpacing, ImageAxis::X);
    }
    else if(plane == ImagePlane::ZY){
        return linearAlongAxis(linearAlongAxis(img, firstAxisSpacing, ImageAxis::Z), secondAxisSpacing, ImageAxis::Y);
    }
    else{
        throw std::invalid_argument("invalid plane");
    }
}

ImageData trilinear(const ImageData& img, const DataSpacing& spacing){
    return linearAlongAxis(
        linearAlongAxis(
            linearAlongAxis(
                img, spacing.frames, ImageAxis::Z),
            spacing.rows, ImageAxis::Y),
        spacing.columns, ImageAxis::X);
}

ImageData linearAlongAxis(const ImageData& img, float offset, float spacing, ImageAxis axis){
    if(axis == ImageAxis::Z){
        // closest point greater than img_offset which is one of points (offset +/- n*spacing)
        float zOffset = offset + std::ceil((img.getOffset().frames - offset) / spacing) * spacing;
        // then this point is reduced by img_offset to be relative to point 0
        float zOffsetRel = zOffset - img.getOffset().frames;

        float oldSpacing = img.getSpacing().frames;
        if(zOffsetRel == 0 && spacing == oldSpacing){
            return img;
        }

        // number of interpolated points that lie on edges or in the middle of original range
        uint32_t newSize = static_cast<uint32_t>((oldSpacing * (img.getSize().frames - 1) - zOffsetRel) / spacing) + 1;
        Image3D<float> newImg(newSize, Image2D<float>(img.getSize().rows, std::vector<float>(img.getSize().columns, 0.0f)));

        for(uint32_t j = 0; j < img.getSize().rows; j++){
            for(uint32_t i = 0; i < img.getSize().columns; i++){
                float z = zOffsetRel;
                for(uint32_t k = 0; k < newSize; k++){
                    const uint32_t ind1 = static_cast<uint32_t>(z / oldSpacing);
                    const uint32_t ind2 = ind1 + 1;
                    const float val1 = img.get(ind1, j, i);
                    if(ind2 < img.getSize().frames){
                        const float val2 = img.get(ind2, j, i);
                        newImg[k][j][i] = val1 + (z - ind1 * oldSpacing) * (val2 - val1) / oldSpacing;
                    }
                    else{
                        newImg[k][j][i] = val1;
                    }
                    z += spacing;
                }
            }
        }
        DataOffset newOffset{zOffset, img.getOffset().rows, img.getOffset().columns};
        DataSpacing newSpacing{spacing, img.getSpacing().rows, img.getSpacing().columns};
        return ImageData(newImg, newOffset, newSpacing);
    }
    else if(axis == ImageAxis::Y){
        // closest point greater than img_offset which is one of points (offset +/- n*spacing)
        float yOffset = offset + std::ceil((img.getOffset().rows - offset) / spacing) * spacing;
        // then this point is reduced by img_offset to be relative to point 0
        float yOffsetRel = yOffset - img.getOffset().rows;

        float oldSpacing = img.getSpacing().rows;
        if(yOffsetRel == 0 && spacing == oldSpacing){
            return img;
        }

        // number of interpolated points that lie on edges or in the middle of original range
        uint32_t newSize = static_cast<uint32_t>((oldSpacing * (img.getSize().rows - 1) - yOffsetRel) / spacing) + 1;
        Image3D<float> newImg(img.getSize().frames, Image2D<float>(newSize, std::vector<float>(img.getSize().columns, 0.0f)));

        for(uint32_t k = 0; k < img.getSize().frames; k++){
            for(uint32_t i = 0; i < img.getSize().columns; i++){
                float y = yOffsetRel;
                for(uint32_t j = 0; j < newSize; j++){
                    const uint32_t ind1 = static_cast<uint32_t>(y / oldSpacing);
                    const uint32_t ind2 = ind1 + 1;
                    const float val1 = img.get(k, ind1, i);
                    if(ind2 < img.getSize().rows){
                        const float val2 = img.get(k, ind2, i);
                        newImg[k][j][i] = val1 + (y - ind1 * oldSpacing) * (val2 - val1) / oldSpacing;
                    }
                    else{
                        newImg[k][j][i] = val1;
                    }
                    y += spacing;
                }
            }
        }
        DataOffset newOffset{img.getOffset().frames, yOffset, img.getOffset().columns};
        DataSpacing newSpacing{img.getSpacing().frames, spacing, img.getSpacing().columns};
        return ImageData(newImg, newOffset, newSpacing);
    }
    else if(axis == ImageAxis::X){
        // closest point greater than img_offset which is one of points (offset +/- n*spacing)
        float xOffset = offset + std::ceil((img.getOffset().columns - offset) / spacing) * spacing;
        // then this point is reduced by img_offset to be relative to point 0
        float xOffsetRel = xOffset - img.getOffset().columns;

        float oldSpacing = img.getSpacing().columns;
        if(xOffsetRel == 0 && spacing == oldSpacing){
            return img;
        }

        // number of interpolated points that lie on edges or in the middle of original range
        uint32_t newSize = static_cast<uint32_t>((oldSpacing * (img.getSize().columns - 1) - xOffsetRel) / spacing) + 1;
        Image3D<float> newImg(img.getSize().frames, Image2D<float>(img.getSize().rows, std::vector<float>(newSize, 0.0f)));

        for(uint32_t k = 0; k < img.getSize().frames; k++){
            for(uint32_t j = 0; j < img.getSize().rows; j++){
                float x = xOffsetRel;
                for(uint32_t i = 0; i < newSize; i++){
                    const uint32_t ind1 = static_cast<uint32_t>(x / oldSpacing);
                    const uint32_t ind2 = ind1 + 1;
                    const float val1 = img.get(k, j, ind1);
                    if(ind2 < img.getSize().columns){
                        const float val2 = img.get(k, j, ind2);
                        newImg[k][j][i] = val1 + (x - ind1 * oldSpacing) * (val2 - val1) / oldSpacing;
                    }
                    else{
                        newImg[k][j][i] = val1;
                    }
                    x += spacing;
                }
            }
        }
        DataOffset newOffset{img.getOffset().frames, img.getOffset().rows, xOffset};
        DataSpacing newSpacing{img.getSpacing().frames, img.getSpacing().rows, spacing};
        return ImageData(newImg, newOffset, newSpacing);
    }
    else{
        throw std::invalid_argument("invalid axis");
    }
}

ImageData bilinearOnPlane(const ImageData& img, float firstAxisOffset, float secondAxisOffset,
                         float firstAxisSpacing, float secondAxisSpacing, ImagePlane plane){
    if(plane == ImagePlane::YX){
        return linearAlongAxis(
            linearAlongAxis(
                img, firstAxisOffset, firstAxisSpacing, ImageAxis::Y),
            secondAxisOffset, secondAxisSpacing, ImageAxis::X);
    }
    else if(plane == ImagePlane::ZX){
        return linearAlongAxis(
            linearAlongAxis(
                img, firstAxisOffset, firstAxisSpacing, ImageAxis::Z),
            secondAxisOffset, secondAxisSpacing, ImageAxis::X);
    }
    else if(plane == ImagePlane::ZY){
        return linearAlongAxis(
            linearAlongAxis(
                img, firstAxisOffset, firstAxisSpacing, ImageAxis::Z),
            secondAxisOffset, secondAxisSpacing, ImageAxis::Y);
    }
    else{
        throw std::invalid_argument("invalid plane");
    }
}

ImageData trilinear(const ImageData& img, const DataOffset& offset, const DataSpacing& spacing){
    return linearAlongAxis(
        linearAlongAxis(
            linearAlongAxis(
                img, offset.frames, spacing.frames, ImageAxis::Z),
            offset.rows, spacing.rows, ImageAxis::Y),
        offset.columns, spacing.columns, ImageAxis::X);

}

ImageData linearAlongAxis(const ImageData& evalImg, const ImageData& refImg, ImageAxis axis){
    float offset = 0;
    float spacing = 0;
    if(axis == ImageAxis::Z){
        offset = refImg.getOffset().frames;
        spacing = refImg.getSpacing().frames;
    }
    else if(axis == ImageAxis::Y){
        offset = refImg.getOffset().rows;
        spacing = refImg.getSpacing().rows;
    }
    else if(axis == ImageAxis::X){
        offset = refImg.getOffset().columns;
        spacing = refImg.getSpacing().columns;
    }
    else{
        throw std::invalid_argument("invalid axis");
    }
    return linearAlongAxis(evalImg, offset, spacing, axis);
}

ImageData bilinearOnPlane(const ImageData& evalImg, const ImageData& refImg, ImagePlane plane){
    float offset1 = 0, offset2 = 0;
    float spacing1 = 0, spacing2 = 0;
    if(plane == ImagePlane::YX){
        offset1 = refImg.getOffset().rows;
        offset2 = refImg.getOffset().columns;
        spacing1 = refImg.getSpacing().rows;
        spacing2 = refImg.getSpacing().columns;
    }
    else if(plane == ImagePlane::ZX){
        offset1 = refImg.getOffset().frames;
        offset2 = refImg.getOffset().columns;
        spacing1 = refImg.getSpacing().frames;
        spacing2 = refImg.getSpacing().columns;
    }
    else if(plane == ImagePlane::ZY){
        offset1 = refImg.getOffset().frames;
        offset2 = refImg.getOffset().rows;
        spacing1 = refImg.getSpacing().frames;
        spacing2 = refImg.getSpacing().rows;
    }
    else{
        throw std::invalid_argument("invalid plane");
    }
    return bilinearOnPlane(evalImg, offset1, offset2, spacing1, spacing2, plane);
}

ImageData trilinear(const ImageData& evalImg, const ImageData& refImg){
    return trilinear(evalImg, refImg.getOffset(), refImg.getSpacing());
}

std::optional<float> bilinearAtPoint(const ImageData& img, uint32_t frame, float y, float x){
    const int32_t indy1 = std::floor((y - img.getOffset().rows) / img.getSpacing().rows);
    const int32_t indx1 = std::floor((x - img.getOffset().columns) / img.getSpacing().columns);
    int32_t indy2 = indy1 + 1;
    int32_t indx2 = indx1 + 1;
    if(indy1 >= 0 && indy2 <= static_cast<int32_t>(img.getSize().rows) &&
       indx1 >= 0 && indx2 <= static_cast<int32_t>(img.getSize().columns)){

        if(indy2 == static_cast<int32_t>(img.getSize().rows)){
            indy2 = indy1;
        }
        if(indx2 == static_cast<int32_t>(img.getSize().columns)){
            indx2 = indx1;
        }

        float y1 = img.getOffset().rows + indy1 * img.getSpacing().rows;
        float x1 = img.getOffset().columns + indx1 * img.getSpacing().columns;
        float y2 = y1 + (indy2 - indy1) * img.getSpacing().rows;
        float x2 = x1 + (indx2 - indx1) * img.getSpacing().columns;

        float interpFactor = 1 / (img.getSpacing().rows * img.getSpacing().columns);

        return interpFactor * (
            img.get(frame, indy1, indx1) * (x2 - x) * (y2 - y) +
            img.get(frame, indy1, indx2) * (x - x1) * (y2 - y) +
            img.get(frame, indy2, indx1) * (x2 - x) * (y - y1) +
            img.get(frame, indy2, indx2) * (x - x1) * (y - y1));
    }
    return std::nullopt;
}

std::optional<float> trilinearAtPoint(const ImageData& img, float z, float y, float x){
    const int32_t indz0 = std::floor((z - img.getOffset().frames) / img.getSpacing().frames);
    const int32_t indy0 = std::floor((y - img.getOffset().rows) / img.getSpacing().rows);
    const int32_t indx0 = std::floor((x - img.getOffset().columns) / img.getSpacing().columns);
    int32_t indz1 = indz0 + 1;
    int32_t indy1 = indy0 + 1;
    int32_t indx1 = indx0 + 1;

    if(indz0 >= 0 && indz1 <= static_cast<int32_t>(img.getSize().frames) &&
       indy0 >= 0 && indy1 <= static_cast<int32_t>(img.getSize().rows) &&
       indx0 >= 0 && indx1 <= static_cast<int32_t>(img.getSize().columns)){

        if(indz1 == static_cast<int32_t>(img.getSize().frames)){
            indz1 = indz0;
        }
        if(indy1 == static_cast<int32_t>(img.getSize().rows)){
            indy1 = indy0;
        }
        if(indx1 == static_cast<int32_t>(img.getSize().columns)){
            indx1 = indx0;
        }

        float z0 = img.getOffset().frames + indz0 * img.getSpacing().frames;
        float y0 = img.getOffset().rows + indy0 * img.getSpacing().rows;
        float x0 = img.getOffset().columns + indx0 * img.getSpacing().columns;

        float c000 = img.get(indz0, indy0, indx0);
        float c001 = img.get(indz1, indy0, indx0);
        float c010 = img.get(indz0, indy1, indx0);
        float c011 = img.get(indz1, indy1, indx0);
        float c100 = img.get(indz0, indy0, indx1);
        float c101 = img.get(indz1, indy0, indx1);
        float c110 = img.get(indz0, indy1, indx1);
        float c111 = img.get(indz1, indy1, indx1);

        float zd = (z - z0) / img.getSpacing().frames;
        float yd = (y - y0) / img.getSpacing().rows;
        float xd = (x - x0) / img.getSpacing().columns;

        float c00 = c000*(1 - xd) + c100*xd;
        float c01 = c001*(1 - xd) + c101*xd;
        float c10 = c010*(1 - xd) + c110*xd;
        float c11 = c011*(1 - xd) + c111*xd;

        float c0 = c00*(1 - yd) + c10*yd;
        float c1 = c01*(1 - yd) + c11*yd;

        return c0*(1 - zd) + c1*zd;
    }
    return std::nullopt;
}

}
