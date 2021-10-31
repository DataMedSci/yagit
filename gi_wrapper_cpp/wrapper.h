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

#ifndef WRAPPER_H
#define WRAPPER_H

#include <set>
#include <string>
#include <memory>
#include "wrapper_logger.h"


namespace imebra {
 class DataSet;
}

/// \brief Loads DICOM file into an imebra object.
///
/// \note Returned pointer should be assigned to an unique_ptr.
///       The returned object should be later passed to other functions from this module.
///
/// \param filepath Path to DICOM file tha should be loaded.
///
/// \return An imebra object containing parsed DICOM file.
///
///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<imebra::DataSet> loadDicom(std::string filepath);

/// \brief Retrieves the image matrix as linearized array and its parameters from DICOM file.
///
/// \note Every parameter, except for the first one - dataSet, will be filled by the function
///       with the image spatial parameters. Their initial values are not important and will be lost.
///
///       Returned array must be freed by the user.
///
/// \param dataSet     Pointer to the object in which Imebra library stores parsed DICOM file.
///                    The pointer returned by loadDicom() function.
/// \param dims        Number of dimensions of of acquired image.
/// \param xStart      x coordinate of the center of the most left voxel in the image.
/// \param xSpacing    Distance between centers of adjacent voxels along x axis in the image.
/// \param xNumber     Number of voxels along x axis in the image.
/// \param yStart      y coordinate of the center of the most top voxel in the image.
/// \param ySpacing    Distance between centers of adjacent voxels along y axis in the image.
/// \param yNumber     Number of voxels along y axis in the image.
/// \param zStart      z coordinate of the center of the most front voxel in the image.
/// \param zSpacing    Distance between centers of adjacent voxels along z axis in the image.
/// \param zNumber     Number of voxels along z axis in the image.
///
/// \return Pointer to the image array. It size can be calculated using parameters: dims, xNumber, yNumber and zNumber.
///
///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<double[]> acquireImage(const imebra::DataSet& dataSet, int& dims,
                     double& xStart, double& xSpacing, int& xNumber,
                     double& yStart, double& ySpacing, int& yNumber,
                     double& zStart, double& zSpacing, int& zNumber);

/// \brief Saves Gamma Index array and all necessary tags to a DICOM file.
///
/// \note Values of parameters: dims, refXNumber, refYNumber, refZNumber, plane, refSlice
///       should be the same as those passed to calculateGamma() function.
///
/// \param dims                Number of dimensions of the images under comparison:
///                            - 1 - comparison 1D,
///                            - 2 - comparison 2D,
///                            - 3 - comparison 3D,
///                            - 4 - comparison 2.5D
///                                  (comparison of one slice of the reference image against the whole target image),
///                            - 5 - comparison 2Dfrom3D
///                                  (comparison of one slice of the reference image against another slice of the target image).
/// \param oldDataSet          Pointer to the object in which Imebra library stored parsed reference DICOM file.
///                            The pointer returned by loadDicom() function. Used while transferring tags to the output DICOM file.
/// \param gamma               The calculated gamma array.
/// \param filepath            Path where the output DICOM file will be saved.
/// \param refXNumber          Number of voxels along x axis in the gamma image.
/// \param refYNumber          Number of voxels along y axis in the gamma image.
/// \param refZNumber          Number of voxels along z axis in the gamma image.
/// \param precision           Number of digits after the decimal point in the resulting Gamma Index.
/// \param fillValue           Value that will be assigned to voxels for which the Gamma Index comparison was not performed
///                            or had assigned NaN value during or before comparison (e.g. due to noise filtering).
///                            We propose to choose the value 1.1*limit or limit+1.
/// \param rewriteTagsStrategy Specifies the strategy of transferring tags from reference DICOM to output DICOM file:
///                            - 5 - rewrite all tags, except these specified in tags parameter,
///                            - 4 - rewrite only these specified in tags parameter,
///                            - 3 - rewrite only not private tags,
///                            - 2 - rewrite no tags,
///                            - 1 or other integer values - rewrite all tags.
/// \param tags                The set specifying which exactly tags (not) to transfer.
///                            For more information on how to construct this set see applyDicomTagsRewriteStrategy().
/// \param plane               (optional) Required only when dims is greater than 3.
///                            Specifies the orientation of the gamma slice:
///                            - 0 - parallel to XY plane,
///                            - 1 - parallel to XZ plane,
///                            - 2 - parallel to YZ plane.
/// \param refSlice            (optional) Required only when dims is greater than 3.
///                            Specifies for which reference slice (in the given orientation) the GI comparison was performed.
///                            First slice has number 0.
///
///////////////////////////////////////////////////////////////////////////////
void saveImage(int dims, imebra::DataSet* oldDataSet, std::string filepath, double* gamma,
               int refXNumber, int refYNumber, int refZNumber, int precision, double fillValue,
               int rewriteTagsStrategy = 1, std::set<std::pair<int, int>> tags = (std::set<std::pair<int, int> >()),
               int plane = -1, int refSlice = -1);

#endif
