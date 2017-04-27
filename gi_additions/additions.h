/** @file */

/*********************************************************************************************************************
 * Additions
 *
 * This file contains a set of additional functions provided alongside the core functions.
 * User can use them for preprocessing the input and further processing the output.
 * There are functions that allow to:
 *  - apply mask to input image
 *  - filter the input image
 *  - modify the input image using a linear function
 *  - calculate statistics from the output image
 *  - prepare histogram and cumulative histogram
 *********************************************************************************************************************/
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
#ifndef ADDITIONS_H
#define ADDITIONS_H

#include<cstddef>
#include<string>


/// \brief Converts a double array to binary mask.
///
/// Using custom converter function, constructs a binary mask with the size of input array.
///
/// If no converter is specified (or NULL is passed) the default converter is applied.
///
/// Default converter returns True if array value is greater than 0.
///
/// \note Returned array has to be freed by the user.
///
/// \param size      The length of linearized array representing array used to construct binary mask.
/// \param array     Linearized double array representing array used to construct binary mask.
/// \param converter (optional) Pointer to function determining whether given value in
///                             original mask should evaluate to True or False.
///
/// \return Pointer to binary mask with the size of given array.
///
///////////////////////////////////////////////////////////////////////////////
bool* convertArrayToBinaryMask(int size, double* array, bool ((*converter)(double)) = NULL);

/// \brief Sets image element to NaN if corresponding element in mask is False.
///
/// \note Image and mask must be the same size.
///
/// \param size  The length of linearized array representing the image.
/// \param image Linearized double array representing image.
/// \param mask  Pointer to binary mask.
///
///////////////////////////////////////////////////////////////////////////////
void applyMaskToImage(int size, double* image, bool* mask);

/// \brief Sets image element to NaN if filter function returns False.
///
/// \param size   The length of linearized array representing the image.
/// \param image  Linearized double array representing image.
/// \param filter Pointer to function determining whether given value in image
///               should be excluded from further analysis. Can take additional
///               parameter determined by user.
///               (See applyNoiseFilteringToImage and noiseFilter functions.)
/// \param params (optional) Additional parameter passed to filter function.
///
///////////////////////////////////////////////////////////////////////////////
void applyFilteringFunctionToImage(int size, double* image, bool (*filter)(double, void*), void* params = NULL);

/// \brief Eliminates from the image values considered as a noise.
///
/// \note The threshold will be calculated using formula:
///       threshold = minVal + (maxVal - minVal) * noisePercentage %
///
/// \param size            The length of linearized array representing the image.
/// \param image           Linearized double array representing image.
/// \param noisePercentage Used to calculate threshold below which the voxels
///                        in the image will be considered as a noise.
///
///////////////////////////////////////////////////////////////////////////////
void applyNoiseFilteringToImage(int size, double* image, double noisePercentageLimit);

/// \brief Modifies image using a linear function.
///
/// Each element x in the image is set as follows:
///     x <- multiply * x + add
///
/// \param size     The length of linearized array representing the image.
/// \param image    Linearized double array representing image.
/// \param multiply See above.
/// \param add      See above.
///
///////////////////////////////////////////////////////////////////////////////
void applyLinearMappingToImage(int size, double* image, double multiply, double add);


/// \brief Calculates the Gamma Index array size. It is usually the same as reference image size,
///        but in case of 2.5D and 2Dfrom3D comparisons it differs.
///
/// \note All parameters should be the same as those passed to calculateGamma() function.
///
/// \param dims       Number of dimensions of the images under comparison:
///                   - 1 - comparison 1D,
///                   - 2 - comparison 2D,
///                   - 3 - comparison 3D,
///                   - 4 - comparison 2.5D
///                         (comparison of one slice of the reference image against the whole target image),
///                   - 5 - comparison 2Dfrom3D
///                         (comparison of one slice of the reference image against another slice of the target image).
/// \param refXNumber Number of voxels along x axis in the reference image.
/// \param refYNumber Number of voxels along y axis in the reference image.
/// \param refZNumber Number of voxels along z axis in the reference image.
/// \param plane      (optional) Required only when dims is greater than 3.
///                   Specifies the orientation of the reference slice:
///                   - 0 - parallel to XY plane,
///                   - 1 - parallel to XZ plane,
///                   - 2 - parallel to YZ plane.
///
/// \return Size of Gamma Index array.
///
///////////////////////////////////////////////////////////////////////////////
int calculateGammaArraySize(int dims, int refXNumber, int refYNumber = 1, int refZNumber = 1, int plane = -1);

