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
#include <imebra/imebra.h>
#include "dicom_tag_stategies.h"

using namespace imebra;
using namespace std;


DicomWriter::DicomWriter()
{
}

DicomWriter::~DicomWriter()
{
}

void DicomWriter::saveImage(string filepath, DataSet* oldDataSet, double* gamma,
                            int xNumber, int yNumber, int zNumber, int precision, double fillValue,
                            int rewriteTagsStrategyId, set<pair<int, int> > tags, int slice)
{

    unique_ptr <imebra::Image> oldImage(oldDataSet->getImage(0));
    bitDepth_t depth = oldImage->getDepth();
    string colorSpace = oldImage->getColorSpace();
    uint32_t highBit = oldDataSet->getUnsignedLong(TagId(tagId_t::BitsAllocated_0028_0100), 0) - 1;
    int rescale = pow(10, precision);

    // Set dicom tags connected with pixel data.
    DataSet newDataSet("1.2.840.10008.1.2.1");

    insertFrames(newDataSet, depth, colorSpace, highBit, gamma, xNumber, yNumber, zNumber, rescale, fillValue, slice);

    // Copy seleceted dicom tags.
    applyDicomTagsRewriteStrategy(oldDataSet, &newDataSet, rewriteTagsStrategyId, tags);

    // Set tags allowing to rescale value stored in DICOM pixel array into expected units.
    setDicomRescaleTags(&newDataSet, rescale);

    // Insert an image into the dataset.
    CodecFactory::save(newDataSet, filepath, codecType_t::dicom);
}

void DicomWriter::setDicomRescaleTags(DataSet* newDataSet, int rescale)
{
    if (newDataSet->bufferExists(TagId(tagId_t::RescaleSlope_0028_1053), 0))
        newDataSet->setDouble(TagId(tagId_t::RescaleSlope_0028_1053), 1.0 / rescale);

    if (newDataSet->bufferExists(TagId(tagId_t::RescaleIntercept_0028_1052), 0))
        newDataSet->setDouble(TagId(tagId_t::RescaleIntercept_0028_1052), 0.0);

    if (newDataSet->bufferExists(TagId(tagId_t::DoseGridScaling_3004_000E), 0))
        newDataSet->setDouble(TagId(tagId_t::DoseGridScaling_3004_000E), 1.0 / rescale);
}


DicomWriter2D::DicomWriter2D()
    : DicomWriter()
{
}

DicomWriter2D::~DicomWriter2D()
{
}

void DicomWriter2D::insertFrames(DataSet& newDataSet, bitDepth_t depth, string colorSpace, uint32_t highBit,
                                 double* gamma, int xNumber, int yNumber, int, int rescale, double fillValue, int)
{
    imebra::Image image(xNumber, yNumber, depth, colorSpace, highBit);
    {
        unique_ptr<WritingDataHandlerNumeric> dataHandler(image.getWritingDataHandler());
        for (int j = 0; j < yNumber; j++)
        {
            for (int i = 0; i < xNumber; i++)
            {
                if (!std::isnan(gamma[j * xNumber + i]))
                    dataHandler->setUnsignedLong(j * xNumber + i, (int) (gamma[j * xNumber + i] * rescale));
                else
                    dataHandler->setUnsignedLong(j * xNumber + i, (int) (fillValue * rescale));
            }
        }
    }
    newDataSet.setImage(0, image, imageQuality_t::high);
}


DicomWriter3D::DicomWriter3D()
    : DicomWriter()
{
}

DicomWriter3D::~DicomWriter3D()
{
}

void DicomWriter3D::insertFrames(DataSet& newDataSet, bitDepth_t depth, string colorSpace, uint32_t highBit,
                                 double* gamma, int xNumber, int yNumber, int zNumber, int rescale, double fillValue, int)
{
    for (int k = 0; k < zNumber; k++)
    {
        imebra::Image image(xNumber, yNumber, depth, colorSpace, highBit);
        {
            unique_ptr <WritingDataHandlerNumeric> dataHandler(image.getWritingDataHandler());
            for (int j = 0; j < yNumber; j++)
            {
                for (int i = 0; i < xNumber; i++)
                {
                    if (!std::isnan(gamma[(k * yNumber + j) * xNumber + i]))
                        dataHandler->setUnsignedLong(j * xNumber + i, (int) (gamma[(k * yNumber + j) * xNumber + i] * rescale));
                    else
                        dataHandler->setUnsignedLong(j * xNumber + i, (int) (fillValue * rescale));
                }
            }
        }
        newDataSet.setImage(k, image, imageQuality_t::high);
    }
}


