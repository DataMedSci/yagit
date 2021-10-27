/*********************************************************************************************************************
 * Spiral No Rectangle Solver
 *
 * This is a solver used for Gamma Index calculation using spiral no interpolation version of the algorithm
 *  (subversion of SpiralSolver) without a boundary rectangle, so if needed even the most distant voxels can be compared.
 * This version is a bit closer to the original one.
 *  By not introducing the boundary the comparison of the images can be more exact, but for the cost of the time needed for calculations.
 * With introduction of the spiral the difference between versions with and without rectangle became very small
 *  and for quite similar images under comparison the output GI can be exactly the same in both versions.
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

SpiralNoRectangleSolver1D::SpiralNoRectangleSolver1D(Image1D& reference, Image1D& target, double percentage, double dta)
        : SpiralSolver1D(reference, target, percentage, dta)
{
}

vector<Voxel> SpiralNoRectangleSolver1D::getIndicesOrder(int& left) const
{
    int limit_bound = int(limit_ * sqrt(dtaSquare_)/target_.spacing()[0]) + 1;
    left = max(0, min(int((reference_.end()[0] - target_.start()[0])/target_.spacing()[0]), limit_bound));
    int width = left + max(0, min(int((target_.end()[0] - reference_.start()[0])/target_.spacing()[0]), limit_bound)) + 1;

    vector<Voxel> indices(width);

    for (int i = 0; i < width; i++)
    {
        double di = pow((i - left) * target_.spacing()[0], 2);
        indices[i] = Voxel(sqrt(di), i);
    }
    sort(indices.begin(), indices.end(), Voxel::compare);
    return indices;
}


SpiralNoRectangleSolver2D::SpiralNoRectangleSolver2D(Image2D& reference, Image2D& target, double percentage, double dta)
        : SpiralSolver2D(reference, target, percentage, dta)
{
}

vector<Voxel> SpiralNoRectangleSolver2D::getIndicesOrder(int& left, int& top) const
{
    int limit_bound = int(limit_ * sqrt(dtaSquare_)/target_.spacing()[0]) + 1;
    left = max(0, min(int((reference_.end()[0] - target_.start()[0])/target_.spacing()[0]), limit_bound));
    int width = left + max(0, min(int((target_.end()[0] - reference_.start()[0])/target_.spacing()[0]), limit_bound)) + 1;

    limit_bound = int(limit_ * sqrt(dtaSquare_)/target_.spacing()[1]) + 1;
    top = max(0, min(int((reference_.end()[1] - target_.start()[1])/target_.spacing()[1]), limit_bound));
    int height = top + max(0, min(int((target_.end()[1] - reference_.start()[1])/target_.spacing()[1]), limit_bound)) + 1;

    vector<Voxel> indices(width * height);

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


SpiralNoRectangleSolver3D::SpiralNoRectangleSolver3D(Image3D& reference, Image3D& target, double percentage, double dta)
        : SpiralSolver3D(reference, target, percentage, dta)
{
}

vector<Voxel> SpiralNoRectangleSolver3D::getIndicesOrder(int& left, int& top, int& front) const
{
    int limit_bound = int(limit_ * sqrt(dtaSquare_)/target_.spacing()[0]) + 1;
    left = max(0, min(int((reference_.end()[0] - target_.start()[0])/target_.spacing()[0]), limit_bound));
    int width = left + max(0, min(int((target_.end()[0] - reference_.start()[0])/target_.spacing()[0]), limit_bound)) + 1;

    limit_bound = int(limit_ * sqrt(dtaSquare_)/target_.spacing()[1]) + 1;
    top = max(0, min(int((reference_.end()[1] - target_.start()[1])/target_.spacing()[1]), limit_bound));
    int height = top + max(0, min(int((target_.end()[1] - reference_.start()[1])/target_.spacing()[1]), limit_bound)) + 1;

    limit_bound = int(limit_ * sqrt(dtaSquare_)/target_.spacing()[2]) + 1;
    front = max(0, min(int((reference_.end()[2] - target_.start()[2])/target_.spacing()[2]), limit_bound));
    int depth = front + max(0, min(int((target_.end()[2] - reference_.start()[2])/target_.spacing()[2]), limit_bound)) + 1;

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
