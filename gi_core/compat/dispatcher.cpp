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
 
#include "dispatcher.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include "image.h"
#include "solver.h"
#include "core_logger.h"

using namespace std;


Image1D* acquireImage1D(double* array,
                        double xStart, double xSpacing, int xNumber)
{
    if (xSpacing <= 0)
    {
        cerr << "Error. Non-positive spacing." << endl;
        exit(-1);
    }
    if (xNumber <= 0)
    {
        cerr << "Error. Non-positive resolution." << endl;
        exit(-1);
    }

    vector<double> start(1);
    vector<double> spacing(1);
    vector<double> data(xNumber);

    start[0] = xStart;
    spacing[0] = xSpacing;

    for (int i = 0; i < xNumber; i++)
        data[i] = array[i];

    Image1D* image = new Image1D(start, spacing, data);
    return image;
}

Image2D* acquireImage2D(double* array,
                        double xStart, double xSpacing, int xNumber,
                        double yStart, double ySpacing, int yNumber)
{
    if (xSpacing <= 0 || ySpacing <= 0)
    {
        cerr << "Error. Non-positive spacing." << endl;
        exit(-1);
    }
    if (xNumber <= 0 || yNumber <= 0)
    {
        cerr << "Error. Non-positive resolution." << endl;
        exit(-1);
    }

    vector<double> start(2);
    vector<double> spacing(2);
    vector<vector<double> > data(yNumber, vector<double>(xNumber));

    start[0] = xStart;
    start[1] = yStart;
    spacing[0] = xSpacing;
    spacing[1] = ySpacing;

    for (int j = 0; j < yNumber; j++)
        for (int i = 0; i < xNumber; i++)
            data[j][i] = array[j * xNumber + i];

    Image2D* image = new Image2D(start, spacing, data);
    return image;
}

Image3D* acquireImage3D(double* array,
                        double xStart, double xSpacing, int xNumber,
                        double yStart, double ySpacing, int yNumber,
                        double zStart, double zSpacing, int zNumber)
{
    if (xSpacing <= 0 || ySpacing <= 0 || zSpacing <= 0)
    {
        cerr << "Error. Non-positive spacing." << endl;
        exit(-1);
    }
    if (xNumber <= 0 || yNumber <= 0 || zNumber <= 0)
    {
        cerr << "Error. Non-positive resolution." << endl;
        exit(-1);
    }

    vector<double> start(3);
    vector<double> spacing(3);
    vector<vector<vector<double> > > data(zNumber, vector<vector<double> >(yNumber, vector<double>(xNumber)));

    start[0] = xStart;
    start[1] = yStart;
    start[2] = zStart;
    spacing[0] = xSpacing;
    spacing[1] = ySpacing;
    spacing[2] = zSpacing;

    for (int k = 0; k < zNumber; k++)
        for (int j = 0; j < yNumber; j++)
            for (int i = 0; i < xNumber; i++)
                data[k][j][i] = array[(k * yNumber + j) * xNumber + i];

    Image3D* image = new Image3D(start, spacing, data);
    return image;
}

Image2D* acquireImage3DSliceXY(double* array,
                               double xStart, double xSpacing, int xNumber,
                               double yStart, double ySpacing, int yNumber,
                               int zNumber, int zSlice)
{
    if (xSpacing <= 0 || ySpacing <= 0)
    {
        cerr << "Error. Non-positive spacing." << endl;
        exit(-1);
    }
    if (xNumber <= 0 || yNumber <= 0)
    {
        cerr << "Error. Non-positive resolution." << endl;
        exit(-1);
    }
    if (zSlice < 0 || zSlice >= zNumber)
    {
        cerr << "Error. zSlice outside the image." << endl;
        exit(-2);
    }

    vector<double> start(2);
    vector<double> spacing(2);
    vector<vector<double> > data(yNumber, vector<double>(xNumber));

    start[0] = xStart;
    start[1] = yStart;
    spacing[0] = xSpacing;
    spacing[1] = ySpacing;

    for (int j = 0; j < yNumber; j++)
        for (int i = 0; i < xNumber; i++)
            data[j][i] = array[(zSlice * yNumber + j) * xNumber + i];

    Image2D* image = new Image2D(start, spacing, data);
    return image;
}