/// \brief Counts the number of elements of image that are not NaN.
///
/// \param size  The length of linearized array representing the image.
/// \param image Linearized double array representing image.
///
/// \return Number of active elements in the image.
///
///////////////////////////////////////////////////////////////////////////////
int activeVoxelsNumber(int size, double* image);

/// \brief Counts the percentage of elements of image that are not NaN.
///
/// \param size  The length of linearized array representing the image.
/// \param image Linearized double array representing image.
///
/// \return Percentage of active elements in the image.
///
///////////////////////////////////////////////////////////////////////////////
double activeVoxelsPercentage(int size, double* image);

/// \brief Finds the minimum value in the image.
///
/// \param size  The length of linearized array representing the image.
/// \param image Linearized double array representing image.
///
/// \return Minimum value in the image.
///
///////////////////////////////////////////////////////////////////////////////
double imageMin(int size, double* image);

/// \brief Finds the maximum value in the image.
///
/// \param size  The length of linearized array representing the image.
/// \param image Linearized double array representing image.
///
/// \return Maximum value in the image.
///
///////////////////////////////////////////////////////////////////////////////
double imageMax(int size, double* image);

/// \brief Calculates the average value in the image.
///
/// \param size  The length of linearized array representing the image.
/// \param image Linearized double array representing image.
///
/// \return Average value in the image.
///
///////////////////////////////////////////////////////////////////////////////
double imageAvg(int size, double* image);

/// \brief Calculates Gamma Passing Rate.
///
/// Gamma Passing Rate is the percentage of voxels for which
/// the Gamma Index value is smaller than 1.0.
///
/// \param size  The length of linearized array representing image of GI values.
///              It can be calculated using calculateGammaArraySize()
/// \param gamma Linearized double array representing image of GI values.
///
/// \return Gamma Passing Rate.
///
///////////////////////////////////////////////////////////////////////////////
double gammaPassingRate(int size, double* gamma);

/// \brief Constructs brief example of description of the comparison. Can be used to distinguish entries in statistics file.
///
/// \note One can specify their own description and pass it to the: prepareStatisticsFile() function.
///
/// \param refFile   Name of the reference file.
/// \param tarFile   Name of the target file.
/// \param algorithm Algorithm used for comparison.
///                  - 0 - Classic Solver,
///                  - 1 - Spiral Solver with rectangle,
///                  - 2 - Spiral Solver without rectangle.
///
/// \return A short description.
///
///////////////////////////////////////////////////////////////////////////////
std::string createComputationDescription(std::string refFile, std::string tarFile, int algorithm);

/// \brief Writes Gamma Index statistics to a file.
///
/// \param size     The length of linearized array representing image of GI values.
///                 It can be calculated using calculateGammaArraySize()
/// \param gamma    Linearized double array representing image of GI values.
/// \param info     String that briefly describes the new stats entry.
///                 It can be prepared using createComputationDescription()
/// \param filepath Path to the file where the statistics will be saved (default: stats.txt).
///
///////////////////////////////////////////////////////////////////////////////
void prepareStatisticsFile(int size, double* gamma, std::string info, std::string filepath = "stats.txt");

/// \brief Creates a histogram of Gamma Index values.
///
/// Constructed histogram is an array of integers, in which i-th element
/// states for the number of voxels in gamma image that have value in
/// the range <delimiters[i]; delimiters[i+1]).
///
/// \note Returned array has to be freed by the user.
///
/// \param gammaSize      The length of linearized array representing image of GI values.
/// \param gamma          Linearized double array representing image of GI values.
/// \param delimitersSize The length of the delimiters array.
/// \param delimiters     Pointer to double array, which values are the boundaries
///                       of the consecutive brackets of the histogram.
///
/// \return Array of integers representing histogram.
///         The array size is equal to delimiters-1.
///
///////////////////////////////////////////////////////////////////////////////
int* prepareHistogram(int gammaSize, double* gamma, int delimitersSize, double* delimiters);

