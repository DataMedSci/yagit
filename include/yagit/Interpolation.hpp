/********************************************************************************************
 * Copyright (C) 2023-2024 'Yet Another Gamma Index Tool' Developers.
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
#pragma once

#include <optional>

#include "yagit/ImageData.hpp"
#include "yagit/Image.hpp"

namespace yagit::Interpolation{

/**
 * @brief Linear interpolation along @a axis with new spacing
 * @param img Image to interpolate.
 * It can be 1D, 2D, or 3D image - interpolation is done along one axis and won't take into account other axes.
 * @param spacing New spacing by which interpolation is performed
 * @param axis Axis along which interpolation is performed
 * @return Interpolated image with new spacing
 */
ImageData linearAlongAxis(const ImageData& img, float spacing, ImageAxis axis);

/**
 * @brief Linear interpolation along @a axis on new grid with @a offset and @a spacing
 * 
 * On an infinite grid spanning all directions, utilizing new @a offset and @a spacing,
 * we determine the values that are on this grid that fall within the interpolated image.
 * 
 * @param img Image to interpolate.
 * It can be 1D, 2D, or 3D image - interpolation is done along one axis and won't take into account other axes.
 * @param gridOffset Offset of interpolation grid. New grid on which image is interpolated will be offsetted by this value
 * @param spacing New spacing by which interpolation is performed
 * @param axis Axis along which interpolation is performed
 * @return Image interpolated on new grid
 */
ImageData linearAlongAxis(const ImageData& img, float gridOffset, float spacing, ImageAxis axis);

/**
 * @brief Linear interpolation along @a axis on the grid of @a refImg.
 * 
 * On an infinite grid spanning all directions, utilizing offset and spacing of @a refImg,
 * we determine the values that are on this grid that fall within the interpolated image.
 * 
 * @param targetImg Image to interpolate
 * @param refImg Image from which offset and spacing is retrieved and used to create grid on which interpolation is performed
 * @param axis Axis along which interpolation is performed
 * @return Image interpolated on the grid of @a refImg
 */
ImageData linearAlongAxis(const ImageData& targetImg, const ImageData& refImg, ImageAxis axis);

/**
 * @brief Bilinear interpolation on @a plane with new spacing
 * @param img Image to interpolate.
 * It can be 1D, 2D, or 3D image - interpolation is done on one plane and won't take into account other planes.
 * @param firstAxisSpacing First spacing by which interpolation is performed.
 * E.g., when interpolating on plane YX, @a firstAxisSpacing is applied to Y axis.
 * @param secondAxisSpacing Second spacing by which interpolation is performed.
 * E.g., when interpolating on plane YX, @a secondAxisSpacing is applied to X axis.
 * @param plane Plane on which interpolation is performed
 * @return Interpolated image with new spacing
 */
ImageData bilinearOnPlane(const ImageData& img, float firstAxisSpacing, float secondAxisSpacing, ImagePlane plane);

/**
 * @brief Bilinear interpolation on @a plane on new grid with offset and spacing.
 * 
 * On an infinite grid spanning all directions, utilizing new offsets and spacings,
 * we determine the values that are on this grid that fall within the interpolated image.
 * 
 * @param img Image to interpolate.
 * It can be 1D, 2D, or 3D image - interpolation is done on one plane and won't take into account other planes.
 * @param firstAxisGridOffset First offset of interpolation grid. 
 * E.g., when interpolating on plane YX, @a firstAxisOffset is applied to Y axis.
 * New grid on which image is interpolated will be offsetted by this value.
 * @param secondAxisGridOffset Second offset of interpolation grid.
 * E.g., when interpolating on plane YX, @a secondAxisOffset is applied to X axis.
 * New grid on which image is interpolated will be offsetted by this value.
 * @param firstAxisSpacing First spacing by which interpolation is performed. 
 * E.g., when interpolating on plane YX, @a firstAxisSpacing is applied to Y axis.
 * @param secondAxisSpacing Second spacing by which interpolation is performed. 
 * E.g., when interpolating on plane YX, @a secondAxisSpacing is applied to X axis.
 * @param plane Plane on which interpolation is performed
 * @return Image interpolated on new grid
 */
ImageData bilinearOnPlane(const ImageData& img, float firstAxisGridOffset, float secondAxisGridOffset,
                          float firstAxisSpacing, float secondAxisSpacing, ImagePlane plane);

/**
 * @brief Bilinear interpolation on @a plane on the grid of @a refImg.
 * 
 * On an infinite grid spanning all directions, utilizing offset and spacing of @a refImg,
 * we determine the values that are on this grid that fall within the interpolated image.
 * 
 * @param targetImg Image to interpolate
 * @param refImg Image from which offset and spacing is retrieved and used to create grid on which interpolation is performed
 * @param plane Plane on which interpolation is performed
 * @return Image interpolated on the grid of @a refImg
 */
ImageData bilinearOnPlane(const ImageData& targetImg, const ImageData& refImg, ImagePlane plane);

/**
 * @brief Trilinear interpolation - along all axes (Z, Y, X) - with new spacing
 * @param img Image to interpolate
 * @param spacing New spacing by which interpolation is performed
 * @return Interpolated image with new spacing
 */
ImageData trilinear(const ImageData& img, const DataSpacing& spacing);

/**
 * @brief Trilinear interpolation - along all axes (Z, Y, X) - on new grid with @a offset and @a spacing.
 * 
 * On an infinite grid spanning all directions, utilizing new @a offset and @a spacing,
 * we determine the values that are on this grid that fall within the interpolated image.
 * 
 * @param img Image to interpolate
 * @param gridOffset Offset of interpolation grid. New grid on which image is interpolated will be offsetted by this value
 * @param spacing New spacing by which interpolation is performed
 * @return Image interpolated on new grid
 */
ImageData trilinear(const ImageData& img, const DataOffset& gridOffset, const DataSpacing& spacing);

/**
 * @brief Trilinear interpolation - along all axes (Z, Y, X) - on the grid of @a refImg.
 * 
 * On an infinite grid spanning all directions, utilizing offset and spacing of @a refImg,
 * we determine the values that are on this grid that fall within the interpolated image.
 * 
 * @param targetImg Image to interpolate
 * @param refImg Image from which offset and spacing is retrieved and used to create grid on which interpolation is performed
 * @return Image interpolated on the grid of @a refImg
 */
ImageData trilinear(const ImageData& targetImg, const ImageData& refImg);

/**
 * @brief Bilinear interpolation at point inside image
 * 
 * @param img Image on which interpolation is performed
 * @param frame Axial frame where interpolation is performed
 * @param y Y coordinate of the point where interpolation is performed
 * @param x X coordinate of the point where interpolation is performed
 * @return If the point is inside the image, then an interpolated value is returned
 */
std::optional<float> bilinearAtPoint(const ImageData& img, uint32_t frame, float y, float x);

/**
 * @brief Trilinear interpolation at point inside image
 * 
 * @param img Image on which interpolation is performed
 * @param z Z coordinate of the point where interpolation is performed
 * @param y Y coordinate of the point where interpolation is performed
 * @param x X coordinate of the point where interpolation is performed
 * @return If the point is inside the image, then an interpolated value is returned
 */
std::optional<float> trilinearAtPoint(const ImageData& img, float z, float y, float x);

}