Image2D* acquireImage3DSliceXZ(double* array,
                               double xStart, double xSpacing, int xNumber,
                               double zStart, double zSpacing, int zNumber,
                               int yNumber, int ySlice)
{
    if (xSpacing <= 0 || zSpacing <= 0)
    {
        cerr << "Error. Non-positive spacing." << endl;
        exit(-1);
    }
    if (xNumber <= 0 || zNumber <= 0)
    {
        cerr << "Error. Non-positive resolution." << endl;
        exit(-1);
    }
    if (ySlice < 0 || ySlice >= yNumber)
    {
        cerr << "Error. ySlice outside the image." << endl;
        exit(-2);
    }

    vector<double> start(2);
    vector<double> spacing(2);
    vector<vector<double> > data(zNumber, vector<double>(xNumber));

    start[0] = xStart;
    start[1] = zStart;
    spacing[0] = xSpacing;
    spacing[1] = zSpacing;

    for (int k = 0; k < zNumber; k++)
        for (int i = 0; i < xNumber; i++)
            data[k][i] = array[(k * yNumber + ySlice) * xNumber + i];

    Image2D* image = new Image2D(start, spacing, data);
    return image;
}

Image2D* acquireImage3DSliceYZ(double* array,
                               double yStart, double ySpacing, int yNumber,
                               double zStart, double zSpacing, int zNumber,
                               int xNumber, int xSlice)
{
    if (ySpacing <= 0 || zSpacing <= 0)
    {
        cerr << "Error. Non-positive spacing." << endl;
        exit(-1);
    }
    if (yNumber <= 0 || zNumber <= 0)
    {
        cerr << "Error. Non-positive resolution." << endl;
        exit(-1);
    }
    if (xSlice < 0 || xSlice >= xNumber)
    {
        cerr << "Error. xSlice outside the image." << endl;
        exit(-2);
    }

    vector<double> start(2);
    vector<double> spacing(2);
    vector<vector<double> > data(zNumber, vector<double>(yNumber));

    start[0] = yStart;
    start[1] = zStart;
    spacing[0] = ySpacing;
    spacing[1] = zSpacing;

    for (int k = 0; k < zNumber; k++)
        for (int j = 0; j < yNumber; j++)
            data[k][j] = array[(k * yNumber + j) * xNumber + xSlice];

    Image2D* image = new Image2D(start, spacing, data);
    return image;
}

Image2D* acquireImage3DSlice(double* array,
                             double xStart, double xSpacing, int xNumber,
                             double yStart, double ySpacing, int yNumber,
                             double zStart, double zSpacing, int zNumber,
                             int plane, int slice)
{
    switch(plane)
    {
        case 0:
            return acquireImage3DSliceXY(array, xStart, xSpacing, xNumber, yStart, ySpacing, yNumber, zNumber, slice);
        case 1:
            return acquireImage3DSliceXZ(array, xStart, xSpacing, xNumber, zStart, zSpacing, zNumber, yNumber, slice);
        case 2:
            return acquireImage3DSliceYZ(array, yStart, ySpacing, yNumber, zStart, zSpacing, zNumber, xNumber, slice);
        default:
            cerr << "Error. Wrong plane: " << plane << ". Should be in range 0-2." << endl;
            exit(-1);
    }
}


