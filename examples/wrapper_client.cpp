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

#include <iostream>
#include "gdcmReader.h"
#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmDict.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"
#include "gdcmImageReader.h"
#include "gdcmImageWriter.h"
#include "additions.h"
#include "dispatcher.h"
#include "wrapper.h"
#include "core_logger.h"
#include "wrapper_logger.h"
#include <set>

using namespace std;
using namespace gdcm;

int main() {
    /************** Parameters **************/
    string refFile = "images/dicoms/Dose_3D_org.dcm",
           tarFile = "images/dicoms/Dose_3D_meas.dcm",
           outFile = "images/dicoms/Dose_3D_org_meas.dcm";

    int algorithm = 2, precision = 3, tagStrategy = 4;
    double dims = 3.0, percentage = 3.0, dta = 3.0, referenceValue = -1.0, limit = 2.0, fillValue = 1.1*limit;
    bool local = false;

    // The tag set used to determine which tags should be transferred from reference DICOM into result DICOM file.
    // For more information on how to construct this set see applyDicomTagsRewriteStrategy().
    set <pair<int, int>> tagSet = {
                    {0x0018, 0x0050},	// Slice Thicknes
                    {0x0020, 0x0032},	// Image Position (Patient)
                    {0x0020, 0x0037},	// Image Orientation (Patient)
                    {0x0028, 0x0002},	// Samples per Pixel
                    {0x0028, 0x0008},	// Number of Frames
                    {0x0028, 0x0010},	// Rows
                    {0x0028, 0x0011},	// Columns
                    {0x0028, 0x0030},	// Pixel Spacing
                    {0x0028, 0x0100},	// Bits Allocated
                    {0x0028, 0x0101},	// Bits Stored
                    {0x0028, 0x0102},	// High Bit
                    {0x0028, 0x0103},	// Pixel Representation
                    {0x0028, 0x0004},	// Photometric Interpretation
                    {0x0028, 0x1052},	// Rescale Intercept
                    {0x0028, 0x1053},	// Rescale Slope
                    {0x3004, 0x0002},	// Dose Units
                    {0x3004, 0x0004},	// Dose Type
                    {0x3004, 0x000A},	// Dose Summation Type
                    {0x3004, 0x000C},	// Grid Frame Offset Vector
                    {0x3004, 0x000E},	// Dose Grid Scaling
                    {0x3004, 0x0014}	// Tissue Heterogeneity Correction
    };
    /***************************************/

    //initialize loggers service
    initializeCoreLogger();
    initializeWrapperLogger();

    // Variables in which images parameters will be stored.
    int refNDims, tarNDims, refXNumber, refYNumber, refZNumber, tarXNumber, tarYNumber, tarZNumber;
    double refXStart, refYStart, refZStart, tarXStart, tarYStart, tarZStart,
           refXSpacing, refYSpacing, refZSpacing, tarXSpacing, tarYSpacing, tarZSpacing;

    // Load reference and target images from DICOM file. These references need to be passed to the other functions.
    gdcm::File& refDataSet = loadDicom(refFile);
    gdcm::File& tarDataSet = loadDicom(tarFile);

    // Acquire images from the dataSets.
    double* reference = acquireImage(refDataSet, refNDims,
                                     refXStart, refXSpacing, refXNumber,
                                     refYStart, refYSpacing, refYNumber,
                                     refZStart, refZSpacing, refZNumber);
    double* target = acquireImage(tarDataSet, tarNDims,
                                  tarXStart, tarXSpacing, tarXNumber,
                                  tarYStart, tarYSpacing, tarYNumber,
                                  tarZStart, tarZSpacing, tarZNumber);

    if (refNDims != tarNDims || (tarNDims != dims && (tarNDims != 3 || dims < 3)))
    {
        cerr << "Error. Dimensions mismatch." << endl;
        exit(-1);
    }

    // Perform Gamma Index calculation.
    double* gamma = calculateGamma(algorithm, dims, reference, target,
                                   refXStart, refXSpacing, refXNumber,
                                   refYStart, refYSpacing, refYNumber,
                                   refZStart, refZSpacing, refZNumber,
                                   tarXStart, tarXSpacing, tarXNumber,
                                   tarYStart, tarYSpacing, tarYNumber,
                                   tarZStart, tarZSpacing, tarZNumber,
                                   percentage, dta, local, referenceValue, limit);

    // Save result into a DICOM file.
    saveImage(dims, refDataSet, outFile, gamma, refXNumber, refYNumber, refZNumber, precision, fillValue, tagStrategy, tagSet);

    //Prepare statistics file.
    int gammaSize = calculateGammaArraySize(dims, refXNumber, refYNumber, refZNumber);
    string info =  createComputationDescription(refFile, tarFile, algorithm);
    string filepath = "stats.txt";
    prepareStatisticsFile(gammaSize, gamma, info, filepath);

    // Free up the memory.
    delete[] reference;
    delete[] target;
    delete[] gamma;

    return 0;
}
