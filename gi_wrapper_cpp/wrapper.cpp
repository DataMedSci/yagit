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

#include "wrapper.h"

#include <iostream>
#include <cstdlib>
#include "gdcmReader.h"
#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmDict.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"
#include "gdcmImageReader.h"
#include "gdcmImageWriter.h"

#include "dicom_reader.h"
#include "dicom_writer.h"
#include "wrapper_logger.h"

using namespace gdcm;
using namespace std;


File& loadDicom(string filepath)
{
    logWrapperMessage("Loading DICOM file: " + filepath);
    Reader reader;
    reader.SetFileName(filepath.c_str());
    return reader.GetFile();
}

double* acquireImage(File& file, int& dims,
    double& xStart, double& xSpacing, int& xNumber,
    double& yStart, double& ySpacing, int& yNumber,
    double& zStart, double& zSpacing, int& zNumber)
{
    logWrapperMessage("Parsing image...");
    DicomReader reader;
    return reader.acquireImage(file, dims,
        xStart, xSpacing, xNumber,
        yStart, ySpacing, yNumber,
        zStart, zSpacing, zNumber);
}

void saveImage(int dims, File& oldFile, string filepath, double* gamma, int refXNumber, int refYNumber, int refZNumber,
    int precision, double fillValue, int rewriteTagsStrategy, set <pair<int, int>> tags, int plane, int refSlice)
{
    logWrapperMessage("Creating results file at " + filepath);

    DicomWriter* writer;
    switch (dims)
    {
    case 2:
        writer = new DicomWriter2D();
        break;
    case 3:
        writer = new DicomWriter3D();
        break;
    case 4:
    case 5:
        switch (plane)
        {
        case 0:
            writer = new DicomWriter3DSliceXY();
            break;
        case 1:
            writer = new DicomWriter3DSliceXZ();
            break;
        case 2:
            writer = new DicomWriter3DSliceYZ();
            break;
        default:
            cerr << "Error. Wrong plane: " << plane << ". Should be in range 0-2." << endl;
            exit(-1);
        }
    default:
        cerr << "Error. Wrong dimensions number: " << dims << ". Should be in range 2-5." << endl;
        exit(-1);
    }
    writer->saveImage(filepath, oldFile, gamma, refXNumber, refYNumber, refZNumber, precision, fillValue, rewriteTagsStrategy, tags, refSlice);
    delete writer;
    logWrapperMessage("DICOM saved successfully.");
}
