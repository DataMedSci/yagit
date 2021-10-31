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
#include <imebra/imebra.h>
#include "wrapper_logger.h"

using namespace imebra;
using namespace std;


DicomReader::DicomReader()
{
}

std::unique_ptr<double[]> DicomReader::acquireImage(const imebra::DataSet& dataSet, int& dims,
                                  double& xStart, double& xSpacing, int& xNumber,
                                  double& yStart, double& ySpacing, int& yNumber,
                                  double& zStart, double& zSpacing, int& zNumber)
{
    unique_ptr <imebra::Image> im(const_cast<DataSet&>(dataSet).getImage(0));
    xNumber = im->getWidth();
    yNumber = im->getHeight();
    zNumber = 1;
    //zNumber = const_cast<DataSet&>(dataSet).getUnsignedLong(TagId(tagId_t::NumberOfFrames_0028_0008), 0, 1);

    dims = 2;
    /*if (zNumber == 1)
    {
        dims--;
        if (yNumber == 1)
            dims--;
    }*/

    //xStart = acquireStart(dataSet, 0);
    //yStart = acquireStart(dataSet, 1);
    //zStart = acquireStart(dataSet, 2);

    //xSpacing = acquireSpacing(dataSet, 0);
    //ySpacing = acquireSpacing(dataSet, 1);
    //if (dims == 3)
    //    zSpacing = acquireSpacing(dataSet, 2);
    /*
    if ((dims > 0 && xSpacing <= 0) || (dims > 1 && ySpacing <= 0) || (dims > 2 && zSpacing <= 0))
    {
        cerr << "Error. Non-positive spacing." << endl;
        exit(-1);
    }
    if ((dims > 0 && xNumber <= 0) || (dims > 1 && yNumber <= 0) || (dims > 2 && zNumber <= 0))
    {
        cerr << "Error. Non-positive resolution." << endl;
        exit(-1);
    }*/


    double rescaleSlope = 1.0;
    double rescaleIntercept = 0.0;
    if (const_cast<DataSet&>(dataSet).bufferExists(TagId(tagId_t::DoseGridScaling_3004_000E), 0))
        rescaleSlope = const_cast<DataSet&>(dataSet).getDouble(TagId(tagId_t::DoseGridScaling_3004_000E), 0);
    else
    {
        if (const_cast<DataSet&>(dataSet).bufferExists(TagId(tagId_t::RescaleSlope_0028_1053), 0))
            rescaleSlope = const_cast<DataSet&>(dataSet).getDouble(TagId(tagId_t::RescaleSlope_0028_1053), 0);

        if (const_cast<DataSet&>(dataSet).bufferExists(TagId(tagId_t::RescaleIntercept_0028_1052), 0))
            rescaleIntercept = const_cast<DataSet&>(dataSet).getDouble(TagId(tagId_t::RescaleIntercept_0028_1052), 0);
    }

    std::unique_ptr<double[]> data_array = std::make_unique<double[]>(xNumber * yNumber * zNumber);
    for (int k = 0; k < zNumber; k++)
    {
        unique_ptr <imebra::Image> image(const_cast<DataSet&>(dataSet).getImage(k));
        unique_ptr <ReadingDataHandlerNumeric> dataHandler(image->getReadingDataHandler());
        for (int j = 0; j < yNumber; j++)
        {
            for (int i = 0; i < xNumber; i++)
            {
                data_array[(k * yNumber + j) * xNumber + i] = rescaleSlope * dataHandler->getDouble(j * xNumber + i) + rescaleIntercept;
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

    return data_array;
}

double DicomReader::acquireStart(const imebra::DataSet& dataSet, int dim)
{
    return const_cast<DataSet&>(dataSet).getDouble(TagId(tagId_t::ImagePositionPatient_0020_0032), dim);
}

double DicomReader::acquireSpacing(const imebra::DataSet& dataSet, int dim)
{
    if (dim < 2)
        // We subtract dim from 1 because the order of pixel spacing values is inversed in DICOM standard.
        // More info - http://dicom.nema.org/medical/Dicom/2016b/output/chtml/part03/sect_10.7.html.
        return const_cast<DataSet&>(dataSet).getDouble(TagId(tagId_t::PixelSpacing_0028_0030), 1 - dim);
    else
    {
        double thirdDimSpacing = 0.0;
        if (const_cast<DataSet&>(dataSet).getString(TagId(tagId_t::Modality_0008_0060), 0) == "RTDOSE" &&
            const_cast<DataSet&>(dataSet).bufferExists(TagId(tagId_t::GridFrameOffsetVector_3004_000C), 0))
        {
            thirdDimSpacing = const_cast<DataSet&>(dataSet).getDouble(TagId(tagId_t::GridFrameOffsetVector_3004_000C), 1) -
                const_cast<DataSet&>(dataSet).getDouble(TagId(tagId_t::GridFrameOffsetVector_3004_000C), 0);
        }

        if (thirdDimSpacing == 0 && const_cast<DataSet&>(dataSet).bufferExists(TagId(tagId_t::SpacingBetweenSlices_0018_0088), 0))
        {
            thirdDimSpacing = const_cast<DataSet&>(dataSet).getDouble(TagId(tagId_t::SpacingBetweenSlices_0018_0088), 0);
        }

        if (thirdDimSpacing == 0)
        {
            cerr << "Error. Failed to determine spacing in third dimension" << endl;
            exit(-1);
        }

        return thirdDimSpacing;
    }
}
