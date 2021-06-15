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

#endif