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

#include "additions.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <vector>

using namespace std;


////////////////////////////////HELPER FUNCTIONS////////////////////////////////

bool defaultMaskConverter(double value)
{
    return value > 0.0;
}

bool doubleComparator(double a, double b)
{
    if (std::isnan(b))
        return true;

    if (std::isnan(a))
        return false;

   return a < b;
}

bool noiseFilter(double value, void* params)
{
    double threshold = *(double*)params;
    return value >= threshold;
}


////////////////////////////////PUBLIC FUNCTIONS////////////////////////////////

bool* convertArrayToBinaryMask(int size, double* array, bool (*converter)(double))
{
    if (converter == NULL)
        converter = defaultMaskConverter;

    bool* mask = new bool[size];
    for (int i = 0; i < size; i++)
        mask[i] = converter(array[i]);

    return mask;
}

void applyMaskToImage(int size, double* image, bool* mask)
{
    for (int i = 0; i < size; i++)
        if (!mask[i])
            image[i] = nan("");
}

void applyFilteringFunctionToImage(int size, double* image, bool (*filter)(double, void*), void* params)
{
    for (int i = 0; i < size; i++)
        if (!filter(image[i], params))
            image[i] = nan("");
}

void applyNoiseFilteringToImage(int size, double* image, double noisePercentageLimit)
{
    double minVal = imageMin(size, image);
    double threshold = minVal + noisePercentageLimit * (imageMax(size, image) - minVal) / 100.0;
    return applyFilteringFunctionToImage(size, image, noiseFilter, (void*)&threshold);
}

void applyLinearMappingToImage(int size, double* image, double multiply, double add)
{
    for (int i = 0; i < size; i++)
        if (!std::isnan(image[i]))
            image[i] = multiply * image[i] + add;
}


int calculateGammaArraySize(int dims, int refXNumber, int refYNumber, int refZNumber, int plane)
{
    switch(dims)
    {
        case 1:
            return refXNumber;
        case 2:
            return refXNumber*refYNumber;
        case 3:
            return refXNumber*refYNumber*refZNumber;
        case 4:
        case 5:
            switch(plane)
            {
                case 0:
                    return refXNumber*refYNumber;
                case 1:
                    return refXNumber*refZNumber;
                case 2:
                    return refYNumber*refZNumber;
                default:
                    cerr << "Error. Wrong plane: " << plane << ". Should be in range 0-2." << endl;
                    exit(-1);
            }
        default:
            cerr << "Error. Wrong dimensions number: " << dims << ". Should be in range 1-5." << endl;
            exit(-1);
    }
}

int activeVoxelsNumber(int size, double* image)
{
    int counter = 0;
    for (int i = 0; i < size; i++)
        if (!std::isnan(image[i]))
            counter++;

    return counter;
}

double activeVoxelsPercentage(int size, double* image)
{
    return double(activeVoxelsNumber(size, image)) / double(size);
}

double imageMin(int size, double* image)
{
    double minVal = 1.0e10;
    for (int i = 0; i < size; i++)
        if (image[i] < minVal)
            minVal = image[i];

    return minVal;
}

double imageMax(int size, double* image)
{
    double maxVal = -1.0e10;
    for (int i = 0; i < size; i++)
        if (image[i] > maxVal)
            maxVal = image[i];

    return maxVal;
}

double imageAvg(int size, double* image)
{
    double sum = 0.0;
    for (int i = 0; i < size; i++)
        if (!std::isnan(image[i]))
            sum += image[i];

    return sum / activeVoxelsNumber(size, image);
}

double gammaPassingRate(int size, double* gamma)
{
     int counter = 0;
     for (int i = 0; i < size; i++)
         if (gamma[i] < 1.0)
             counter++;

     return double(counter) / double(activeVoxelsNumber(size, gamma));
}

string createComputationDescription(string refFile, string tarFile, int algorithm)
{
    if (algorithm < 0 || algorithm > 2)
    {
        cerr << "Error. Wrong algorithm number: " << algorithm << ". Should be in range 0-2." << endl;
        exit(-1);
    }

    string algorithms[3] = {"ClassicSolver", "SpiralSolverRectangle", "SpiralSolverWithoutRectangle"};
    return "[" + refFile + ", " + tarFile + ", " + algorithms[algorithm] + "]";
}

void prepareStatisticsFile(int size, double* gamma, string info, string filepath)
{
    double min = imageMin(size, gamma);
    double max = imageMax(size, gamma);
    double avg = imageAvg(size, gamma);
    double gpr = gammaPassingRate(size, gamma);

    ofstream out;
    out.open(filepath.c_str(), ios::app);
    out << info << ":\t" << min << "\t" << avg << "\t" << max << "\t" << (gpr*100) << "%" << endl;
    out.close();
}

int* prepareHistogram(int gammaSize, double* gamma, int delimitersSize, double* delimiters)
{
    double* sortedGamma = new double[gammaSize];
    copy(gamma, gamma + gammaSize, sortedGamma);
    sort(sortedGamma, gamma + gammaSize, doubleComparator);

    int* histogram = new int[delimitersSize - 1];
    fill(histogram, histogram + (delimitersSize - 1), 0);

    int delimitersIndex = -1;
    for (int i = 0; i < gammaSize; i++)
    {
        if (std::isnan(gamma[i]))
            break;

        while (delimitersIndex < delimitersSize - 1 && gamma[i] >= delimiters[delimitersIndex + 1])
            delimitersIndex++;

        if (delimitersIndex == -1)
            continue;

        if (delimitersIndex == delimitersSize - 1)
            break;

        histogram[delimitersIndex]++;
    }

    delete [] sortedGamma;

    return histogram;
}

int* prepareHistogram(int gammaSize, double* gamma, double xMin, double xMax, int bucketsNumber)
{
    double* delimiters = new double[bucketsNumber + 1];
    double step = (xMax - xMin) / bucketsNumber;

    for (int i = 0; i <= bucketsNumber; i++)
        delimiters[i] = xMin + i * step;

    int* histogram = prepareHistogram(gammaSize, gamma, bucketsNumber + 1, delimiters);

    delete [] delimiters;

    return histogram;
}

int* prepareHistogram(int gammaSize, double* gamma)
{
    return prepareHistogram(gammaSize, gamma, 0.0, 3.0, 300);
}

int* prepareCumulativeHistogram(int gammaSize, double* gamma, int delimitersSize, double* delimiters)
{
    int* histogram = prepareHistogram(gammaSize, gamma, delimitersSize, delimiters);
    for (int i = 1; i < delimitersSize - 1; i++)
        histogram[i] += histogram[i - 1];
    return histogram;
}

int* prepareCumulativeHistogram(int gammaSize, double* gamma, double xMin, double xMax, int bucketsNumber)
{
    int* histogram = prepareHistogram(gammaSize, gamma, xMin, xMax, bucketsNumber);
    for (int i = 1; i < bucketsNumber; i++)
        histogram[i] += histogram[i - 1];
    return histogram;
}

int* prepareCumulativeHistogram(int gammaSize, double* gamma)
{
    return prepareCumulativeHistogram(gammaSize, gamma, 0.0, 3.0, 300);
}