/// \brief Creates a histogram of Gamma Index values.
///
/// Constructed histogram is an array of integers. Histogram consists of
/// bucketsNumber buckets of the same size (step = (xMax - xMin)/bucketsNumber)).
/// The i-th element of the histogram states for the number of voxels in gamma image
/// that have value in the range <xMin + i * step; xMin + (i+1) * step).
///
/// \note Returned array has to be freed by the user.
///
/// \param gammaSize     The length of linearized array representing image of GI values.
/// \param gamma         Linearized double array representing image of GI values.
/// \param xMin          Minimum value that is taken into the histogram.
/// \param xMax          Maximum value that is taken into the histogram.
/// \param bucketsNumber Number of buckets that the range <xMin; xMax) is divided into.
///
/// \return Array of integers representing histogram.
///         The array size is equal to bucketsNumber.
///
///////////////////////////////////////////////////////////////////////////////
int* prepareHistogram(int gammaSize, double* gamma, double xMin, double xMax, int bucketsNumber);

/// \brief Creates a histogram of Gamma Index values.
///
/// This histogram consists of 300 buckets in the range <0.0; 3.0).
/// For further information see the comment to the function immediately above.
///
/// \note Returned array has to be freed by the user.
///
/// \param gammaSize The length of linearized array representing image of GI values.
/// \param gamma     Linearized double array representing image of GI values.
///
/// \return Array of integers representing histogram. The array size is 300.
///
///////////////////////////////////////////////////////////////////////////////
int* prepareHistogram(int gammaSize, double* gamma);

/// \brief Creates a cumulative histogram of Gamma Index values.
///
/// Constructed histogram is an array of integers, in which i-th element
/// states for the number of voxels in gamma image that have value in
/// the range <delimiters[0]; delimiters[i+1]).
///
/// \note Returned array has to be freed by the user.
///
/// \param gammaSize      The length of linearized array representing image of GI values.
/// \param gamma          Linearized double array representing image of GI values.
/// \param delimitersSize The length of the delimiters array.
/// \param delimiters     Pointer to double array, which values are the boundaries
///                       of the consecutive brackets of the histogram.
///
/// \return Array of integers representing histogram.
///         The array size is equal to delimiters-1.
///
///////////////////////////////////////////////////////////////////////////////
int* prepareCumulativeHistogram(int gammaSize, double* gamma, int delimitersSize, double* delimiters);

/// \brief Creates a cumulative histogram of Gamma Index values.
///
/// Constructed histogram is an array of integers. Histogram consists of
/// bucketsNumber buckets of the same size (step = (xMax - xMin)/bucketsNumber)).
/// The i-th element of the histogram states for the number of voxels in gamma image
/// that have value in the range <xMin; xMax + (i+1) * step).
///
/// \note Returned array has to be freed by the user.
///
/// \param gammaSize     The length of linearized array representing image of GI values.
/// \param gamma         Linearized double array representing image of GI values.
/// \param xMin          Minimum value that is taken into the histogram.
/// \param xMax          Maximum value that is taken into the histogram.
/// \param bucketsNumber Number of buckets that the range <xMin; xMax) is divided into.
///
/// \return Array of integers representing histogram.
///         The array size is equal to bucketsNumber.
///
///////////////////////////////////////////////////////////////////////////////
int* prepareCumulativeHistogram(int gammaSize, double* gamma, double xMin, double xMax, int bucketsNumber);

/// \brief Creates a cumulative histogram of Gamma Index values.
///
/// This histogram consists of 300 buckets in the range <0.0; 3.0).
/// For further information see the comment to the function immediately above.
///
/// \note Returned array has to be freed by the user.
///
/// \param gammaSize The length of linearized array representing image of GI values.
/// \param gamma     Linearized double array representing image of GI values.
///
/// \return Array of integers representing histogram. The array size is 300.
///
///////////////////////////////////////////////////////////////////////////////
int* prepareCumulativeHistogram(int gammaSize, double* gamma);


#endif //ADDITIONS_H
