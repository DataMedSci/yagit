/** @file */

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

#ifndef GI_IMAGE_H
#define GI_IMAGE_H

#include<cmath>
#include<vector>

using namespace std;


class Image
{
public:
    /// \brief Constructs the base for Images - internal representation of dose images.
    ///
    /// \param start   Vector of coordinates of the center of left top front voxel (in this order).
    /// \param spacing Vector of distances between centers of adjacent voxels along each axis
    ///        (x, y and z axis - in this order).
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Image(vector<double> start, vector<double> spacing);

    virtual ~Image();

    /// \brief Returns coordinates of the center of the most left top front voxel (in this order).
    ///
    /// \return Vector of coordinates of the first voxel.
    ///         Vector size is equal to number of dimensions of the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const vector<double>& start() const;

    /// \brief Returns the distance between centers of adjacent voxels along each axis
    ///        (x, y and z axis - in this order).
    ///
    /// \return Vector of distances between centers of adjacent voxels along each axis.
    ///         Vector size is equal to number of dimensions of the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const vector<double>& spacing() const;

    /// \brief Returns coordinates of the center of the most right bottom back voxel (in this order).
    ///
    /// \return Vector of coordinates of the last voxel.
    ///         Vector size is equal to number of dimensions of the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const vector<double>& end();

    /// \brief List of ascending voxel coordinates along each axis (x, y and z axis - in this order).
    ///
    /// Voxels in the images are placed on a grid. Each voxel has an index along each axis.
    /// Voxels lying on a perpendicular line to an axis share the same index(and coordinate) along this axis.
    /// So for each axis a list can be created of ascending coordinates of voxels along the given axis.
    ///
    /// \return Vector of vectors of ascending voxel coordinates.
    ///         Outer vector size is equal to number of dimensions of the image.
    ///         Inner vectors have lengths according to getReversedShape().
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const vector<vector<double> >& coordinates();

    /// \brief List of corresponding indices in the reference image along each axis (x, y and z axis - in this order).
    ///
    /// Voxels in the images are placed on a grid. Each voxel has an index along each axis.
    /// Voxels lying on a perpendicular line to an axis share the same index(and coordinate) along this axis.
    /// Function finds corresponding indices in the reference image, this is:
    ///     for each axis and
    ///         for each index i of voxels in this image along given axis
    ///             index in the reference image that has the closest coordinate to the index i.
    ///
    /// \return Vector of vectors of corresponding indices in the reference image.
    ///         Outer vector size is equal to number of dimensions of the image.
    ///         Inner vectors have lengths according to getReversedShape().
    ///
    ///////////////////////////////////////////////////////////////////////////////
    vector<vector<int> > getCorresponding(Image& reference);

    /// \brief Checks whether point with given coordinates lies inside the image.
    ///
    /// \param x x coordinate of the query point.
    /// \param y (optional) y coordinate of the query point (if the image has 2 or 3 dimensions).
    /// \param z (optional) z coordinate of the query point (if the image has 3 dimensions).
    ///
    /// \return True if point lies inside the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool isPointInside(double x, double y = nan(""), double z = nan(""));

    /// \brief Number of dimensions of the image.
    ///
    /// \return Number of dimensions of the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual int getNdim() const = 0;

    /// \brief Number of voxles in the image.
    ///
    /// \return Number of voxles in the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual int getNelems() const = 0;

    /// \brief Maximum value of the image.
    ///
    /// \return Maximum value of the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual double getMaxValue() const = 0;

    /// \brief Shape of the image.
    ///
    /// \return Vector of numbers of voxels along each axis (z, y and x axis - in this order).
    ///         Vector size is equal to number of dimensions of the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual vector<int> getShape() const = 0;

    /// \brief Reversed shape of the image.
    ///
    /// \return Vector of numbers of voxels along each axis (x, y and z axis - in this order).
    ///         Vector size is equal to number of dimensions of the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual vector<int> getReversedShape() const = 0;

    /// \brief Prints the image nicely.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void print() const = 0;

