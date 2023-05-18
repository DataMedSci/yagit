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
#pragma once

#include "ImageData.hpp"
#include "Image.hpp"

namespace yagit::Interpolation{

/**
 * @brief Linear interpolation along @a axis with new spacing
 * @param img Image to interpolate.
 * It can be 1D, 2D or 3D image - interpolation is done along one axis and won't take into account other axes.
 * @param spacing New spacing by which interpolation is performed
 * @param axis Axis along which interpolation is performed
 * @return Interpolated image with new spacing
 */
ImageData linearAlongAxis(const ImageData& img, float spacing, ImageAxis axis);

/**
 * @brief Bilinear interpolation on @a plane with new spacing
 * @param img Image to interpolate.
 * It can be 1D, 2D or 3D image - interpolation is done on one plane and won't take into account other planes.
 * @param firstAxisSpacing First spacing by which interpolation is performed.
 * E.g. when interpolating on plane YX, @a firstAxisSpacing is applied to Y axis.
 * @param secondAxisSpacing Second spacing by which interpolation is performed.
 * E.g. when interpolating on plane YX, @a secondAxisSpacing is applied to X axis.
 * @param plane Plane on which interpolation is performed
 * @return Interpolated image with new spacing
 */
ImageData bilinearOnPlane(const ImageData& img, float firstAxisSpacing, float secondAxisSpacing, ImagePlane plane);

/**
 * @brief Trilinear interpolation - along all axes (Z, Y, X) - with new spacing
 * @param img Image to interpolate.
 * @param spacing New spacing by which interpolation is performed
 * @return Interpolated image with new spacing
 */
ImageData trilinear(const ImageData& img, const DataSpacing& spacing);

/**
 * @brief Linear interpolation along @a axis on new grid with @a offset and @a spacing
 * 
 * On an infinite grid spanning all directions, utilizing new @a offset and @a spacing,
 * we determine the values that are on this grid and lying on the edge or in the middle of the interpolated image.
 * 
 * @param img Image to interpolate.
 * It can be 1D, 2D or 3D image - interpolation is done along one axis and won't take into account other axes.
 * @param offset Offset of interpolation grid. New grid on which image is interpolated will be offsetted by this value
 * @param spacing New spacing by which interpolation is performed
 * @param axis Axis along which interpolation is performed
 * @return Image interpolated on new grid
 */
ImageData linearAlongAxis(const ImageData& img, float offset, float spacing, ImageAxis axis);

/**
 * @brief Bilinear interpolation on @a plane on new grid with offset and spacing.
 * 
 * On an infinite grid spanning all directions, utilizing new offsets and spacings,
 * we determine the values that are on this grid and lying on the edge or in the middle of the interpolated image.
 * 
 * @param img Image to interpolate.
 * It can be 1D, 2D or 3D image - interpolation is done on one plane and won't take into account other planes.
 * @param firstAxisOffset First offset of interpolation grid. 
 * E.g. when interpolating on plane YX, @a firstAxisOffset is applied to Y axis.
 * New grid on which image is interpolated will be offsetted by this value.
 * @param secondAxisOffset Second offset of interpolation grid.
 * E.g. when interpolating on plane YX, @a secondAxisOffset is applied to X axis.
 * New grid on which image is interpolated will be offsetted by this value.
 * @param firstAxisSpacing First spacing by which interpolation is performed. 
 * E.g. when interpolating on plane YX, @a firstAxisSpacing is applied to Y axis.
 * @param secondAxisSpacing Second spacing by which interpolation is performed. 
 * E.g. when interpolating on plane YX, @a secondAxisSpacing is applied to X axis.
 * @param plane Plane on which interpolation is performed
 * @return Image interpolated on new grid
 */
ImageData bilinearOnPlane(const ImageData& img, float firstAxisOffset, float secondAxisOffset,
                          float firstAxisSpacing, float secondAxisSpacing, ImagePlane plane);

/**
 * @brief Trilinear interpolation - along all axes (Z, Y, X) - on new grid with @a offset and @a spacing.
 * 
 * On an infinite grid spanning all directions, utilizing new @a offset and @a spacing,
 * we determine the values that are on this grid and lying on the edge or in the middle of the interpolated image.
 * 
 * @param img Image to interpolate.
 * @param offset Offset of interpolation grid. New grid on which image is interpolated will be offsetted by this value
 * @param spacing New spacing by which interpolation is performed
 * @return Image interpolated on new grid
 */
ImageData trilinear(const ImageData& img, const DataOffset& offset, const DataSpacing& spacing);

/**
 * @brief Linear interpolation along @a axis on the grid of @a refImg.
 * 
 * On an infinite grid spanning all directions, utilizing offset and spacing of @a refImg,
 * we determine the values that are on this grid and lying on the edge or in the middle of the interpolated image.
 * 
 * @param evalImg Image to interpolate
 * @param refImg Image from which offset and spacing is retrieved and used to create grid on which interpolation is performed
 * @param axis Axis along which interpolation is performed
 * @return Image interpolated on the grid of @a refImg
 */
ImageData linearAlongAxis(const ImageData& evalImg, const ImageData& refImg, ImageAxis axis);

/**
 * @brief Bilinear interpolation on @a plane on the grid of @a refImg.
 * 
 * On an infinite grid spanning all directions, utilizing offset and spacing of @a refImg,
 * we determine the values that are on this grid and lying on the edge or in the middle of the interpolated image.
 * 
 * @param evalImg Image to interpolate
 * @param refImg Image from which offset and spacing is retrieved and used to create grid on which interpolation is performed
 * @param plane Plane on which interpolation is performed
 * @return Image interpolated on the grid of @a refImg
 */
ImageData bilinearAlongPlane(const ImageData& evalImg, const ImageData& refImg, ImagePlane plane);

/**
 * @brief Trilinear interpolation - along all axes (Z, Y, X) - on the grid of @a refImg.
 * 
 * On an infinite grid spanning all directions, utilizing offset and spacing of @a refImg,
 * we determine the values that are on this grid and lying on the edge or in the middle of the interpolated image.
 * 
 * @param evalImg Image to interpolate
 * @param refImg Image from which offset and spacing is retrieved and used to create grid on which interpolation is performed
 * @return Image interpolated on the grid of @a refImg
 */
ImageData trilinear(const ImageData& evalImg, const ImageData& refImg);

}
