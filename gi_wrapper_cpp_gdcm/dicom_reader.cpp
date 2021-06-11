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
//#include "wrapper_logger.h"

using namespace gdcm;
using namespace std;

DicomReader::DicomReader()
{
}

double DicomReader::acquireStart(DataSet& dataSet, int dim)
{
    Attribute<0x0020, 0x0032> imagePosition;
    imagePosition.SetFromDataSet(dataSet);

    return (double) imagePosition.GetValue(dim);
}

double DicomReader::acquireSpacing(DataSet& dataSet, int dim)
{
    if (dim < 2) {
        Attribute<0x0028, 0x0030> pixelSpacing;
        pixelSpacing.SetFromDataSet(dataSet);
        // We subtract dim from 1 because the order of pixel spacing values is inversed in DICOM standard.
        // More info - http://dicom.nema.org/medical/Dicom/2016b/output/chtml/part03/sect_10.7.html.
        return (double) pixelSpacing.GetValue(1 - dim);
    }
    
    else
    {
        double thirdDimSpacing = 0.0;
        Attribute<0x0008, 0x0060> modality;
        modality.SetFromDataSet(dataSet);
        if (modality.GetValue() == "RTDOSE" && 
            dataSet.FindDataElement(gdcm::Tag(0x3004, 0x000C)))
        {
            Attribute<0x3004, 0x000C> gridFrameOffsetVector;
            gridFrameOffsetVector.SetFromDataSet(dataSet);

            thirdDimSpacing = (double) gridFrameOffsetVector.GetValue(1) -
                (double) gridFrameOffsetVector.GetValue(0);
        }

        if (thirdDimSpacing == 0 && dataSet.FindDataElement(gdcm::Tag(0x0018, 0x0088)))
        {
            Attribute<0x0018, 0x0088> spacingBetweenSlices;
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

