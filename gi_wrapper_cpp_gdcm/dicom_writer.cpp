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

#include "dicom_writer.h"

#include <iostream>
#include <cmath>
#include <set>
#include <utility>
#include "gdcmReader.h"
#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmDict.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"
#include "gdcmImageReader.h"
#include "gdcmImageWriter.h"
#include "dicom_tag_strategies.h"

using namespace gdcm;
using namespace std;

DicomWriter::DicomWriter()
{
}

DicomWriter::~DicomWriter()
{
}

void DicomWriter::setDicomRescaleTags(DataSet& newDataSet, int rescale)
{
    if (newDataSet.FindDataElement(Tag(0x0028, 0x1053)))
    {
        DataElement newDE = DataElement(Tag(0x0028, 0x1053), (VL)(1.0 / rescale), newDataSet.GetDataElement(Tag(0x0028, 0x1053)).GetVR());
        newDataSet.Replace(newDE);
    }

    if (newDataSet.FindDataElement(Tag(0x0028, 0x1052)))
    {
        DataElement newDE = DataElement(Tag(0x0028, 0x1052), (VL)(0.0), newDataSet.GetDataElement(Tag(0x0028, 0x1052)).GetVR());
        newDataSet.Replace(newDE);
    }

    if (newDataSet.FindDataElement(Tag(0x3004, 0x000E)))
    {
        DataElement newDE = DataElement(Tag(0x3004, 0x000E), (VL)(1.0 / rescale), newDataSet.GetDataElement(Tag(0x3004, 0x000E)).GetVR());
        newDataSet.Replace(newDE);
    }
}


DicomWriter2D::DicomWriter2D()
    : DicomWriter()
{
}

DicomWriter2D::~DicomWriter2D()
{
}

void DicomWriter2D::insertFrames(File& newFile, PixelFormat pf, PhotometricInterpretation pi,
    double* gamma, int xNumber, int yNumber, int, int rescale, double fillValue, int)
{
    DataSet& newDataSet = newFile.GetDataSet();

    Image image = Image();
    image.SetNumberOfDimensions(2);
    image.SetDimension(0, xNumber);
    image.SetDimension(1, yNumber);
    image.SetPixelFormat(pf);
    image.SetPhotometricInterpretation(pi);


    {
        int pixelSize = (int)image.GetPixelFormat().GetPixelSize();
        char* buffer = new char[xNumber * yNumber * pixelSize];
        for (int j = 0; j < yNumber; j++)
        {
            for (int i = 0; i < xNumber; i++)
            {
                int toFill;
                if (!std::isnan(gamma[j * xNumber + i]))
                    toFill = (int)(gamma[j * xNumber + i] * rescale);
                else
                    toFill = (int)(fillValue * rescale);

                if (pixelSize == 1) {
                    _int8 currentValue = (_int8)toFill;
                    memcpy(&buffer[(j * xNumber + i) * pixelSize], &currentValue, pixelSize);
                }
                else if (pixelSize == 2) {
                    _int16 currentValue = (_int16)toFill;
                    memcpy(&buffer[(j * xNumber + i) * pixelSize], &currentValue, pixelSize);
                }
                else if (pixelSize == 4) {
                    _int32 currentValue = (_int32)toFill;
                    memcpy(&buffer[(j * xNumber + i) * pixelSize], &currentValue, pixelSize);
                }
                else if (pixelSize == 8) {
                    _int64 currentValue = (_int64)toFill;
                    memcpy(&buffer[(j * xNumber + i) * pixelSize], &currentValue, pixelSize);
                }
                else {
                    cerr << "Error. Wrong pixel size " << pixelSize << "." << endl;
                    exit(-1);
                }
            }
        }
        DataElement pixeldata(Tag(0x7fe0, 0x0010));
        pixeldata.SetByteValue(buffer, xNumber * yNumber * pixelSize);
        delete[] buffer;
        image.SetDataElement(pixeldata);
    }

    ImageWriter writer;
    writer.SetFile(newFile);
    writer.SetImage(image);
}

