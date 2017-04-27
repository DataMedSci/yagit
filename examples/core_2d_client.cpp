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
#include <fstream>
#include <iomanip>
#include <string>
#include "additions.h"
#include "dispatcher.h"

using namespace std;

#ifdef WIN32
    #define OS_SEP "\\"
#else
    #define OS_SEP "/"
#endif


double* read2DMatrixFromTxt(string filepath,
                            double &xStart, double &xSpacing, int &xNumber,
                            double &yStart, double &ySpacing, int &yNumber)
{
    //read ref
    ifstream file(filepath.c_str());
    double *matrix = NULL;
    if (file.is_open())
    {
        file >> xStart;
        file >> yStart;
        file >> xSpacing;
        file >> ySpacing;
        file >> xNumber;
        file >> yNumber;

        matrix = new double[xNumber * yNumber];
        for (int j = 0; j < yNumber; j++)
            for (int i = 0; i < xNumber; i++)
                file >> matrix[j * xNumber + i];
    }
    return matrix;
}

void printMatrix(int xNumber, int yNumber, double *matrix)
{
    for (int j = 0; j < yNumber; j++)
    {
        for (int i = 0; i < xNumber; i++)
            cout << matrix[j * xNumber + i] << " ";
        cout << endl;
    }
}

void printMatrixProperites(double xStart, double xSpacing, int xNumber, double yStart, double ySpacing,
                             int yNumber)
{
    cout << setprecision(10) << xStart << endl;
    cout << setprecision(10) << xSpacing << endl;
    cout << xNumber << endl;
    cout << setprecision(10) << yStart << endl;
    cout << setprecision(10) << ySpacing << endl;
    cout << yNumber << endl;
}
int main()
{
    /************** Parameters **************/
    string referencePath = "images" + string(OS_SEP) + "1.txt";
    string targetPath = "images" + string(OS_SEP) + "2.txt";

    int algorithm = 2, dims = 2;
    double percentage = 3.0, dta = 3.0, referenceValue = -1, limit = 20.0;
    bool local = false;
    /***************************************/

    // Variables in which images parameters will be stored.
    int refXNumber = 0, refYNumber = 0, refZNumber = 0, tarXNumber = 0, tarYNumber = 0, tarZNumber = 0;
    double refXStart = 0.0, refYStart = 0.0, refZStart = 0.0, tarXStart = 0.0, tarYStart = 0.0, tarZStart = 0.0,
           refXSpacing = 0.0, refYSpacing = 0.0, refZSpacing = 0.0, tarXSpacing = 0.0, tarYSpacing = 0.0, tarZSpacing = 0.0;


    // Read reference and target images.
    double* reference = read2DMatrixFromTxt(referencePath,
                                            refXStart, refXSpacing, refXNumber,
                                            refYStart, refYSpacing, refYNumber);
    double* target = read2DMatrixFromTxt(targetPath,
                                         tarXStart, tarXSpacing, tarXNumber,
                                         tarYStart, tarYSpacing, tarYNumber);

    printMatrixProperites(refXStart, refXSpacing, refXNumber, refYStart, refYSpacing, refYNumber);
    printMatrixProperites(tarXStart, tarXSpacing, tarXNumber, tarYStart, tarYSpacing, tarYNumber);

    cout << imageMin(refXNumber * refYNumber, reference) << " " << imageMax(refXNumber * refYNumber, reference) << endl;
    cout << imageMin(tarXNumber * tarYNumber, target) << " " << imageMax(tarXNumber * tarYNumber, target) << endl;

    //(Optionally) You can apply a linear mapping to parsed images.
    applyLinearMappingToImage(refXNumber * refYNumber, reference, 1.1, 0.0);  // This will multiply the image by 1.1 and add 0.0.

    //(Optionally) You can also filter some noise form the image.
    applyNoiseFilteringToImage(refXNumber * refYNumber, reference, 5.0);  // This will filter evey value in reference image
                                                                          // below threshold = minVal + 5% * (maxVal - minVal).

    cout << imageMin(refXNumber * refYNumber, reference) << " " << imageMax(refXNumber * refYNumber, reference) << endl;
    cout << imageMin(tarXNumber * tarYNumber, target) << " " << imageMax(tarXNumber * tarYNumber, target) << endl;

    // Perform Gamma Index calculation.
    double *gamma = calculateGamma(algorithm, dims, reference, target,
                                   refXStart, refXSpacing, refXNumber,
                                   refYStart, refYSpacing, refYNumber,
                                   refZStart, refZSpacing, refZNumber,
                                   tarXStart, tarXSpacing, tarXNumber,
                                   tarYStart, tarYSpacing, tarYNumber,
                                   tarZStart, tarZSpacing, tarZNumber,
                                   percentage, dta, local, referenceValue, limit);

    printMatrix(refXNumber, refYNumber, gamma);

    // Free up the memory.
    delete [] reference;
    delete [] target;
    delete [] gamma;

    return 0;
}
