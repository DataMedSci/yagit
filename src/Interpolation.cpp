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

#include "yagit/Interpolation.hpp"

#include <cmath>
#include <stdexcept>

namespace yagit::Interpolation{

namespace{
const double Tolerance{3e-6};

float calcNewOffset(float oldOffset, float gridOffset, float spacing){
    // calculate closest point to oldOffset that is greater than or equal to oldOffset and also lies on grid
    int n = std::ceil((oldOffset - gridOffset) / static_cast<double>(spacing) - Tolerance);
    float newOffset = gridOffset + n * spacing;
    return newOffset;
}

constexpr uint32_t calcNewSize(uint32_t oldSize, float oldSpacing, float offsetRel, float newSpacing){
    // calculate number of interpolated points that lie on edges or in the middle of original image
    return static_cast<uint32_t>((oldSpacing * (oldSize - 1) - offsetRel) / newSpacing + 1 + Tolerance);
}
}

ImageData linearAlongAxis(const ImageData& img, float spacing, ImageAxis axis){
    if(axis == ImageAxis::Z){
        return linearAlongAxis(img, img.getOffset().frames, spacing, axis);
    }
    else if(axis == ImageAxis::Y){
        return linearAlongAxis(img, img.getOffset().rows, spacing, axis);
    }
    else if(axis == ImageAxis::X){
        return linearAlongAxis(img, img.getOffset().columns, spacing, axis);
    }
    else{
        throw std::invalid_argument("invalid axis");
    }
}

ImageData linearAlongAxis(const ImageData& img, float gridOffset, float spacing, ImageAxis axis){
    if(spacing <= 0){
        throw std::invalid_argument("spacing should be greater than 0");
    }

    if(axis == ImageAxis::Z){
        float zOffset = calcNewOffset(img.getOffset().frames, gridOffset, spacing);
        // get relative point by reducing new offset by old offset
        float zOffsetRel = zOffset - img.getOffset().frames;

        float oldSpacing = img.getSpacing().frames;
        if(zOffsetRel == 0 && spacing == oldSpacing){
            return img;
        }

        uint32_t newSize = calcNewSize(img.getSize().frames, oldSpacing, zOffsetRel, spacing);
        Image3D newImg(newSize, Image2D(img.getSize().rows, std::vector<float>(img.getSize().columns, 0.0f)));

        if(newSize > 0){
            if(2.0 * spacing < oldSpacing){  // this version is faster for small spacing
                const uint32_t indStart = static_cast<uint32_t>(zOffsetRel / oldSpacing);

                for(uint32_t j = 0; j < img.getSize().rows; j++){
                    for(uint32_t i = 0; i < img.getSize().columns; i++){
                        float z = zOffsetRel;
                        float oldZ = indStart * oldSpacing;
                        uint32_t ind1 = indStart;
                        uint32_t ind2 = indStart + 1;

                        for(uint32_t k = 0; k < newSize;){
                            const float val1 = img.get(ind1, j, i);
                            if(ind2 < img.getSize().frames){
                                const float val2 = img.get(ind2, j, i);
                                const float slope = (val2 - val1) / oldSpacing;
                                while(z < oldZ + oldSpacing && k < newSize){
                                    newImg[k][j][i] = val1 + (z - oldZ) * slope;
                                    z += spacing;
                                    k++;
                                }
                            }
                            else{
                                newImg[k][j][i] = val1;
                                k++;
                            }
                            oldZ += oldSpacing;
                            ind1++;
                            ind2++;
                        }
                    }
                }
            }
            else{
                for(uint32_t j = 0; j < img.getSize().rows; j++){
                    for(uint32_t i = 0; i < img.getSize().columns; i++){
                        float z = zOffsetRel;
                        for(uint32_t k = 0; k < newSize; k++){
                            float temp = z / oldSpacing;
                            const uint32_t ind1 = static_cast<uint32_t>(temp);
                            const uint32_t ind2 = ind1 + 1;
                            const float val1 = img.get(ind1, j, i);
                            if(ind2 < img.getSize().frames){
                                const float val2 = img.get(ind2, j, i);
                                newImg[k][j][i] = val1 + (temp - ind1) * (val2 - val1);
                            }
                            else{
                                newImg[k][j][i] = val1;
                            }
                            z += spacing;
                        }
                    }
                }
            }
        }
        DataOffset newOffset{zOffset, img.getOffset().rows, img.getOffset().columns};
        DataSpacing newSpacing{spacing, img.getSpacing().rows, img.getSpacing().columns};
        return ImageData(newImg, newOffset, newSpacing);
    }
    else if(axis == ImageAxis::Y){
        float yOffset = calcNewOffset(img.getOffset().rows, gridOffset, spacing);
        // get relative point by reducing new offset by old offset
        float yOffsetRel = yOffset - img.getOffset().rows;

        float oldSpacing = img.getSpacing().rows;
        if(yOffsetRel == 0 && spacing == oldSpacing){
            return img;
        }

        uint32_t newSize = calcNewSize(img.getSize().rows, oldSpacing, yOffsetRel, spacing);
        Image3D newImg(img.getSize().frames, Image2D(newSize, std::vector<float>(img.getSize().columns, 0.0f)));

        if(newSize > 0){
            if(2.0 * spacing < oldSpacing){  // this version is faster for small spacing
                const uint32_t indStart = static_cast<uint32_t>(yOffsetRel / oldSpacing);

                for(uint32_t k = 0; k < img.getSize().frames; k++){
                    for(uint32_t i = 0; i < img.getSize().columns; i++){
                        float y = yOffsetRel;
                        float oldY = indStart * oldSpacing;
                        uint32_t ind1 = indStart;
                        uint32_t ind2 = indStart + 1;

                        for(uint32_t j = 0; j < newSize;){
                            const float val1 = img.get(k, ind1, i);
                            if(ind2 < img.getSize().rows){
                                const float val2 = img.get(k, ind2, i);
                                const float slope = (val2 - val1) / oldSpacing;
                                while(y < oldY + oldSpacing && j < newSize){
                                    newImg[k][j][i] = val1 + (y - oldY) * slope;
                                    y += spacing;
                                    j++;
                                }
                            }
                            else{
                                newImg[k][j][i] = val1;
                                j++;
                            }
                            oldY += oldSpacing;
                            ind1++;
                            ind2++;
                        }
                    }
                }
            }
            else{
                for(uint32_t k = 0; k < img.getSize().frames; k++){
                    for(uint32_t i = 0; i < img.getSize().columns; i++){
                        float y = yOffsetRel;
                        for(uint32_t j = 0; j < newSize; j++){
                            float temp = y / oldSpacing;
                            const uint32_t ind1 = static_cast<uint32_t>(temp);
                            const uint32_t ind2 = ind1 + 1;
                            const float val1 = img.get(k, ind1, i);
                            if(ind2 < img.getSize().rows){
                                const float val2 = img.get(k, ind2, i);
                                newImg[k][j][i] = val1 + (temp - ind1) * (val2 - val1);
                            }
                            else{
                                newImg[k][j][i] = val1;
                            }
                            y += spacing;
                        }
                    }
                }
            }
        }
        DataOffset newOffset{img.getOffset().frames, yOffset, img.getOffset().columns};
        DataSpacing newSpacing{img.getSpacing().frames, spacing, img.getSpacing().columns};
        return ImageData(newImg, newOffset, newSpacing);
    }
    else if(axis == ImageAxis::X){
        float xOffset = calcNewOffset(img.getOffset().columns, gridOffset, spacing);
        // get relative point by reducing new offset by old offset
        float xOffsetRel = xOffset - img.getOffset().columns;

        float oldSpacing = img.getSpacing().columns;
        if(xOffsetRel == 0 && spacing == oldSpacing){
            return img;
        }

        uint32_t newSize = calcNewSize(img.getSize().columns, oldSpacing, xOffsetRel, spacing);
        Image3D newImg(img.getSize().frames, Image2D(img.getSize().rows, std::vector<float>(newSize, 0.0f)));

        if(newSize > 0){
            if(2.0 * spacing < oldSpacing){  // this version is faster for small spacing
                const uint32_t indStart = static_cast<uint32_t>(xOffsetRel / oldSpacing);

                for(uint32_t k = 0; k < img.getSize().frames; k++){
                    for(uint32_t j = 0; j < img.getSize().rows; j++){
                        float x = xOffsetRel;
                        float oldX = indStart * oldSpacing;
                        uint32_t ind1 = indStart;
                        uint32_t ind2 = indStart + 1;

                        for(uint32_t i = 0; i < newSize;){
                            const float val1 = img.get(k, j, ind1);
                            if(ind2 < img.getSize().columns){
                                const float val2 = img.get(k, j, ind2);
                                const float slope = (val2 - val1) / oldSpacing;
                                while(x < oldX + oldSpacing && i < newSize){
                                    newImg[k][j][i] = val1 + (x - oldX) * slope;
                                    x += spacing;
                                    i++;
                                }
                            }
                            else{
                                newImg[k][j][i] = val1;
                                i++;
                            }
                            oldX += oldSpacing;
                            ind1++;
                            ind2++;
                        }
                    }
                }
            }
            else{
                for(uint32_t k = 0; k < img.getSize().frames; k++){
                    for(uint32_t j = 0; j < img.getSize().rows; j++){
                        float x = xOffsetRel;
                        for(uint32_t i = 0; i < newSize; i++){
                            float temp = x / oldSpacing;
                            const uint32_t ind1 = static_cast<uint32_t>(temp);
                            const uint32_t ind2 = ind1 + 1;
                            const float val1 = img.get(k, j, ind1);
                            if(ind2 < img.getSize().columns){
                                const float val2 = img.get(k, j, ind2);
                                newImg[k][j][i] = val1 + (temp - ind1) * (val2 - val1);
                            }
                            else{
                                newImg[k][j][i] = val1;
                            }
                            x += spacing;
                        }
                    }
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

ImageData linearAlongAxis(const ImageData& targetImg, const ImageData& refImg, ImageAxis axis){
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
    return linearAlongAxis(targetImg, offset, spacing, axis);
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

ImageData bilinearOnPlane(const ImageData& img, float firstAxisGridOffset, float secondAxisGridOffset,
                         float firstAxisSpacing, float secondAxisSpacing, ImagePlane plane){
    if(plane == ImagePlane::YX){
        return linearAlongAxis(
            linearAlongAxis(
                img, firstAxisGridOffset, firstAxisSpacing, ImageAxis::Y),
            secondAxisGridOffset, secondAxisSpacing, ImageAxis::X);
    }
    else if(plane == ImagePlane::ZX){
        return linearAlongAxis(
            linearAlongAxis(
                img, firstAxisGridOffset, firstAxisSpacing, ImageAxis::Z),
            secondAxisGridOffset, secondAxisSpacing, ImageAxis::X);
    }
    else if(plane == ImagePlane::ZY){
        return linearAlongAxis(
            linearAlongAxis(
                img, firstAxisGridOffset, firstAxisSpacing, ImageAxis::Z),
            secondAxisGridOffset, secondAxisSpacing, ImageAxis::Y);
    }
    else{
        throw std::invalid_argument("invalid plane");
    }
}

ImageData bilinearOnPlane(const ImageData& targetImg, const ImageData& refImg, ImagePlane plane){
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
    return bilinearOnPlane(targetImg, offset1, offset2, spacing1, spacing2, plane);
}

ImageData trilinear(const ImageData& img, const DataSpacing& spacing){
    return linearAlongAxis(
        linearAlongAxis(
            linearAlongAxis(
                img, spacing.frames, ImageAxis::Z),
            spacing.rows, ImageAxis::Y),
        spacing.columns, ImageAxis::X);
}

ImageData trilinear(const ImageData& img, const DataOffset& gridOffset, const DataSpacing& spacing){
    return linearAlongAxis(
        linearAlongAxis(
            linearAlongAxis(
                img, gridOffset.frames, spacing.frames, ImageAxis::Z),
            gridOffset.rows, spacing.rows, ImageAxis::Y),
        gridOffset.columns, spacing.columns, ImageAxis::X);

}

ImageData trilinear(const ImageData& targetImg, const ImageData& refImg){
    return trilinear(targetImg, refImg.getOffset(), refImg.getSpacing());
}

std::optional<float> bilinearAtPoint(const ImageData& img, uint32_t frame, float y, float x){
    if(frame >= img.getSize().frames){
        throw std::out_of_range("frame out of range (frame >= nr of frames)");
    }
    if(y < img.getOffset().rows || y > img.getOffset().rows + (img.getSize().rows - 1) * img.getSpacing().rows ||
       x < img.getOffset().columns || x > img.getOffset().columns + (img.getSize().columns - 1) * img.getSpacing().columns){
        return std::nullopt;
    }

    const uint32_t indy0 = static_cast<uint32_t>((y - img.getOffset().rows) / img.getSpacing().rows);
    const uint32_t indx0 = static_cast<uint32_t>((x - img.getOffset().columns) / img.getSpacing().columns);
    uint32_t indy1 = indy0 + 1;
    uint32_t indx1 = indx0 + 1;

    if(indy1 == img.getSize().rows){
        indy1 = indy0;
    }
    if(indx1 == img.getSize().columns){
        indx1 = indx0;
    }

    float y0 = img.getOffset().rows + indy0 * img.getSpacing().rows;
    float x0 = img.getOffset().columns + indx0 * img.getSpacing().columns;

    float c00 = img.get(frame, indy0, indx0);
    float c01 = img.get(frame, indy1, indx0);
    float c10 = img.get(frame, indy0, indx1);
    float c11 = img.get(frame, indy1, indx1);

    float yd = (y - y0) / img.getSpacing().rows;
    float xd = (x - x0) / img.getSpacing().columns;

    float c0 = c00*(1 - xd) + c10*xd;
    float c1 = c01*(1 - xd) + c11*xd;

    return c0*(1 - yd) + c1*yd;
}

std::optional<float> trilinearAtPoint(const ImageData& img, float z, float y, float x){
    if(z < img.getOffset().frames || z > img.getOffset().frames + (img.getSize().frames - 1) * img.getSpacing().frames ||
       y < img.getOffset().rows || y > img.getOffset().rows + (img.getSize().rows - 1) * img.getSpacing().rows ||
       x < img.getOffset().columns || x > img.getOffset().columns + (img.getSize().columns - 1) * img.getSpacing().columns){
        return std::nullopt;
    }

    const uint32_t indz0 = static_cast<uint32_t>((z - img.getOffset().frames) / img.getSpacing().frames);
    const uint32_t indy0 = static_cast<uint32_t>((y - img.getOffset().rows) / img.getSpacing().rows);
    const uint32_t indx0 = static_cast<uint32_t>((x - img.getOffset().columns) / img.getSpacing().columns);
    uint32_t indz1 = indz0 + 1;
    uint32_t indy1 = indy0 + 1;
    uint32_t indx1 = indx0 + 1;

    if(indz1 == img.getSize().frames){
        indz1 = indz0;
    }
    if(indy1 == img.getSize().rows){
        indy1 = indy0;
    }
    if(indx1 == img.getSize().columns){
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

}
