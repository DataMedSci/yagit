/** @file */

/*********************************************************************************************************************
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
 *********************************************************************************************************************/

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include "core_logger.h"


/// \brief Main function. Responsible for calculating Gamma Index.
///
/// \note
///       - Passed images must have the same number of dimensions.
///       - ...Y... parameters are only relevant if the images to be compared have 2 or 3 dimensions.
///       - ...Z... parameters are only relevant if the images to be compared have 3 dimensions.
///       - Returned array has to be freed by the user.
///
/// \param algorithm      Determines which solver should be used to perform the comparison:
///                       - 0 - Classic Solver,
///                       - 1 - Spiral Solver with rectangle,
///                       - 2 - Spiral Solver without rectangle.
/// \param dims           Number of dimensions of the images under comparison:
///                       - 1 - comparison 1D,
///                       - 2 - comparison 2D,
///                       - 3 - comparison 3D,
///                       - 4 - comparison 2.5D
///                             (comparison of one slice of the reference image against the whole target image),
///                       - 5 - comparison 2Dfrom3D
///                             (comparison of one slice of the reference image against another slice of the target image).
/// \param reference      First image that will be compared.
/// \param target         Second image that will be compared.
/// \param refXStart      x coordinate of the center of the most left top front voxel in the reference image.
/// \param refXSpacing    Distance between centers of adjacent voxels along x axis in the reference image.
/// \param refXNumber     Number of voxels along x axis in the reference image.
/// \param refYStart      y coordinate of the center of the most left top front voxel in the reference image.
/// \param refYSpacing    Distance between centers of adjacent voxels along y axis in the reference image.
/// \param refYNumber     Number of voxels along y axis in the reference image.
/// \param refZStart      z coordinate of the center of the most left top front voxel in the reference image.
/// \param refZSpacing    Distance between centers of adjacent voxels along z axis in the reference image.
/// \param refZNumber     Number of voxels along z axis in the reference image.
/// \param tarXStart      x coordinate of the center of the most left top front voxel in the target image.
/// \param tarXSpacing    Distance between centers of adjacent voxels along x axis in the target image.
/// \param tarXNumber     Number of voxels along x axis in the target image.
/// \param tarYStart      y coordinate of the center of the most left top front voxel in the target image.
/// \param tarYSpacing    Distance between centers of adjacent voxels along y axis in the target image.
/// \param tarYNumber     Number of voxels along y axis in the target image.
/// \param tarZStart      z coordinate of the center of the most left top front voxel in the target image.
/// \param tarZSpacing    Distance between centers of adjacent voxels along z axis in the target image.
/// \param tarZNumber     Number of voxels along z axis in the target image.
/// \param percentage     This is the DD parameter passed in %. Usually is set to 3.0.
/// \param dta            This is the DTA parameter passed in mm. Usually is set to 3.0.
/// \param local          Flag determining if the comparison should be performed using local of global
///                       version of GI algorithm.
/// \param referenceValue The value that is multiplied by the percentage to calculate
///                       the denominator of the dd component. Used only if local flag is set to False.
///                       If -1 is passed, then the default values is set, which is the maximum value in reference image.
/// \param limit          The initial value of GI.
///                       If -1 is passed, then the default values is set: 2.0.
/// \param plane          (optional) Required only when dims is greater than 3.
///                       Specifies the orientation of the reference slice:
///                       - 0 - parallel to XY plane,
///                       - 1 - parallel to XZ plane,
///                       - 2 - parallel to YZ plane.
/// \param refSlice       (optional) Required only when dims is greater than 3.
///                       Specifies for which reference slice (in the given orientation) the comparison should be performed.
///                       First slice has number 0.
/// \param tarSlice       (optional) Required only when dims is equal 5.
///                       Specifies for which target slice (in the given orientation) the comparison should be performed.
///                       First slice has number 0.
///
/// \return Array containing calculated Gamma Index values for the given images.
///         Array size is the same as the reference image (or its slice when 2.5D or 2Dfrom3D comparison is performed).
///
///////////////////////////////////////////////////////////////////////////////
double* calculateGamma(int algorithm, int dims, double* reference, double* target,
                       double refXStart, double refXSpacing, int refXNumber,
                       double refYStart, double refYSpacing, int refYNumber,
                       double refZStart, double refZSpacing, int refZNumber,
                       double tarXStart, double tarXSpacing, int tarXNumber,
                       double tarYStart, double tarYSpacing, int tarYNumber,
                       double tarZStart, double tarZSpacing, int tarZNumber,
                       double percentage, double dta, bool local, double referenceValue,
                       double limit, int plane = -1, int refSlice = -1, int tarSlice = -1);

#endif
