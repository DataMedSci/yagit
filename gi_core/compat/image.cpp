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
 
#include "image.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;


Image::Image(vector<double> start, vector<double> spacing)
    : start_(start), spacing_(spacing)
{
}

Image::~Image() {}

const vector<double>& Image::end()
{
    if (end_.empty())
        for (int dim = 0; dim < getNdim(); dim++)
            end_.push_back(start_[dim] + spacing_[dim] * (getReversedShape()[dim] - 1));

    return end_;
}

const vector<vector<double> >& Image::coordinates()
{
    if (coordinates_.empty())
    {
        vector<double> axis;
        for (int dim = 0; dim < getNdim(); dim++)
        {
            for (int i = 0; i < getReversedShape()[dim]; i++)
                axis.push_back(start_[dim] + spacing_[dim] * i);
            coordinates_.push_back(axis);
            axis.clear();
        }
    }

    return coordinates_;
}

vector<vector<int> > Image::getCorresponding(Image& reference)
{
    vector<vector<int> > corresponding(getNdim());
    vector<int> axis;
    for (int dim = 0; dim < getNdim(); dim++)
    {
        for (int i = 0; i < getReversedShape()[dim]; i++)
            axis.push_back(
                    min(max(0, int(round((coordinates()[dim][i] - reference.start()[dim])/ reference.spacing()[dim]))),
                        reference.getReversedShape()[dim] - 1));
        corresponding[dim] = axis;
        axis.clear();
    }

    return corresponding;
}

bool Image::isPointInside(double x, double y, double z)
{
    return start()[0] <= x && x <= end()[0] &&
           (std::isnan(y) || (start()[1] <= y && y <= end()[1] &&
                              (std::isnan(z) || (start()[2] <= z && z <= end()[2]))));
}

const vector<double>& Image::start() const { return start_; }

const vector<double>& Image::spacing() const { return spacing_; }


Image1D::Image1D(vector<double> start, vector<double> spacing, vector<double> data)
        : Image(start, spacing), data_(data)
{
}

Image1D::~Image1D(){}

int Image1D::getNdim() const { return 1; }

int Image1D::getNelems() const { return data_.size(); }

double Image1D::getMaxValue() const
{
    return *max_element(data_.begin(), data_.end());
}

vector<int> Image1D::getShape() const
{
    vector<int> shape;
    shape.push_back(data_.size());
    return shape;
}

vector<int> Image1D::getReversedShape() const
{
    vector<int> shape;
    shape.push_back(data_.size());
    return shape;
}

void Image1D::print() const
{
    vector<int> shape = getReversedShape();
    for (int i = 0; i < shape[0]; i++)
        cout << data_[i] << " ";
    cout << endl;
}

bool Image1D::isIndexInside(int i) const
{
    return 0 <= i && i < getReversedShape()[0];
}

const vector<double>& Image1D::data() const { return data_; }

double* Image1D::dataToArray() const
{
    vector<int> shape = getReversedShape();
    double* result = new double[getNelems()];

    for (int i = 0; i < shape[0]; i++)
        result[i] = data()[i];

    return result;
}


Image2D::Image2D(vector<double> start, vector<double> spacing, vector<vector<double> > data)
        : Image(start, spacing), data_(data)
{
}

Image2D::~Image2D(){}

int Image2D::getNdim() const { return 2; }

int Image2D::getNelems() const { return data_.size() * data_[0].size(); }

double Image2D::getMaxValue() const
{
    double max_val = -1.0e8;
    vector<int> shape = getReversedShape();
    for (int j = 0; j < shape[1]; j++)
        for (int i = 0; i < shape[0]; i++)
            max_val = max(max_val, data_[j][i]);
    return max_val;
}

vector<int> Image2D::getShape() const
{
    vector<int> shape;
    shape.push_back(data_.size());
    shape.push_back(data_[0].size());
    return shape;
}

vector<int> Image2D::getReversedShape() const
{
    vector<int> shape;
    shape.push_back(data_[0].size());
    shape.push_back(data_.size());
    return shape;
}

void Image2D::print() const
{
    vector<int> shape = getReversedShape();
    for (int j = 0; j < shape[1]; j++){
        for (int i = 0; i < shape[0]; i++)
            cout << data_[j][i] << " ";
        cout << endl;
    }
}

bool Image2D::isIndexInside(int i, int j) const
{
    return 0 <= i && i < getReversedShape()[0] &&
           0 <= j && j < getReversedShape()[1];
}

const vector<vector<double> >& Image2D::data() const { return data_; }

double* Image2D::dataToArray() const
{
    vector<int> shape = getReversedShape();
    double* result = new double[getNelems()];

    for (int j = 0; j < shape[1]; j++)
        for (int i = 0; i < shape[0]; i++)
            result[j * shape[0] + i] = data()[j][i];

    return result;
}


Image3D::Image3D(vector<double> start, vector<double> spacing, vector<vector<vector<double> > > data)
        : Image(start, spacing), data_(data)
{
}

Image3D::~Image3D(){}

int Image3D::getNdim() const { return 3; }

int Image3D::getNelems() const { return data_.size() * data_[0].size() * data_[0][0].size(); }

double Image3D::getMaxValue() const
{
    double max_val = -1.0e8;
    vector<int> shape = getReversedShape();
    for (int k = 0; k < shape[2]; k++)
        for (int j = 0; j < shape[1]; j++)
            for (int i = 0; i < shape[0]; i++)
                max_val = max(max_val, data_[k][j][i]);
    return max_val;
}

vector<int> Image3D::getShape() const
{
    vector<int> shape;
    shape.push_back(data_.size());
    shape.push_back(data_[0].size());
    shape.push_back(data_[0][0].size());
    return shape;
}

vector<int> Image3D::getReversedShape() const
{
    vector<int> shape;
    shape.push_back(data_[0][0].size());
    shape.push_back(data_[0].size());
    shape.push_back(data_.size());
    return shape;
}

void Image3D::print() const
{
    vector<int> shape = getReversedShape();
    for (int k = 0; k < shape[2]; k++)
    {
        for (int j = 0; j < shape[1]; j++)
        {
            for (int i = 0; i < shape[0]; i++)
                cout << data_[k][j][i] << " ";
            cout << endl;
        }
        cout << endl;
    }
}

bool Image3D::isIndexInside(int i, int j, int k) const
{
    return 0 <= i && i < getReversedShape()[0] &&
           0 <= j && j < getReversedShape()[1] &&
           0 <= k && k < getReversedShape()[2];
}

const vector<vector<vector<double> > >& Image3D::data() const { return data_; }

double* Image3D::dataToArray() const
{
    vector<int> shape = getReversedShape();
    double *result = new double[getNelems()];

    for (int k = 0; k < shape[2]; k++)
        for (int j = 0; j < shape[1]; j++)
            for (int i = 0; i < shape[0]; i++)
                result[(k * shape[1] + j)* shape[0] + i] = data()[k][j][i];

    return result;
}