double* calculateGamma1D(int algorithm, double* reference, double* target,
                         double refXStart, double refXSpacing, int refXNumber,
                         double tarXStart, double tarXSpacing, int tarXNumber,
                         double percentage, double dta, bool local,
                         double referenceValue, double limit)
{
    // Creating Images based on given arrays
    Image1D* refImage = acquireImage1D(reference,
                                       refXStart, refXSpacing, refXNumber);

    Image1D* tarImage = acquireImage1D(target,
                                       tarXStart, tarXSpacing, tarXNumber);

    // Obtaining Solver with proper algorithm version
    Solver1D* solver;
    switch(algorithm)
    {
        case 0:
            solver = new ClassicSolver1D(*refImage, *tarImage, percentage, dta);
            break;
        case 1:
            solver = new SpiralRectangleSolver1D(*refImage, *tarImage, percentage, dta);
            break;
        case 2:
            solver = new SpiralNoRectangleSolver1D(*refImage, *tarImage, percentage, dta);
            break;
        default:
            cerr << "Error. Wrong algorithm number: " << algorithm << ". Should be in range 0-2." << endl;
            exit(-1);
    }

    // Setting additional Solver parameters
    solver->setLocal(local);
    if (referenceValue > 0)
        solver->setReferenceValue(referenceValue);
    if (limit > 0)
        solver->setLimit(limit);

    // Performing calculations
    const Image1D& image = solver->calculateGamma();
    double* result = image.dataToArray();

    // Cleaning up
    delete refImage;
    delete tarImage;
    delete solver;

    return result;
}

double* calculateGamma2D(int algorithm, double* reference, double* target,
                         double refXStart, double refXSpacing, int refXNumber,
                         double refYStart, double refYSpacing, int refYNumber,
                         double tarXStart, double tarXSpacing, int tarXNumber,
                         double tarYStart, double tarYSpacing, int tarYNumber,
                         double percentage, double dta, bool local,
                         double referenceValue, double limit)
{
    // Creating Images based on given arrays
    Image2D* refImage = acquireImage2D(reference,
                                       refXStart, refXSpacing, refXNumber,
                                       refYStart, refYSpacing, refYNumber);

    Image2D* tarImage = acquireImage2D(target,
                                       tarXStart, tarXSpacing, tarXNumber,
                                       tarYStart, tarYSpacing, tarYNumber);

    // Obtaining Solver with proper algorithm version
    Solver2D* solver;
    switch(algorithm)
    {
        case 0:
            solver = new ClassicSolver2D(*refImage, *tarImage, percentage, dta);
            break;
        case 1:
            solver = new SpiralRectangleSolver2D(*refImage, *tarImage, percentage, dta);
            break;
        case 2:
            solver = new SpiralNoRectangleSolver2D(*refImage, *tarImage, percentage, dta);
            break;
        default:
            cerr << "Error. Wrong algorithm number: " << algorithm << ". Should be in range 0-2." << endl;
            exit(-1);
    }

    // Setting additional Solver parameters
    solver->setLocal(local);
    if (referenceValue > 0)
        solver->setReferenceValue(referenceValue);
    if (limit > 0)
        solver->setLimit(limit);

    // Performing calculations
    const Image2D& image = solver->calculateGamma();
    double* result = image.dataToArray();

    // Cleaning up
    delete refImage;
    delete tarImage;
    delete solver;

    return result;
}

double* calculateGamma3D(int algorithm, double* reference, double* target,
                         double refXStart, double refXSpacing, int refXNumber,
                         double refYStart, double refYSpacing, int refYNumber,
                         double refZStart, double refZSpacing, int refZNumber,
                         double tarXStart, double tarXSpacing, int tarXNumber,
                         double tarYStart, double tarYSpacing, int tarYNumber,
                         double tarZStart, double tarZSpacing, int tarZNumber,
                         double percentage, double dta, bool local,
                         double referenceValue, double limit)
{
    // Creating Images based on given arrays
    Image3D* refImage = acquireImage3D(reference,
                                       refXStart, refXSpacing, refXNumber,
                                       refYStart, refYSpacing, refYNumber,
                                       refZStart, refZSpacing, refZNumber);

    Image3D* tarImage = acquireImage3D(target,
                                       tarXStart, tarXSpacing, tarXNumber,
                                       tarYStart, tarYSpacing, tarYNumber,
                                       tarZStart, tarZSpacing, tarZNumber);

    // Obtaining Solver with proper algorithm version
    Solver3D* solver;
    switch(algorithm)
    {
        case 0:
            solver = new ClassicSolver3D(*refImage, *tarImage, percentage, dta);
            break;
        case 1:
            solver = new SpiralRectangleSolver3D(*refImage, *tarImage, percentage, dta);
            break;
        case 2:
            solver = new SpiralNoRectangleSolver3D(*refImage, *tarImage, percentage, dta);
            break;
        default:
            cerr << "Error. Wrong algorithm number: " << algorithm << ". Should be in range 0-2." << endl;
            exit(-1);
    }

    // Setting additional Solver parameters
    solver->setLocal(local);
    if (referenceValue > 0)
        solver->setReferenceValue(referenceValue);
    if (limit > 0)
        solver->setLimit(limit);

    // Performing calculations
    const Image3D& image = solver->calculateGamma();
    double* result = image.dataToArray();

    // Cleaning up
    delete refImage;
    delete tarImage;
    delete solver;

    return result;
}

