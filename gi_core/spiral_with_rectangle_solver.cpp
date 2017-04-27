/*********************************************************************************************************************
 * SpiralRectangleSolver = Spiral No Interpolation Rectangle Solver
 *
 * This is a solver used for Gamma Index calculation using spiral no interpolation version of the algorithm
 *(subversion of SpiralSolver) with a boundary rectangle, within which the voxel comparison is performed.
 * The boundary was introduced in order to speed up calculations by elimination of comparison of distant voxels.
 *
 * getIndicesOrder
 * Function for calculating the spiral, this is ordered vector of offsets from a virtual central point of the spiral.
 * For further description see sni_solver.cpp
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

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>
#include "image.h"

using namespace std;


typedef Solver::Voxel Voxel;

SpiralRectangleSolver1D::SpiralRectangleSolver1D(Image1D& reference, Image1D& target, double percentage, double dta)
        : SpiralSolver1D(reference, target, percentage, dta)
{
}

vector<Voxel> SpiralRectangleSolver1D::getIndicesOrder(int& left) const
{
    left = int(2 * sqrt(dtaSquare_)/ target_.spacing()[0]);
    int width = 2 * left + 1;

    vector<Voxel> indices(width);

    for (int i = 0; i < width; i++)
    {
        double di = pow((i - left) * target_.spacing()[0], 2);
        indices[i] = Voxel(sqrt(di), i);
    }
    sort(indices.begin(), indices.end(), Voxel::compare);
    return indices;
}


SpiralRectangleSolver2D::SpiralRectangleSolver2D(Image2D& reference, Image2D& target, double percentage, double dta)
        : SpiralSolver2D(reference, target, percentage, dta)
{
}

vector<Voxel> SpiralRectangleSolver2D::getIndicesOrder(int& left, int& top) const
{
    left = int(2 * sqrt(dtaSquare_)/ target_.spacing()[0]);
    int width = 2 * left + 1;
    top = int(2 * sqrt(dtaSquare_)/ target_.spacing()[1]);
    int height = 2 * top + 1;

    vector<Voxel> indices(height * width);

    for (int j = 0; j < height; j++)
    {
        double dj = pow((j - top) * target_.spacing()[1], 2);
        for (int i = 0; i < width; i++)
        {
            double di = pow((i - left) * target_.spacing()[0], 2);
            indices[j * width + i] = Voxel(sqrt(di + dj), i, j);
        }
    }
    sort(indices.begin(), indices.end(), Voxel::compare);
    return indices;
}


SpiralRectangleSolver3D::SpiralRectangleSolver3D(Image3D& reference, Image3D& target, double percentage, double dta)
        : SpiralSolver3D(reference, target, percentage, dta)
{
}

vector<Voxel> SpiralRectangleSolver3D::getIndicesOrder(int& left, int& top, int& front) const
{
    left = int(2 * sqrt(dtaSquare_)/ target_.spacing()[0]);
    int width = 2 * left + 1;
    top = int(2 * sqrt(dtaSquare_)/ target_.spacing()[1]);
    int height = 2 * top + 1;
    front = int(2 * sqrt(dtaSquare_)/ target_.spacing()[2]);
    int depth = 2 * front + 1;

    vector<Voxel> indices(width * height * depth);

    for (int k = 0; k < depth; k++)
    {
        double dk = pow((k - front) * target_.spacing()[2], 2);
        for (int j = 0; j < height; j++)
        {
            double dj = pow((j - top) * target_.spacing()[1], 2);
            for (int i = 0; i < width; i++)
            {
                double di = pow((i - left) * target_.spacing()[0], 2);
                indices[(k * height + j) * width + i] = Voxel(sqrt(di + dj + dk), i, j, k);
            }
        }
    }
    sort(indices.begin(), indices.end(), Voxel::compare);
    return indices;
}
