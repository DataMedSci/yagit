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
#include "image.h"

using namespace std;


Solver::Solver(double percentage, double dta, double referenceValue)
        : percentage_(percentage / 100.0), dtaSquare_(pow(dta, 2)),
          local_(false), referenceValue_(referenceValue), limit_(2.0)
{
    calculateDD();
}

double Solver::doseDifference(double ref, double tar) const
{
    if (!local_)
        return pow((ref - tar), 2) / ddSquare_;
    else
        return pow((ref - tar) / (ref * percentage_), 2);
}

double Solver::distanceToAgreement(double x, double y, double z) const
{
    return (pow(x, 2) + pow(y, 2) + pow(z, 2)) / dtaSquare_;
}

double Solver::gammaEquation(double ref, double tar, double x, double y, double z) const
{
    return doseDifference(ref, tar) + distanceToAgreement(x, y, z);
}

void Solver::calculateDD()
{
    if (!local_)
    {
        if (percentage_ <= 0)
        {
            cerr << "Error. Wrong percentage value: " << percentage_ << ". Should be greater than 0." << endl;
            exit(-1);
        }
        if (referenceValue_ <= 0)
        {
            cerr << "Error. Wrong referenceValue: " << referenceValue_ << ". Should be greater than 0." << endl;
            exit(-1);
        }
        ddSquare_ = pow(percentage_ * referenceValue_, 2);
    }
}

double Solver::percentage() const { return percentage_ * 100.0; }

double Solver::dta() const { return sqrt(dtaSquare_); }

double Solver::referenceValue() const { return referenceValue_; }

double Solver::limit() const { return limit_; }

bool Solver::local() const { return local_; }

void Solver::setReferenceValue(double referenceValue)
{
    referenceValue_ = referenceValue;
    calculateDD();
}

void Solver::setLimit(double limit) { limit_ = limit; }

void Solver::setLocal(bool local)
{
    local_ = local;
    calculateDD();
}


Solver1D::Solver1D(Image1D& reference, Image1D& target, double percentage, double dta)
        : Solver(percentage, dta, reference.getMaxValue()), reference_(reference), target_(target)
{
}

Solver2D::Solver2D(Image2D& reference, Image2D& target, double percentage, double dta)
        : Solver(percentage, dta, reference.getMaxValue()), reference_(reference), target_(target)
{
}

Solver3D::Solver3D(Image3D& reference, Image3D& target, double percentage, double dta)
        : Solver(percentage, dta, reference.getMaxValue()), reference_(reference), target_(target)
        {
}