double* calculateGamma2_5D(int algorithm, double* reference, double* target,
                           double refXStart, double refXSpacing, int refXNumber,
                           double refYStart, double refYSpacing, int refYNumber,
                           double refZStart, double refZSpacing, int refZNumber,
                           double tarXStart, double tarXSpacing, int tarXNumber,
                           double tarYStart, double tarYSpacing, int tarYNumber,
                           double tarZStart, double tarZSpacing, int tarZNumber,
                           double percentage, double dta, bool local, double referenceValue,
                           double limit, int plane, int slice)
{
    // Creating Images based on given arrays
    Image3D* refImage = acquireImage3D(reference,
                                       refXStart, refXSpacing, refXNumber,
                                       refYStart, refYSpacing, refYNumber,
                                       refZStart, refZSpacing, refZNumber);

    Image3D* tarImage = acquireImage3D(target,
                                       tarXStart, tarXSpacing, tarXNumber,
                                       tarYStart, tarYSpacing, tarYNumber,
                                       tarZStart, tarZSpacing, tarZNumber);

    // Obtaining Solver with proper algorithm version
    Solver3D* solver;
    switch(algorithm)
    {
        case 0:
            solver = new ClassicSolver3D(*refImage, *tarImage, percentage, dta);
            break;
        case 1:
            solver = new SpiralRectangleSolver3D(*refImage, *tarImage, percentage, dta);
            break;
        case 2:
            solver = new SpiralNoRectangleSolver3D(*refImage, *tarImage, percentage, dta);
            break;
        default:
            cerr << "Error. Wrong algorithm number: " << algorithm << ". Should be in range 0-2." << endl;
            exit(-1);
    }

    // Setting additional Solver parameters
    solver->setLocal(local);
    if (referenceValue > 0)
        solver->setReferenceValue(referenceValue);
    if (limit > 0)
        solver->setLimit(limit);

    // Performing calculations
    const Image2D& image = solver->calculateGamma2_5D(plane, slice);
    double* result = image.dataToArray();

    // Cleaning up
    delete refImage;
    delete tarImage;
    delete solver;

    return result;
}

double* calculate2DGammaFrom3D(int algorithm, double* reference, double* target,
                               double refXStart, double refXSpacing, int refXNumber,
                               double refYStart, double refYSpacing, int refYNumber,
                               double refZStart, double refZSpacing, int refZNumber,
                               double tarXStart, double tarXSpacing, int tarXNumber,
                               double tarYStart, double tarYSpacing, int tarYNumber,
                               double tarZStart, double tarZSpacing, int tarZNumber,
                               double percentage, double dta, bool local, double referenceValue,
                               double limit, int plane, int refSlice, int tarSlice)
{
    // Creating Images based on given arrays
    Image2D* refImage = acquireImage3DSlice(reference,
                                            refXStart, refXSpacing, refXNumber,
                                            refYStart, refYSpacing, refYNumber,
                                            refZStart, refZSpacing, refZNumber,
                                            plane, refSlice);

    Image2D* tarImage = acquireImage3DSlice(target,
                                            tarXStart, tarXSpacing, tarXNumber,
                                            tarYStart, tarYSpacing, tarYNumber,
                                            tarZStart, tarZSpacing, tarZNumber,
                                            plane, tarSlice);

    // Obtaining Solver with proper algorithm version
    Solver2D* solver;
    switch(algorithm)
    {
        case 0:
            solver = new ClassicSolver2D(*refImage, *tarImage, percentage, dta);
            break;
        case 1:
            solver = new SpiralRectangleSolver2D(*refImage, *tarImage, percentage, dta);
            break;
        case 2:
            solver = new SpiralNoRectangleSolver2D(*refImage, *tarImage, percentage, dta);
            break;
        default:
            cerr << "Error. Wrong algorithm number: " << algorithm << ". Should be in range 0-2." << endl;
            exit(-1);
    }

    // Setting additional Solver parameters
    solver->setLocal(local);
    if (referenceValue > 0)
        solver->setReferenceValue(referenceValue);
    if (limit > 0)
        solver->setLimit(limit);

    // Performing calculations
    const Image2D& image = solver->calculateGamma();
    double* result = image.dataToArray();

    // Cleaning up
    delete refImage;
    delete tarImage;
    delete solver;

    return result;
}


