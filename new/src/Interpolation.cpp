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

#include "Interpolation.hpp"

#include <cmath>
#include <stdexcept>

namespace yagit::Interpolation{

ImageData linearAlongAxis(const ImageData& img, float spacing, ImageAxis axis){
    if(axis == ImageAxis::Z){
        float oldSpacing = img.getSpacing().framesSpacing;
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
        DataSpacing newSpacing{spacing, img.getSpacing().rowsSpacing, img.getSpacing().columnsSpacing};
        return ImageData(newImg, img.getOffset(), newSpacing);
    }
    else if(axis == ImageAxis::Y){
        float oldSpacing = img.getSpacing().rowsSpacing;
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
        DataSpacing newSpacing{img.getSpacing().framesSpacing, spacing, img.getSpacing().columnsSpacing};
        return ImageData(newImg, img.getOffset(), newSpacing);
    }
    else if(axis == ImageAxis::X){
        float oldSpacing = img.getSpacing().columnsSpacing;
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
        DataSpacing newSpacing{img.getSpacing().framesSpacing, img.getSpacing().rowsSpacing, spacing};
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
                img, spacing.framesSpacing, ImageAxis::Z),
            spacing.rowsSpacing, ImageAxis::Y),
        spacing.columnsSpacing, ImageAxis::X);
}

ImageData linearAlongAxis(const ImageData& img, float offset, float spacing, ImageAxis axis){
    if(axis == ImageAxis::Z){
        // closest point greater than img_offset which is one of points (offset +/- n*spacing)
        float zOffset = offset + std::ceil((img.getOffset().framesOffset - offset) / spacing) * spacing;
        // then this point is reduced by img_offset to be relative to point 0
        float zOffsetRel = zOffset - img.getOffset().framesOffset;

        float oldSpacing = img.getSpacing().framesSpacing;
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
        DataOffset newOffset{zOffset, img.getOffset().rowsOffset, img.getOffset().columnsOffset};
        DataSpacing newSpacing{spacing, img.getSpacing().rowsSpacing, img.getSpacing().columnsSpacing};
        return ImageData(newImg, newOffset, newSpacing);
    }
    else if(axis == ImageAxis::Y){
        // closest point greater than img_offset which is one of points (offset +/- n*spacing)
        float yOffset = offset + std::ceil((img.getOffset().rowsOffset - offset) / spacing) * spacing;
        // then this point is reduced by img_offset to be relative to point 0
        float yOffsetRel = yOffset - img.getOffset().rowsOffset;

        float oldSpacing = img.getSpacing().rowsSpacing;
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
        DataOffset newOffset{img.getOffset().framesOffset, yOffset, img.getOffset().columnsOffset};
        DataSpacing newSpacing{img.getSpacing().framesSpacing, spacing, img.getSpacing().columnsSpacing};
        return ImageData(newImg, newOffset, newSpacing);
    }
    else if(axis == ImageAxis::X){
        // closest point greater than img_offset which is one of points (offset +/- n*spacing)
        float xOffset = offset + std::ceil((img.getOffset().columnsOffset - offset) / spacing) * spacing;
        // then this point is reduced by img_offset to be relative to point 0
        float xOffsetRel = xOffset - img.getOffset().columnsOffset;

        float oldSpacing = img.getSpacing().columnsSpacing;
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
        DataOffset newOffset{img.getOffset().framesOffset, img.getOffset().rowsOffset, xOffset};
        DataSpacing newSpacing{img.getSpacing().framesSpacing, img.getSpacing().rowsSpacing, spacing};
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
                img, offset.framesOffset, spacing.framesSpacing, ImageAxis::Z),
            offset.rowsOffset, spacing.rowsSpacing, ImageAxis::Y),
        offset.columnsOffset, spacing.columnsSpacing, ImageAxis::X);

}

ImageData linearAlongAxis(const ImageData& evalImg, const ImageData& refImg, ImageAxis axis){
    float offset = 0;
    float spacing = 0;
    if(axis == ImageAxis::Z){
        offset = refImg.getOffset().framesOffset;
        spacing = refImg.getSpacing().framesSpacing;
    }
    else if(axis == ImageAxis::Y){
        offset = refImg.getOffset().rowsOffset;
        spacing = refImg.getSpacing().rowsSpacing;
    }
    else if(axis == ImageAxis::X){
        offset = refImg.getOffset().columnsOffset;
        spacing = refImg.getSpacing().columnsSpacing;
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
        offset1 = refImg.getOffset().rowsOffset;
        offset2 = refImg.getOffset().columnsOffset;
        spacing1 = refImg.getSpacing().rowsSpacing;
        spacing2 = refImg.getSpacing().columnsSpacing;
    }
    else if(plane == ImagePlane::ZX){
        offset1 = refImg.getOffset().framesOffset;
        offset2 = refImg.getOffset().columnsOffset;
        spacing1 = refImg.getSpacing().framesSpacing;
        spacing2 = refImg.getSpacing().columnsSpacing;
    }
    else if(plane == ImagePlane::ZY){
        offset1 = refImg.getOffset().framesOffset;
        offset2 = refImg.getOffset().rowsOffset;
        spacing1 = refImg.getSpacing().framesSpacing;
        spacing2 = refImg.getSpacing().rowsSpacing;
    }
    else{
        throw std::invalid_argument("invalid plane");
    }
    return bilinearOnPlane(evalImg, offset1, offset2, spacing1, spacing2, plane);
}

ImageData trilinear(const ImageData& evalImg, const ImageData& refImg){
    return trilinear(evalImg, refImg.getOffset(), refImg.getSpacing());
}

}
