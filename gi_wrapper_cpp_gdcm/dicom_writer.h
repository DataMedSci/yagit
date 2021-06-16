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

#ifndef DICOM_WRITER_H
#define DICOM_WRITER_H

#include <set>
#include <string>

namespace gdcm {
    class DataSet;
    class File;
    class PixelFormat;
    class PhotometricInterpretation;
}

class DicomWriter
{
public:
    DicomWriter();

    virtual ~DicomWriter();

/// \brief Saves Gamma Index array and all necessary tags to a DICOM file.
///
/// \note Values of parameters: dims, refXNumber, refYNumber, refZNumber, plane, refSlice
///       should be the same as those passed to calculateGamma() function.
///
/// \param filepath              Path where the output DICOM file will be saved.
/// \param oldFile               Reference to the object in which GDCM library stored parsed reference DICOM file.
///                              The reference returned by loadDicom() function. Used while transferring tags to the output DICOM file.
/// \param gamma                 The calculated gamma array.
/// \param xNumber               Number of voxels along x axis in the gamma image.
/// \param yNumber               Number of voxels along y axis in the gamma image.
/// \param zNumber               Number of voxels along z axis in the gamma image.
/// \param precision             Number of digits after the decimal point in the resulting Gamma Index.
/// \param fillValue             Value that will be assigned to voxels for which the GI comparison was not performed.
/// \param rewriteTagsStrategyId Specifies the strategy of transferring tags from reference DICOM to output DICOM file:
///                              - 5 - rewrite all tags, except these specified in tags parameter
///                              - 4 - rewrite only these specified in tags parameter
///                              - 3 - rewrite only not private tags
///                              - 2 - rewrite no tags
///                              - 1 or other integer values - rewrite all tags
/// \param tags                  (optional) Required only rewriteTagsStrategy dims is greater than 3.
///                              The set specifying which exactly tags (not) to transfer.
///                              For more information on how to construct this set see applyDicomTagsRewriteStrategy().
/// \param slice                 (optional) Required only when GI comparison was performed in 2.5D od 2dfrom3D version.
///                              Specifies for which slice (in the given orientation) the GI comparison was performed.
///                              First slice has number 0.
///
///////////////////////////////////////////////////////////////////////////////
    void saveImage(std::string filepath, gdcm::File& oldFile, double* gamma,
        int xNumber, int yNumber, int zNumber, int precision, double fillValue,
        int rewriteTagsStrategyId, std::set<std::pair<int, int> > tags = (std::set<std::pair<int, int> >()),
        int slice = -1);

protected:

    /// \brief Copies Gamma Index array to a DICOM matrix.
    ///
    /// \note Values of parameters: xNumber, yNumber, zNumber, plane, slice
    ///       should be the same as those passed to calculateGamma() function.
    ///
    /// \param newFile    Object in which the new DICOM is constructed.
    /// \param pf         The pixel format of DICOM image (GDCM parameter)
    /// \param pi         The photometric interpretation of DICOM image (GDCM parameter)
    /// \param gamma      The calculated gamma array.
    /// \param xNumber    Number of voxels along x axis in the gamma image.
    /// \param yNumber    Number of voxels along y axis in the gamma image.
    /// \param zNumber    Number of voxels along z axis in the gamma image.
    /// \param rescale    Value by which the GI values are rescaled before inserting into DICOM,
    ///                   in order to preserve the precision.
    /// \param fillValue  Value that will be assigned to voxels for which the GI comparison was not performed.
    /// \param slice      (optional) Required only when GI comparison was performed in 2.5D od 2dfrom3D version.
    ///                   Specifies for which slice (in the given orientation) the GI comparison was performed.
    ///                   First slice has number 0.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void insertFrames(gdcm::File& newFile, gdcm::PixelFormat pf, gdcm::PhotometricInterpretation pi,
        double* gamma, int xNumber, int yNumber, int zNumber, int rescale, double fillValue, int slice) = 0;

private:
    void setDicomRescaleTags(gdcm::DataSet& newDataSet, int rescale);
};


class DicomWriter2D : public DicomWriter
{
public:
    DicomWriter2D();

    virtual ~DicomWriter2D();

protected:
    virtual void insertFrames(gdcm::File& newFile, gdcm::PixelFormat pf, gdcm::PhotometricInterpretation pi,
        double* gamma, int xNumber, int yNumber, int, int rescale, double fillValue, int);
};

class DicomWriter3D : public DicomWriter
{
public:
    DicomWriter3D();

    virtual ~DicomWriter3D();

protected:
    virtual void insertFrames(gdcm::File& newFile, gdcm::PixelFormat pf, gdcm::PhotometricInterpretation pi,
        double* gamma, int xNumber, int yNumber, int zNumber, int rescale, double fillValue, int);
};

class DicomWriter3DSliceXY : public DicomWriter
{
public:
    DicomWriter3DSliceXY();

    virtual ~DicomWriter3DSliceXY();

protected:
    virtual void insertFrames(gdcm::File& newFile, gdcm::PixelFormat pf, gdcm::PhotometricInterpretation pi,
        double* gamma, int xNumber, int yNumber, int zNumber, int rescale, double fillValue, int zSlice);
};

class DicomWriter3DSliceXZ : public DicomWriter
{
public:
    DicomWriter3DSliceXZ();

    virtual ~DicomWriter3DSliceXZ();

protected:
    virtual void insertFrames(gdcm::File& newFile, gdcm::PixelFormat pf, gdcm::PhotometricInterpretation pi,
        double* gamma, int xNumber, int yNumber, int zNumber, int rescale, double fillValue, int ySlice);
};

class DicomWriter3DSliceYZ : public DicomWriter
{
public:
    DicomWriter3DSliceYZ();

    virtual ~DicomWriter3DSliceYZ();

protected:
    virtual void insertFrames(gdcm::File& newFile, gdcm::PixelFormat pf, gdcm::PhotometricInterpretation pi,
        double* gamma, int xNumber, int yNumber, int zNumber, int rescale, double fillValue, int xSlice);
};

#endif