DicomWriter3DSliceXY::DicomWriter3DSliceXY()
    : DicomWriter()
{
}

DicomWriter3DSliceXY::~DicomWriter3DSliceXY()
{
}

void DicomWriter3DSliceXY::insertFrames(DataSet& newDataSet, bitDepth_t depth, string colorSpace, uint32_t highBit,
                                        double* gamma, int xNumber, int yNumber, int zNumber,
                                        int rescale, double fillValue, int zSlice)
{
    if (zSlice < 0 || zSlice >= zNumber)
    {
        cerr << "Error. zSlice outside the image." << endl;
        exit(-1);
    }

    for (int k = 0; k < zNumber; k++)
    {
        imebra::Image image(xNumber, yNumber, depth, colorSpace, highBit);
        {
            unique_ptr <WritingDataHandlerNumeric> dataHandler(image.getWritingDataHandler());
            for (int j = 0; j < yNumber; j++)
            {
                for (int i = 0; i < xNumber; i++)
                {
                    if (k == zSlice && !std::isnan(gamma[j * xNumber + i]))
                        dataHandler->setUnsignedLong(j * xNumber + i, (int) (gamma[j * xNumber + i] * rescale));
                    else
                        dataHandler->setUnsignedLong(j * xNumber + i, (int) (fillValue * rescale));
                }
            }
        }
        newDataSet.setImage(k, image, imageQuality_t::high);
    }
}


DicomWriter3DSliceXZ::DicomWriter3DSliceXZ()
    : DicomWriter()
{
}

DicomWriter3DSliceXZ::~DicomWriter3DSliceXZ()
{
}

void DicomWriter3DSliceXZ::insertFrames(DataSet& newDataSet, bitDepth_t depth, string colorSpace, uint32_t highBit,
                                        double* gamma, int xNumber, int yNumber, int zNumber,
                                        int rescale, double fillValue, int ySlice)
{
    if (ySlice < 0 || ySlice >= yNumber)
    {
        cerr << "Error. ySlice outside the image." << endl;
        exit(-1);
    }

    for (int k = 0; k < zNumber; k++)
    {
        imebra::Image image(xNumber, yNumber, depth, colorSpace, highBit);
        {
            unique_ptr <WritingDataHandlerNumeric> dataHandler(image.getWritingDataHandler());
            for (int j = 0; j < yNumber; j++)
            {
                for (int i = 0; i < xNumber; i++)
                {
                    if (j == ySlice && !std::isnan(gamma[k * xNumber + i]))
                        dataHandler->setUnsignedLong(j * xNumber + i, (int) (gamma[k * xNumber + i] * rescale));
                    else
                        dataHandler->setUnsignedLong(j * xNumber + i, (int) (fillValue * rescale));
                }
            }
        }
        newDataSet.setImage(k, image, imageQuality_t::high);
    }
}


DicomWriter3DSliceYZ::DicomWriter3DSliceYZ()
    : DicomWriter()
{
}

DicomWriter3DSliceYZ::~DicomWriter3DSliceYZ()
{
}

void DicomWriter3DSliceYZ::insertFrames(DataSet& newDataSet, bitDepth_t depth, string colorSpace, uint32_t highBit,
                                        double* gamma, int xNumber, int yNumber, int zNumber,
                                        int rescale, double fillValue, int xSlice)
{
    if (xSlice < 0 || xSlice >= xNumber)
    {
        cerr << "Error. xSlice outside the image." << endl;
        exit(-1);
    }

    for (int k = 0; k < zNumber; k++)
    {
        imebra::Image image(xNumber, yNumber, depth, colorSpace, highBit);
        {
            unique_ptr <WritingDataHandlerNumeric> dataHandler(image.getWritingDataHandler());
            for (int j = 0; j < yNumber; j++)
            {
                for (int i = 0; i < xNumber; i++)
                {
                    if (i == xSlice && !std::isnan(gamma[k * yNumber + j]))
                        dataHandler->setUnsignedLong(j * xNumber + i, (int) (gamma[k * yNumber + j] * rescale));
                    else
                        dataHandler->setUnsignedLong(j * xNumber + i, (int) (fillValue * rescale));
                }
            }
        }
        newDataSet.setImage(k, image, imageQuality_t::high);
    }
}
