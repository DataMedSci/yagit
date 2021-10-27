/*********************************************************************************************************************
 * Classic Solver - without interpolation and rectangle
 *
 * This is a solver used for Gamma Index calculation in most classical version of this algorithm. It does not limit
 * comparison to certain rectangle, but compare every pair of points from reference and target images.
 *
 * calculateGamma
 * Main function for calculating GI for given two - reference and target - images.
 * It iterates over the reference image and calculates the GI value for each reference point.
 *********************************************************************************************************************/
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

#include "solver.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>
#include "image.h"
#include "core_logger.h"

using namespace std;


typedef Solver::Voxel Voxel;

ClassicSolver1D::ClassicSolver1D(Image1D& reference, Image1D& target, double percentage, double dta)
        : Solver1D(reference, target, percentage, dta)
{
}

Image1D ClassicSolver1D::calculateGamma()
{
    logCoreMessage("Calculating 1-D Gamma with classic solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<int> tarRShape = target_.getReversedShape();
    vector<double> gamma(refRShape[0], pow(limit_, 2));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();

    // Iterating over the reference image
    for (int rx = 0; rx < refRShape[0]; rx++)
    {
        if (std::isnan(reference_.data()[rx]))
        {
            gamma[rx] = nan("");
            continue;
        }

        // Iterating over the target image
        for (int tx = 0; tx < tarRShape[0]; tx++)
        {
            if (std::isnan(target_.data()[tx]))
                continue;

            // Calculating gamma equation and remembering better (smaller)value
            gamma[rx] = min(gamma[rx], gammaEquation(reference_.data()[rx], target_.data()[tx],
                                                     tar[0][tx] - ref[0][rx], 0, 0));
        }

        gamma[rx] = sqrt(gamma[rx]);
    }

    logCoreMessage("Calculations completed successfully.");

    // Returning the image of GI values for each reference point
    return Image1D(reference_.start(), reference_.spacing(), gamma);
}


ClassicSolver2D::ClassicSolver2D(Image2D& reference, Image2D& target, double percentage, double dta)
        : Solver2D(reference, target, percentage, dta)
{
}

Image2D ClassicSolver2D::calculateGamma()
{
    logCoreMessage("Calculating 2-D Gamma with classic solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<int> tarRShape = target_.getReversedShape();
    vector<vector<double> > gamma(refRShape[1], vector<double>(refRShape[0], pow(limit_, 2)));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();

    // Iterating over the reference image
    for (int ry = 0; ry < refRShape[1]; ry++)
    {
        for (int rx = 0; rx < refRShape[0]; rx++)
        {
            if (std::isnan(reference_.data()[ry][rx]))
            {
                gamma[ry][rx] = nan("");
                continue;
            }

            // Iterating over the target image
            for (int ty = 0; ty < tarRShape[1]; ty++)
            {
                for (int tx = 0; tx < tarRShape[0]; tx++)
                {
                    if (std::isnan(target_.data()[ty][tx]))
                        continue;

                    // Calculating gamma equation and remembering better (smaller)value
                    gamma[ry][rx] = min(gamma[ry][rx], gammaEquation(reference_.data()[ry][rx], target_.data()[ty][tx],
                                                                     tar[0][tx] - ref[0][rx], tar[1][ty] - ref[1][ry], 0));
                }
            }
            gamma[ry][rx] = sqrt(gamma[ry][rx]);
        }
    }

    logCoreMessage("Calculations completed successfully.");

    // Returning the image of GI values for each reference point
    return Image2D(reference_.start(), reference_.spacing(), gamma);
}

ClassicSolver3D::ClassicSolver3D(Image3D& reference, Image3D& target, double percentage, double dta)
        : Solver3D(reference, target, percentage, dta)
{
}

Image3D ClassicSolver3D::calculateGamma()
{
    logCoreMessage("Calculating 3-D Gamma with classic solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<int> tarRShape = target_.getReversedShape();
    vector<vector<vector<double> > > gamma(refRShape[2], vector<vector<double> >(refRShape[1], vector<double>(refRShape[0], pow(limit_, 2))));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();

    // Iterating over the reference image
    for (int rz = 0; rz < refRShape[2]; rz++)
    {
        for (int ry = 0; ry < refRShape[1]; ry++)
        {
            for (int rx = 0; rx < refRShape[0]; rx++)
            {
                if (std::isnan(reference_.data()[rz][ry][rx]))
                {
                    gamma[rz][ry][rx] = nan("");
                    continue;
                }

                // Iterating over the target image
                for (int tz = 0; tz < tarRShape[2]; tz++)
                {
                    for (int ty = 0; ty < tarRShape[1]; ty++)
                    {
                        for (int tx = 0; tx < tarRShape[0]; tx++)
                        {
                            if (std::isnan(target_.data()[tz][ty][tx]))
                                continue;

                            // Calculating gamma equation and remembering better (smaller)value
                            gamma[rz][ry][rx] = min(gamma[rz][ry][rx], gammaEquation(reference_.data()[rz][ry][rx], target_.data()[tz][ty][tx],
                                                                                     tar[0][tx] - ref[0][rx], tar[1][ty] - ref[1][ry], tar[2][tz] - ref[2][rz]));
                        }
                    }
                }
                gamma[rz][ry][rx] = sqrt(gamma[rz][ry][rx]);
            }
        }
    }

    logCoreMessage("Calculations completed successfully.");

    // Returning the image of GI values for each reference point
    return Image3D(reference_.start(), reference_.spacing(), gamma);
}

Image2D ClassicSolver3D::calculateGamma2_5DSliceXY(int slice)
{
    logCoreMessage("Calculating 2.5-D Gamma through XY with classic solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<int> tarRShape = target_.getReversedShape();
    vector<vector<double> > gamma(refRShape[1], vector<double>(refRShape[0], pow(limit_, 2)));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();

    // Iterating over the given SLICE of the reference image
    for (int ry = 0; ry < refRShape[1]; ry++)
    {
        for (int rx = 0; rx < refRShape[0]; rx++)
        {
            if (std::isnan(reference_.data()[slice][ry][rx]))
            {
                gamma[ry][rx] = nan("");
                continue;
            }

            // Iterating over the target image
            for (int tz = 0; tz < tarRShape[2]; tz++)
            {
                for (int ty = 0; ty < tarRShape[1]; ty++)
                {
                    for (int tx = 0; tx < tarRShape[0]; tx++)
                    {
                        if (std::isnan(target_.data()[tz][ty][tx]))
                            continue;

                        // Calculating gamma equation and remembering better (smaller)value
                        gamma[ry][rx] = min(gamma[ry][rx],
                                            gammaEquation(reference_.data()[slice][ry][rx], target_.data()[tz][ty][tx],
                                                          tar[0][tx] - ref[0][rx], tar[1][ty] - ref[1][ry], tar[2][tz] - ref[2][slice]));
                    }
                }
            }
            gamma[ry][rx] = sqrt(gamma[ry][rx]);
        }
    }

    vector<double> gammaStart(2);
    gammaStart[0] = reference_.start()[0];
    gammaStart[1] = reference_.start()[1];

    vector<double> gammaSpacing(2);
    gammaSpacing[0] = reference_.spacing()[0];
    gammaSpacing[1] = reference_.spacing()[1];

    logCoreMessage("Calculations completed successfully.");

    // Returning the image of GI values for each reference point
    return Image2D(gammaStart, gammaSpacing, gamma);
}

Image2D ClassicSolver3D::calculateGamma2_5DSliceXZ(int slice)
{
    logCoreMessage("Calculating 2.5-D Gamma through XZ with classic solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<int> tarRShape = target_.getReversedShape();
    vector<vector<double> > gamma(refRShape[2], vector<double>(refRShape[0], pow(limit_, 2)));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();

    // Iterating over the given SLICE of the reference image
    for (int rz = 0; rz < refRShape[2]; rz++)
    {
        for (int rx = 0; rx < refRShape[0]; rx++)
        {
            if (std::isnan(reference_.data()[rz][slice][rx]))
            {
                gamma[rz][rx] = nan("");
                continue;
            }

            // Iterating over the target image
            for (int tz = refRShape[2] - 1; tz >= 0; tz--)
            {
                for (int ty = 0; ty < tarRShape[1]; ty++)
                {
                    for (int tx = 0; tx < tarRShape[0]; tx++)
                    {
                        if (std::isnan(target_.data()[tz][ty][tx]))
                            continue;

                        // Calculating gamma equation and remembering better (smaller)value
                        gamma[rz][rx] = min(gamma[rz][rx],
                                            gammaEquation(reference_.data()[rz][slice][rx], target_.data()[tz][ty][tx],
                                                          tar[0][tx] - ref[0][rx], tar[1][ty] - ref[1][slice], tar[2][tz] - ref[2][rz]));
                    }
                }
            }
            gamma[rz][rx] = sqrt(gamma[rz][rx]);
        }
    }

    vector<double> gammaStart(2);
    gammaStart[0] = reference_.start()[0];
    gammaStart[1] = reference_.start()[2];

    vector<double> gammaSpacing(2);
    gammaSpacing[0] = reference_.spacing()[0];
    gammaSpacing[1] = reference_.spacing()[2];

    logCoreMessage("Calculations completed successfully.");

    // Returning the image of GI values for each reference point
    return Image2D(gammaStart, gammaSpacing, gamma);
}

Image2D ClassicSolver3D::calculateGamma2_5DSliceYZ(int slice)
{
    logCoreMessage("Calculating 2.5-D Gamma through YZ with classic solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<int> tarRShape = target_.getReversedShape();
    vector<vector<double> > gamma(refRShape[2], vector<double>(refRShape[1], pow(limit_, 2)));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();

    // Iterating over the given SLICE of the reference image
    for (int rz = 0; rz < refRShape[2]; rz++)
    {
        for (int ry = 0; ry < refRShape[1]; ry++)
        {
            if (std::isnan(reference_.data()[rz][ry][slice]))
            {
                gamma[rz][ry] = nan("");
                continue;
            }

            // Iterating over the target image
            for (int tz = refRShape[2] - 1; tz >= 0; tz--)
            {
                for (int ty = 0; ty < tarRShape[1]; ty++)
                {
                    for (int tx = 0; tx < tarRShape[0]; tx++)
                    {
                        if (std::isnan(target_.data()[tz][ty][tx]))
                            continue;

                        // Calculating gamma equation and remembering better (smaller)value
                        gamma[rz][ry] = min(gamma[rz][ry],
                                            gammaEquation(reference_.data()[rz][ry][slice], target_.data()[tz][ty][tx],
                                                          tar[0][tx] - ref[0][slice], tar[1][ty] - ref[1][ry], tar[2][tz] - ref[2][rz]));
                    }
                }
            }
            gamma[rz][ry] = sqrt(gamma[rz][ry]);
        }
    }

    vector<double> gammaStart(2);
    gammaStart[0] = reference_.start()[1];
    gammaStart[1] = reference_.start()[2];

    vector<double> gammaSpacing(2);
    gammaSpacing[0] = reference_.spacing()[1];
    gammaSpacing[1] = reference_.spacing()[2];

    logCoreMessage("Calculations completed successfully.");

    // Returning the image of GI values for each reference point
    return Image2D(gammaStart, gammaSpacing, gamma);
}

Image2D ClassicSolver3D::calculateGamma2_5D(int plane, int slice)
{
    switch(plane)
    {
        case 0:
            return calculateGamma2_5DSliceXY(slice);
        case 1:
            return calculateGamma2_5DSliceXZ(slice);
        case 2:
            return calculateGamma2_5DSliceYZ(slice);
        default:
            cerr << "Error. Wrong plane: " << plane << ". Should be in range 0-2." << endl;
            exit(-1);
    }
}
