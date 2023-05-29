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
#pragma once

#include "ImageData.hpp"
#include "GammaParameters.hpp"
#include "GammaResult.hpp"

namespace yagit{

/**
 * @brief Calculate 2D gamma index using classic method.
 * 
 * It takes into account y and x coordinates of images.
 * It doesn't take into account the z coordinates
 * (different z-offsets of @a refImg and @a evalImg have no impact on the result).
 * 
 * Based on https://doi.org/10.1118/1.598248
 * 
 * @param refImg2D 2D reference image
 * @param evalImg2D 2D evaluated image
 * @param gammaParams Parameters of gamma index
 * @return 2D image containing gamma index values
 */
GammaResult gammaIndex2D(const ImageData& refImg2D, const ImageData& evalImg2D, const GammaParameters& gammaParams);

/**
 * @brief Calculate 2.5D gamma index using classic method.
 * 
 * It calculates gamma index slice by slice going along axial plane.
 * On each slice, it takes into account y and x coordinates of images.
 * It uses z coordinates to calculate distance between two corresponding frames of @a refImg3D and @a evalImg3D.
 * 
 * Based on https://doi.org/10.1118/1.598248
 * 
 * @param refImg3D 3D reference image
 * @param evalImg3D 3D evaluated image
 * @param gammaParams Parameters of gamma index
 * @return 3D image containing gamma index values
 */
GammaResult gammaIndex2_5D(const ImageData& refImg3D, const ImageData& evalImg3D, const GammaParameters& gammaParams);

/**
 * @brief Calculate 3D gamma index using classic method.
 * 
 * It takes into account z, y and x coordinates of images.
 * 
 * Based on https://doi.org/10.1118/1.598248
 * 
 * @param refImg3D 3D reference image
 * @param evalImg3D 3D evaluated image
 * @param gammaParams Parameters of gamma index
 * @return 3D image containing gamma index values 
 */
GammaResult gammaIndex3D(const ImageData& refImg3D, const ImageData& evalImg3D, const GammaParameters& gammaParams);

/**
 * @brief Calculate 2D gamma index using Wendling method.
 * 
 * It takes into account y and x coordinates of images.
 * It doesn't take into account the z coordinates
 * (different z-offsets of @a refImg and @a evalImg have no impact on the result).
 * 
 * It is faster than classic method, so using this function is recommended.
 * Based on https://doi.org/10.1118/1.2721657
 * 
 * @param refImg2D 2D reference image
 * @param evalImg2D 2D evaluated image
 * @param gammaParams Parameters of gamma index
 * @return 2D image containing gamma index values
 */
GammaResult gammaIndex2DWendling(const ImageData& refImg2D, const ImageData& evalImg2D, const GammaParameters& gammaParams);

/**
 * @brief Calculate 2.5D gamma index using Wendling method.
 * 
 * It calculates gamma index slice by slice going along axial plane.
 * On each slice, it takes into account y and x coordinates of images.
 * @a evalImg3D is interpolated along the z axis before gamma calculations
 * to have the same grid only along that axis as @a refImg.
 * 
 * It is faster than classic method, so using this function is recommended.
 * Based on https://doi.org/10.1118/1.2721657
 * 
 * @param refImg3D 3D reference image
 * @param evalImg3D 3D evaluated image
 * @param gammaParams Parameters of gamma index
 * @return 3D image containing gamma index values 
 */
GammaResult gammaIndex2_5DWendling(const ImageData& refImg3D, const ImageData& evalImg3D, const GammaParameters& gammaParams);

/**
 * @brief Calculate 3D gamma index using Wendling method.
 * 
 * It takes into account z, y and x coordinates of images.
 * 
 * It is faster than classic method, so using this function is recommended.
 * Based on https://doi.org/10.1118/1.2721657
 * 
 * @param refImg3D 3D reference image
 * @param evalImg3D 3D evaluated image
 * @param gammaParams Parameters of gamma index
 * @return 3D image containing gamma index values 
 */
GammaResult gammaIndex3DWendling(const ImageData& refImg3D, const ImageData& evalImg3D, const GammaParameters& gammaParams);

}