    /// \brief Returns image matrix as an linearized (1-dimensional) array.
    ///
    /// \note Returned array has to be freed by the user.
    ///
    /// \return Linearized array of doubles containing image values.
    ///         Array size is equal to number of elements of the image (getNelems()).
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual double* dataToArray() const = 0;

public:
    vector<double> start_;
    vector<double> spacing_;
    vector<double> end_;
    vector<vector<double> > coordinates_;
};

class Image1D : public Image
{
public:
    /// \brief Constructs 1-dimensional Image.
    ///
    /// \param start   Vector of coordinates of the center of the most left voxel.
    /// \param spacing Vector of distances between centers of adjacent voxels.
    /// \param data    Vector containing values of each voxel in the image.
    ///                Values start from the most left and the next voxels go along the increasing x axis.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Image1D(vector<double> start, vector<double> spacing, vector<double> data);

    virtual ~Image1D();

    virtual int getNdim() const;

    virtual int getNelems() const;

    virtual double getMaxValue() const;

    virtual vector<int> getShape() const;

    virtual vector<int> getReversedShape() const;

    virtual void print() const;

    virtual double* dataToArray() const;

    /// \brief Checks whether there is a voxel in the image with given index.
    ///
    /// \param i Index along x axis.
    ///
    /// \return True if i is a valid index.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool isIndexInside(int i) const;

    /// \brief Return the matrix with image values.
    ///
    /// \return Vector containing values of each voxel in the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const vector<double>& data() const;

private:
    vector<double> data_;
};


class Image2D : public Image
{
public:
    /// \brief Constructs 2-dimensional Image.
    ///
    /// \param start   Vector of coordinates of the center of the most left top voxel (in this order).
    /// \param spacing Vector of distances between centers of adjacent voxels along each axis
    ///                (x and y axis - in this order).
    /// \param data    Vector of vectors containing values of each voxel in the image.
    ///                Values start from the most left and the next voxels go along the increasing:
    ///                x axis - inner vectors,
    ///                y axis - outer vector.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Image2D(vector<double> start, vector<double> spacing, vector<vector<double> > data);

    virtual ~Image2D();

    virtual int getNdim() const;

    virtual int getNelems() const;

    virtual double getMaxValue() const;

    virtual vector<int> getShape() const;

    virtual vector<int> getReversedShape() const;

    virtual void print() const;

    virtual double* dataToArray() const;

    /// \brief Checks whether there is a voxel in the image with given indices.
    ///
    /// \param i Index along x axis.
    /// \param j Index along y axis.
    ///
    /// \return True if i and j are valid indices.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool isIndexInside(int i, int j) const;

    /// \brief Return the matrix with image values.
    ///
    /// \return Vector of vectors containing values of each voxel in the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const vector<vector<double> >& data() const;

private:
    vector<vector<double> > data_;
};


class Image3D : public Image
{
public:
    /// \brief Constructs 3-dimensional Image.
    ///
    /// \param start   Vector of coordinates of the center of the most left top front voxel (in this order).
    /// \param spacing Vector of distances between centers of adjacent voxels along each axis
    ///                (x, y and z axis - in this order).
    /// \param data    Vector of vectors of vectors containing values of each voxel in the image.
    ///                Values start from the most left and the next voxels go along the increasing:
    ///                x axis - most inner vectors,
    ///                y axis - middle vectors,
    ///                z axis - most outer vector.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Image3D(vector<double> start, vector<double> spacing, vector<vector<vector<double> > > data);

    virtual ~Image3D();

    virtual int getNdim() const;

    virtual int getNelems() const;

    virtual double getMaxValue() const;

    virtual vector<int> getShape() const;

    virtual vector<int> getReversedShape() const;

    virtual void print() const;

    virtual double* dataToArray() const;

    /// \brief Checks whether there is a voxel in the image with given indices.
    ///
    /// \param i Index along x axis.
    /// \param j Index along y axis.
    /// \param k Index along z axis.
    ///
    /// \return True if i, j and k are valid indices.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool isIndexInside(int i, int j, int k) const;

    /// \brief Return the matrix with image values.
    ///
    /// \return Vector of vectors of vectors containing values of each voxel in the image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const vector<vector<vector<double> > >& data() const;

private:
    vector<vector<vector<double> > > data_;
};

#endif