double* calculateGamma(int algorithm, int dims, double* reference, double* target,
                       double refXStart, double refXSpacing, int refXNumber,
                       double refYStart, double refYSpacing, int refYNumber,
                       double refZStart, double refZSpacing, int refZNumber,
                       double tarXStart, double tarXSpacing, int tarXNumber,
                       double tarYStart, double tarYSpacing, int tarYNumber,
                       double tarZStart, double tarZSpacing, int tarZNumber,
                       double percentage, double dta, bool local, double referenceValue,
                       double limit, int plane, int refSlice, int tarSlice)
{
    logCoreMessage("Initializing Gamma Index calculations with parameters:");
    logCoreMessage("Percentage (DD) = " + toString(percentage));
    logCoreMessage("DTA = " + toString(dta));
    logCoreMessage("limit = " + toString(limit));
    logCoreMessage("Plane = " + planeToString(plane));
    logCoreMessage("Algorithm version = " + algorithmToString(algorithm));

    switch (dims)
    {
        case 1:
            return calculateGamma1D(algorithm, reference, target,
                                    refXStart, refXSpacing, refXNumber,
                                    tarXStart, tarXSpacing, tarXNumber,
                                    percentage, dta, local,
                                    referenceValue, limit);
        case 2:
            return calculateGamma2D(algorithm, reference, target,
                                    refXStart, refXSpacing, refXNumber,
                                    refYStart, refYSpacing, refYNumber,
                                    tarXStart, tarXSpacing, tarXNumber,
                                    tarYStart, tarYSpacing, tarYNumber,
                                    percentage, dta, local,
                                    referenceValue, limit);
        case 3:
            return calculateGamma3D(algorithm, reference, target,
                                    refXStart, refXSpacing, refXNumber,
                                    refYStart, refYSpacing, refYNumber,
                                    refZStart, refZSpacing, refZNumber,
                                    tarXStart, tarXSpacing, tarXNumber,
                                    tarYStart, tarYSpacing, tarYNumber,
                                    tarZStart, tarZSpacing, tarZNumber,
                                    percentage, dta, local,
                                    referenceValue, limit);
        case 4:
            return calculateGamma2_5D(algorithm, reference, target,
                                      refXStart, refXSpacing, refXNumber,
                                      refYStart, refYSpacing, refYNumber,
                                      refZStart, refZSpacing, refZNumber,
                                      tarXStart, tarXSpacing, tarXNumber,
                                      tarYStart, tarYSpacing, tarYNumber,
                                      tarZStart, tarZSpacing, tarZNumber,
                                      percentage, dta, local, referenceValue,
                                      limit, plane, refSlice);
        case 5:
            return calculate2DGammaFrom3D(algorithm, reference, target,
                                          refXStart, refXSpacing, refXNumber,
                                          refYStart, refYSpacing, refYNumber,
                                          refZStart, refZSpacing, refZNumber,
                                          tarXStart, tarXSpacing, tarXNumber,
                                          tarYStart, tarYSpacing, tarYNumber,
                                          tarZStart, tarZSpacing, tarZNumber,
                                          percentage, dta, local, referenceValue,
                                          limit, plane, refSlice, tarSlice);
        default:
            cerr << "Error. Wrong dimensions number: " << dims << ". Should be in range 1-5." << endl;
            exit(-1);
    }
}
