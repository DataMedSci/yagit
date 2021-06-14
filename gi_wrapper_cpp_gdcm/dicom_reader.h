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

#ifndef DICOM_READER_H
#define DICOM_READER_H

namespace gdcm {
	class DataSet;
}

class DicomReader
{
public:
    DicomReader();

    /// \brief Retrieves the image matrix and its parameters from DICOM file.
    ///
    /// \note Every parameter, except for the first one , will be filled by the function
    ///       with the image spatial parameters. Their initial values are not important and will be lost.
    ///
    /// \param filename Name of the file containing DICOM image.
    /// \param dims     Number of dimensions of acquired image.
    /// \param xStart   x coordinate of the center of the most left voxel in the image.
    /// \param xSpacing Distance between centers of adjacent voxels along x axis in the image.
    /// \param xNumber  Number of voxels along x axis in the image.
    /// \param yStart   y coordinate of the center of the most top voxel in the image.
    /// \param ySpacing Distance between centers of adjacent voxels along y axis in the image.
    /// \param yNumber  Number of voxels along y axis in the image.
    /// \param zStart   z coordinate of the center of the most front voxel in the image.
    /// \param zSpacing Distance between centers of adjacent voxels along z axis in the image.
    /// \param zNumber  Number of voxels along z axis in the image.
    ///
    /// \return Pointer to the image matrix. Its size can be calculated using parameters: dims, xNumber, yNumber and zNumber.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double* acquireImage(char* filename, int& dims,
        double& xStart, double& xSpacing, int& xNumber,
        double& yStart, double& ySpacing, int& yNumber,
        double& zStart, double& zSpacing, int& zNumber);

private:
    double acquireStart(gdcm::DataSet& dataSet, int dim);

    double acquireSpacing(gdcm::DataSet& dataSet, int dim);
};

#endif