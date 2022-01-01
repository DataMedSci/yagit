/*********************************************************************************************************************
 * Spiral Solver
 *
 * This is a solver used for Gamma Index (GI) calculation using spiral no interpolation version of the algorithm
 *  in which for a given reference point comparisons are preformed for the target points ordered with the respect of
 *  increasing distance from current reference point - this forms a spiral of the target points with the center in the current reference point.
 * This solver comes in two subversions:
 *  - with rectangle
 *  - without rectangle
 *
 * calculateGamma
 * Main function for calculating GI for given two - reference and target - images.
 * It iterates over the reference image and calculates the GI value for each reference point
 * using the spiral obtained from getIndicesOrder function.
 * Thanks to the fact that voxels on the spiral characterize with the increasing distance from the reference point
 * (and consequently dta component value), the calculations can be stopped when the dta exceeds the current GI value.
 *
 * getIndicesOrder
 * Function (defined for each subversion) for calculating the spiral, this is vector of offsets from a virtual center of the spiral.
 * The vector is ordered with the respect of increasing distance from this center.
 * The spiral "exists" in the target image, while the center comes from the reference image.
 * The spiral's shape does not depend on the position of its center and its size(length) is calculated,
 * so that it is big enough to cover every potential pairing of voxels in comparison.
 * Thanks to this the spiral can be calculated only once per comparison of two given image
 * and reused for each reference voxel for which we currently calculate GI value.
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
#include <cmath>
#include <cstdlib>
#include <utility>
#include <vector>
#include "image.h"
#include "core_logger.h"

//#ifdef _WIN32
//    #include "windows.h"
//#endif

using namespace std;


typedef Solver::Voxel Voxel;

SpiralSolver1D::SpiralSolver1D(Image1D& reference, Image1D& target, double percentage, double dta)
        : Solver1D(reference, target, percentage, dta)
{
}

Image1D SpiralSolver1D::calculateGamma()
{
    logCoreMessage("Calculating 1-D Gamma with spiral solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<double> gamma(refRShape[0], pow(limit_, 2));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();
    vector<vector<int> > corr = reference_.getCorresponding(target_);

    // Obtaining the spiral
    int left;
    vector<Voxel> indices = getIndicesOrder(left);

    // Iterating over the reference image
    for (int rx = 0; rx < refRShape[0]; rx++)
    {
        if (std::isnan(reference_.data()[rx]))
        {
            gamma[rx] = nan("");
            continue;
        }

        double dx = pow(ref[0][rx] - tar[0][corr[0][rx]], 2);
        double closest = sqrt(dx);

        // Iterating over spiral
        for (int i = 0; i < indices.size(); i++)
        {
            Voxel& index = indices[i];

            // Checking if there is a point in continuing the calculations for current reference point (with the amendment caused by the lack of interpolation)
            if (pow(index.dist - closest, 2)/ dtaSquare_ >= gamma[rx] && index.dist > closest)
                break;

            int tx = corr[0][rx] + index.x - left;

            // Calculating gamma equation if the point from spiral lies inside the target image
            if (target_.isIndexInside(tx) && !std::isnan(target_.data()[tx]))
                gamma[rx] = min(gamma[rx],
                                gammaEquation(reference_.data()[rx], target_.data()[tx], tar[0][tx] - ref[0][rx], 0, 0));
        }
        gamma[rx] = sqrt(gamma[rx]);
    }

    logCoreMessage("Calculations completed successfully.");

    // Returning the image of GI values for each reference point
    return Image1D(reference_.start(), reference_.spacing(), gamma);
}


SpiralSolver2D::SpiralSolver2D(Image2D& reference, Image2D& target, double percentage, double dta)
        : Solver2D(reference, target, percentage, dta)
{
}

Image2D SpiralSolver2D::calculateGamma()
{
    logCoreMessage("Calculating 2-D Gamma with spiral solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<vector<double> > gamma(refRShape[1], vector<double>(refRShape[0], pow(limit_, 2)));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();
    vector<vector<int> > corr = reference_.getCorresponding(target_);

    // Obtaining the spiral
    int left, top;
    vector<Voxel> indices = getIndicesOrder(left, top);

    // Iterating over the reference image
    for (int ry = 0; ry < refRShape[1]; ry++)
    {
        double dy = pow(ref[1][ry] - tar[1][corr[1][ry]], 2);
        for (int rx = 0; rx < refRShape[0]; rx++)
        {
            if (std::isnan(reference_.data()[ry][rx]))
            {
                gamma[ry][rx] = nan("");
                continue;
            }

            double dx = pow(ref[0][rx] - tar[0][corr[0][rx]], 2);
            double closest = sqrt(dx + dy);

            // Iterating over spiral
            for (int i = 0; i < indices.size(); i++)
            {
                Voxel& index = indices[i];

                // Checking if there is a point in continuing the calculations for current reference point (with the amendment caused by the lack of interpolation)
                if (pow(index.dist - closest, 2)/ dtaSquare_ >= gamma[ry][rx] && index.dist > closest)
                    break;

                int tx = corr[0][rx] + index.x - left;
                int ty = corr[1][ry] + index.y - top;

                // Calculating gamma equation if the point from spiral lies inside the target image
                if (target_.isIndexInside(tx, ty) && !std::isnan(target_.data()[ty][tx]))
                    gamma[ry][rx] = min(gamma[ry][rx], gammaEquation(reference_.data()[ry][rx], target_.data()[ty][tx],
                                                                     tar[0][tx] - ref[0][rx], tar[1][ty] - ref[1][ry], 0));
            }
            gamma[ry][rx] = sqrt(gamma[ry][rx]);
        }
    }

    logCoreMessage("Calculations completed successfully.");

    // Returning the image of GI values for each reference point
    return Image2D(reference_.start(), reference_.spacing(), gamma);
}


SpiralSolver3D::SpiralSolver3D(Image3D& reference, Image3D& target, double percentage, double dta)
        : Solver3D(reference, target, percentage, dta)
{
}

Image3D SpiralSolver3D::calculateGamma()
{
    logCoreMessage("Calculating 3-D Gamma with spiral solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<vector<vector<double> > > gamma(refRShape[2], vector<vector<double> >(refRShape[1], vector<double>(refRShape[0], pow(limit_, 2))));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();
    vector<vector<int> > corr = reference_.getCorresponding(target_);

    // Obtaining the spiral
    int left, top, front;
    vector<Voxel> indices = getIndicesOrder(left, top, front);

    // Iterating over the reference image
    auto total_size = refRShape[0] * refRShape[1] * refRShape[2];
    size_t idx = 0;
    for (int rz = 0; rz < refRShape[2]; rz++)
    {
        double dz = pow(ref[2][rz] - tar[2][corr[2][rz]], 2);
        for (int ry = 0; ry < refRShape[1]; ry++)
        {
            double dy = pow(ref[1][ry] - tar[1][corr[1][ry]], 2);
            for (int rx = 0; rx < refRShape[0]; rx++)
            {
                if (idx % 1024 == 0)
                    cout << "Progress: " << (static_cast<double>(idx) / total_size) << endl;
                ++idx;

                if (std::isnan(reference_.data()[rz][ry][rx]))
                {
                    gamma[rz][ry][rx] = nan("");
                    continue;
                }

                double dx = pow(ref[0][rx] - tar[0][corr[0][rx]], 2);
                double closest = sqrt(dx + dy + dz);

                // Iterating over spiral
                for (int i = 0; i < indices.size(); i++)
                {
                    Voxel& index = indices[i];

                    // Checking if there is a point in continuing the calculations for current reference point (with the amendment caused by the lack of interpolation)
                    if (pow(index.dist - closest, 2)/ dtaSquare_ >= gamma[rz][ry][rx] && index.dist > closest)
                        break;

                    int tx = corr[0][rx] + index.x - left;
                    int ty = corr[1][ry] + index.y - top;
                    int tz = corr[2][rz] + index.z - front;

                    // Calculating gamma equation if the point from spiral lies inside the target image
                    if (target_.isIndexInside(tx, ty, tz) && !std::isnan(target_.data()[tz][ty][tx]))
                        gamma[rz][ry][rx] = min(gamma[rz][ry][rx],
                                                gammaEquation(reference_.data()[rz][ry][rx], target_.data()[tz][ty][tx],
                                                              tar[0][tx] - ref[0][rx], tar[1][ty] - ref[1][ry], tar[2][tz] - ref[2][rz]));
                }
                gamma[rz][ry][rx] = sqrt(gamma[rz][ry][rx]);
            }
        }
    }

    logCoreMessage("Calculations completed successfully.");

    // Returning the image of GI values for each reference point
    return Image3D(reference_.start(), reference_.spacing(), gamma);
}

Image2D SpiralSolver3D::calculateGamma2_5DSliceXY(int slice)
{
    logCoreMessage("Calculating 2.5-D Gamma through XY with spiral solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<vector<double> > gamma(refRShape[1], vector<double>(refRShape[0], pow(limit_, 2)));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();
    vector<vector<int> > corr = reference_.getCorresponding(target_);

    // Obtaining the spiral
    int left, top, front;
    vector<Voxel> indices = getIndicesOrder(left, top, front);

    // Iterating over the given SLICE of the reference image
    double dz = pow(ref[2][slice] - tar[2][corr[2][slice]], 2);
    for (int ry = 0; ry < refRShape[1]; ry++)
    {
        double dy = pow(ref[1][ry] - tar[1][corr[1][ry]], 2);
        for (int rx = 0; rx < refRShape[0]; rx++)
        {
            if (std::isnan(reference_.data()[slice][ry][rx]))
            {
                gamma[ry][rx] = nan("");
                continue;
            }

            double dx = pow(ref[0][rx] - tar[0][corr[0][rx]], 2);
            double closest = sqrt(dx + dy + dz);

            // Iterating over spiral
            for (int i = 0; i < indices.size(); i++)
            {
                Voxel& index = indices[i];

                // Checking if there is a point in continuing the calculations for current reference point (with the amendment caused by the lack of interpolation)
                if (pow(index.dist - closest, 2)/ dtaSquare_ >= gamma[ry][rx] && index.dist > closest)
                    break;

                int tx = corr[0][rx] + index.x - left;
                int ty = corr[1][ry] + index.y - top;
                int tz = corr[2][slice] + index.z - front;

                // Calculating gamma equation if the point from spiral lies inside the target image
                if (target_.isIndexInside(tx, ty, tz) && !std::isnan(target_.data()[tz][ty][tx]))
                    gamma[ry][rx] = min(gamma[ry][rx],
                                        gammaEquation(reference_.data()[slice][ry][rx], target_.data()[tz][ty][tx],
                                                      tar[0][tx] - ref[0][rx], tar[1][ty] - ref[1][ry], tar[2][tz] - ref[2][slice]));
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

Image2D SpiralSolver3D::calculateGamma2_5DSliceXZ(int slice)
{
    logCoreMessage("Calculating 2.5-D Gamma through XZ with spiral solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<vector<double> > gamma(refRShape[2], vector<double>(refRShape[0], pow(limit_, 2)));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();
    vector<vector<int> > corr = reference_.getCorresponding(target_);

    // Obtaining the spiral
    int left, top, front;
    vector<Voxel> indices = getIndicesOrder(left, top, front);

    // Iterating over the given SLICE of the reference image
    double dy = pow(ref[1][slice] - tar[1][corr[1][slice]], 2);
    for (int rz = 0; rz < refRShape[2]; rz++)
    {
        double dz = pow(ref[2][rz] - tar[2][corr[2][rz]], 2);
        for (int rx = 0; rx < refRShape[0]; rx++)
        {
            if (std::isnan(reference_.data()[rz][slice][rx]))
            {
                gamma[rz][rx] = nan("");
                continue;
            }

            double dx = pow(ref[0][rx] - tar[0][corr[0][rx]], 2);
            double closest = sqrt(dx + dy + dz);

            // Iterating over spiral
            for (int i = 0; i < indices.size(); i++)
            {
                Voxel& index = indices[i];

                // Checking if there is a point in continuing the calculations for current reference point (with the amendment caused by the lack of interpolation)
                if (pow(index.dist - closest, 2)/ dtaSquare_ >= gamma[rz][rx] && index.dist > closest)
                    break;

                int tx = corr[0][rx] + index.x - left;
                int ty = corr[1][slice] + index.y - top;
                int tz = corr[2][rz] + index.z - front;

                // Calculating gamma equation if the point from spiral lies inside the target image
                if (target_.isIndexInside(tx, ty, tz) && !std::isnan(target_.data()[tz][ty][tx]))
                    gamma[rz][rx] = min(gamma[rz][rx],
                                        gammaEquation(reference_.data()[rz][slice][rx], target_.data()[tz][ty][tx],
                                                      tar[0][tx] - ref[0][rx], tar[1][ty] - ref[1][slice], tar[2][tz] - ref[2][rz]));
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

Image2D SpiralSolver3D::calculateGamma2_5DSliceYZ(int slice)
{
    logCoreMessage("Calculating 2.5-D Gamma through YZ with spiral solver...");

    vector<int> refRShape = reference_.getReversedShape();
    vector<vector<double> > gamma(refRShape[2], vector<double>(refRShape[1], pow(limit_, 2)));

    vector<vector<double> > ref = reference_.coordinates();
    vector<vector<double> > tar = target_.coordinates();
    vector<vector<int> > corr = reference_.getCorresponding(target_);

    // Obtaining the spiral
    int left, top, front;
    vector<Voxel> indices = getIndicesOrder(left, top, front);

    // Iterating over the given SLICE of the reference image
    double dx = pow(ref[0][slice] - tar[0][corr[0][slice]], 2);
    for (int rz = 0; rz < refRShape[2]; rz++)
    {
        double dz = pow(ref[2][rz] - tar[2][corr[2][rz]], 2);
        for (int ry = 0; ry < refRShape[1]; ry++)
        {
            if (std::isnan(reference_.data()[rz][ry][slice]))
            {
                gamma[rz][ry] = nan("");
                continue;
            }

            double dy = pow(ref[1][ry] - tar[1][corr[1][ry]], 2);
            double closest = sqrt(dx + dy + dz);

            // Iterating over spiral
            for (int i = 0; i < indices.size(); i++)
            {
                Voxel& index = indices[i];

                // Checking if there is a point in continuing the calculations for current reference point (with the amendment caused by the lack of interpolation)
                if (pow(index.dist - closest, 2)/ dtaSquare_ >= gamma[rz][ry] && index.dist > closest)
                    break;

                int tx = corr[0][slice] + index.x - left;
                int ty = corr[1][ry] + index.y - top;
                int tz = corr[2][rz] + index.z - front;

                // Calculating gamma equation if the point from spiral lies inside the target image
                if (target_.isIndexInside(tx, ty, tz) && !std::isnan(target_.data()[tz][ty][tx]))
                    gamma[rz][ry] = min(gamma[rz][ry],
                                        gammaEquation(reference_.data()[rz][ry][slice], target_.data()[tz][ty][tx],
                                                      tar[0][tx] - ref[0][slice], tar[1][ty] - ref[1][ry], tar[2][tz] - ref[2][rz]));
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

Image2D SpiralSolver3D::calculateGamma2_5D(int plane, int slice)
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
