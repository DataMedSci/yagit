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

#include "dicom_reader.h"

#include <iostream>
#include "gdcmReader.h"
#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmDict.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"
#include "gdcmImageReader.h"
#include "wrapper_logger.h"
#include "dicom_tags.h"

using namespace gdcm;
using namespace std;

DicomReader::DicomReader()
{
}

double* DicomReader::acquireImage(File& file, int& dims,
    double& xStart, double& xSpacing, int& xNumber,
    double& yStart, double& ySpacing, int& yNumber,
    double& zStart, double& zSpacing, int& zNumber)
{
    ImageReader reader;
    reader.SetFile(file);

    Image im = reader.GetImage();
    xNumber = im.GetDimension(0);
    yNumber = im.GetDimension(1);
    zNumber = im.GetDimension(2);

    dims = 3;
    if (zNumber == 1)
    {
        dims--;
        if (yNumber == 1)
            dims--;
    }

    DataSet dataSet = reader.GetFile().GetDataSet();

    xStart = acquireStart(dataSet, 0);
    yStart = acquireStart(dataSet, 1);
    zStart = acquireStart(dataSet, 2);

    xSpacing = acquireSpacing(dataSet, 0);
    ySpacing = acquireSpacing(dataSet, 1);
    if (dims == 3)
        zSpacing = acquireSpacing(dataSet, 2);

    if ((dims > 0 && xSpacing <= 0) || (dims > 1 && ySpacing <= 0) || (dims > 2 && zSpacing <= 0))
    {
        cerr << "Error. Non-positive spacing." << endl;
        exit(-1);
    }
    if ((dims > 0 && xNumber <= 0) || (dims > 1 && yNumber <= 0) || (dims > 2 && zNumber <= 0))
    {
        cerr << "Error. Non-positive resolution." << endl;
        exit(-1);
    }

    double rescaleSlope = im.GetSlope();
    double rescaleIntercept = im.GetIntercept();

    double* array = new double[xNumber * yNumber * zNumber];
    char* buffer = new char[im.GetBufferLength()];
    int pixelSize = (int) im.GetPixelFormat().GetPixelSize();
    for (int k = 0; k < zNumber; k++)
    {
        for (int j = 0; j < yNumber; j++)
        {
            for (int i = 0; i < xNumber; i++)
            {
                if (pixelSize == 1) {
                    int8_t currentValue;
                    memcpy(&currentValue, &buffer[((k * yNumber + j) * xNumber + i) * pixelSize], pixelSize);
                    array[(k * yNumber + j) * xNumber + i] = rescaleSlope * currentValue + rescaleIntercept;
                }
                else if (pixelSize == 2) {
                    int16_t currentValue;
                    memcpy(&currentValue, &buffer[((k * yNumber + j) * xNumber + i) * pixelSize], pixelSize);
                    array[(k * yNumber + j) * xNumber + i] = rescaleSlope * currentValue + rescaleIntercept;
                }
                else if (pixelSize == 4) {
                    int32_t currentValue;
                    memcpy(&currentValue, &buffer[((k * yNumber + j) * xNumber + i) * pixelSize], pixelSize);
                    array[(k * yNumber + j) * xNumber + i] = rescaleSlope * currentValue + rescaleIntercept;
                }
                else if (pixelSize == 8) {
                    int64_t currentValue;
                    memcpy(&currentValue, &buffer[((k * yNumber + j) * xNumber + i) * pixelSize], pixelSize);
                    array[(k * yNumber + j) * xNumber + i] = rescaleSlope * currentValue + rescaleIntercept;
                }
                else {
                    cerr << "Error. Wrong pixel size " << pixelSize << "." << endl;
                    exit(-1);
                }
            }
        }
    }


     logWrapperMessage("Acquired " + to_string(dims) + "-dimensional image.");

    string parameterLog = "Image parameters: ";
    if (dims >= 1)
    {
        parameterLog += to_string(xStart) + ", " +
            to_string(xSpacing) + ", " +
            to_string(xNumber);
    }
    if (dims >= 2)
    {
        parameterLog += " | " +
            to_string(yStart) + ", " +
            to_string(ySpacing) + ", " +
            to_string(yNumber);
    }
    if (dims >= 3)
    {
        parameterLog += " | " +
            to_string(zStart) + ", " +
            to_string(zSpacing) + ", " +
            to_string(zNumber);
    }
     logWrapperMessage(parameterLog);

    return array;
}


double DicomReader::acquireStart(DataSet& dataSet, int dim)
{
    Attribute<DCM_IMAGE_POSITION> imagePosition;
    imagePosition.SetFromDataSet(dataSet);

    return (double) imagePosition.GetValue(dim);
}

double DicomReader::acquireSpacing(DataSet& dataSet, int dim)
{
    if (dim < 2) {
        Attribute<DCM_PIXEL_SPACING> pixelSpacing;
        pixelSpacing.SetFromDataSet(dataSet);
        // We subtract dim from 1 because the order of pixel spacing values is inversed in DICOM standard.
        // More info - http://dicom.nema.org/medical/Dicom/2016b/output/chtml/part03/sect_10.7.html.
        return (double) pixelSpacing.GetValue(1 - dim);
    }
    
    else
    {
        double thirdDimSpacing = 0.0;
        Attribute<DCM_MODALITY> modality;
        modality.SetFromDataSet(dataSet);
        if (modality.GetValue() == "RTDOSE" && 
            dataSet.FindDataElement(gdcm::Tag(DCM_GRID_FRAME_OFFSET_VECTOR)))
        {
            Attribute<DCM_GRID_FRAME_OFFSET_VECTOR> gridFrameOffsetVector;
            gridFrameOffsetVector.SetFromDataSet(dataSet);

            thirdDimSpacing = (double) gridFrameOffsetVector.GetValue(1) -
                (double) gridFrameOffsetVector.GetValue(0);
        }

        if (thirdDimSpacing == 0 && dataSet.FindDataElement(gdcm::Tag(DCM_SPACING_BETWEEN_SLICES)))
        {
            Attribute<DCM_SPACING_BETWEEN_SLICES> spacingBetweenSlices;
            spacingBetweenSlices.SetFromDataSet(dataSet);
            thirdDimSpacing = (double) spacingBetweenSlices.GetValue();
        }

        if (thirdDimSpacing == 0)
        {
            cerr << "Error. Failed to determine spacing in third dimension" << endl;
            exit(-1);
        }

        return thirdDimSpacing;
    }
}

