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

#ifndef GI_SOLVER_H
#define GI_SOLVER_H

#include <utility>
#include <vector>
#include <string>

using namespace std;


class Image1D;

class Image2D;

class Image3D;

class Solver
{
public:
    /// \brief Constructs the base for GI solvers.
    ///
    /// \param percentage     This is the DD parameter passed in %. Standard value is 3.0.
    /// \param dta            This is the DTA parameter passed in mm. Standard value is 3.0.
    /// \param referenceValue The value that is multiplied by the percentage to calculate
    ///                       the denominator of the dd component. Used only if local_ flag is not set.
    ///                       Standard value is the maximum value in reference image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Solver(double percentage, double dta, double referenceValue);

    /// \brief Calculates dose difference component of Gamma Index.
    ///
    /// \note dd = (ref - tar)^2 / (DD * referenceValue)^2
    ///
    /// \param ref Value of currently analysed point in the reference image.
    /// \param tar Value of currently analysed point in the target image.
    ///
    /// \return Calculated value of dd component.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double doseDifference(double ref, double tar) const;

    /// \brief Calculates distance to agreement component of Gamma Index.
    ///
    /// \note dta = (x^2 + y^2 + z^2) / DTA^2
    ///
    /// \param x  The distance, along x axis, between currently analysed reference and target point.
    /// \param y  The distance, along y axis, between currently analysed reference and target point.
    /// \param z  The distance, along z axis, between currently analysed reference and target point.
    ///
    /// \return Calculated value of dta component.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double distanceToAgreement(double x, double y, double z) const;

    /// \brief Calculates square of gamma function.
    ///
    /// \note gamma_function = sqrt(dd + dta)
    ///
    /// \param ref Value of currently analysed point in the reference image.
    /// \param tar Value of currently analysed point in the target image.
    /// \param x   The distance, along x axis, between currently analysed reference and target point.
    /// \param y   The distance, along y axis, between currently analysed reference and target point.
    /// \param z   The distance, along z axis, between currently analysed reference and target point.
    ///
    /// \return Square of gamma function.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double gammaEquation(double ref, double tar, double x, double y, double z) const;


    /// \brief Recalculates ddSquare_ variable if value of percentage_,
    ///        referenceValue_ or local_ change.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void calculateDD();


    ////////////////////////////////////GETTERS////////////////////////////////////

    double percentage() const;

    double dta() const;

    double referenceValue() const;

    double limit() const;

    bool local() const;


    ////////////////////////////////////SETTERS////////////////////////////////////

    void setReferenceValue(double referenceValue);

    void setLimit(double limit);

    void setLocal(bool local);

public:
    // Helper structure for sorting voxels according to their distance from a given point (center of the spiral)
    typedef struct Voxel
    {
        int x, y, z;
        double dist;

        Voxel() : dist(0.0), x(0), y(0), z(0) {}

        Voxel(double dist_, int x_, int y_ = 0, int z_ = 0) : dist(dist_), x(x_), y(y_), z(z_) {}

        static bool compare(Voxel a, Voxel b) { return a.dist < b.dist; }
    } Voxel;

protected:
    double percentage_;
    double dtaSquare_;
    double ddSquare_;
    double referenceValue_;
    double limit_;
    bool local_;
};


class Solver1D : public Solver
{
public:
    /// \brief Constructs the base for 1-dimensional GI solvers.
    ///
    /// \param reference  First image that will be compared by the solver.
    /// \param target     Second image that will be compared by the solver.
    /// \param percentage This is the DD parameter passed in %. Standard value is 3.0.
    /// \param dta        This is the DTA parameter passed in mm. Standard value is 3.0.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Solver1D(Image1D& reference, Image1D& target, double percentage, double dta);

    /// \brief Performs 1-dimensional Gamma Index calculation.
    ///
    /// \return 1D image containing array of calculated GI values.
    ///         Returned image has the same shape as reference_ image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Image1D calculateGamma() = 0;

protected:
    Image1D& reference_;
    Image1D& target_;
};


class Solver2D : public Solver
{
public:
    /// \brief Constructs the base for 2-dimensional GI solvers.
    ///
    /// \param reference  First image that will be compared by the solver.
    /// \param target     Second image that will be compared by the solver.
    /// \param percentage This is the DD parameter passed in %. Standard value is 3.0.
    /// \param            dta This is the DTA parameter passed in mm. Standard value is 3.0.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Solver2D(Image2D& reference, Image2D& target, double percentage, double dta);

    /// \brief Performs 2-dimensional Gamma Index calculation.
    ///
    /// \return 2D image containing array of calculated GI values.
    ///         Returned image has the same shape as reference_ image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Image2D calculateGamma() = 0;

protected:
    Image2D& reference_;
    Image2D& target_;
};


class Solver3D : public Solver
{
public:
    /// \brief Constructs the base for 3-dimensional GI solvers.
    ///
    /// \param reference  First image that will be compared by the solver.
    /// \param target     Second image that will be compared by the solver.
    /// \param percentage This is the DD parameter passed in %. Standard value is 3.0.
    /// \param dta        This is the DTA parameter passed in mm. Standard value is 3.0.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Solver3D(Image3D& reference, Image3D& target, double percentage, double dta);

    /// \brief Performs 3-dimensional Gamma Index calculation.
    ///
    /// \return 3D image containing array of calculated GI values.
    ///         Returned image has the same shape as reference_ image.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Image3D calculateGamma() = 0;


    /// \brief Performs Gamma Index calculation for one slice of reference_ image
    ///        in comparison to the whole target_ image.
    ///
    /// \param plane  Specifies the slice orientation:
    ///               - 0 - parallel to XY plane,
    ///               - 1 - parallel to XZ plane,
    ///               - 2 - parallel to YZ plane.
    /// \param slice  Specifies for which reference_ slice (in the given orientation)
    ///               the comparison should be performed.
    ///               First slice has number 0.
    ///
    /// \return 2D image containing array of calculated GI values.
    ///         Returned image has the same shape as reference_ slice that it was calculated for.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Image2D calculateGamma2_5D(int plane, int slice) = 0;

protected:
    Image3D& reference_;
    Image3D& target_;
};


///////////////////////////////CLASSIC GI SOLVERS///////////////////////////////

class ClassicSolver1D : public Solver1D
{
public:
    ClassicSolver1D(Image1D& reference, Image1D& target, double percentage, double dta);

    virtual Image1D calculateGamma();
};


class ClassicSolver2D : public Solver2D
{
public:
    ClassicSolver2D(Image2D& reference, Image2D& target, double percentage, double dta);

    virtual Image2D calculateGamma();
};


class ClassicSolver3D : public Solver3D
{
public:
    ClassicSolver3D(Image3D& reference, Image3D& target, double percentage, double dta);

    virtual Image3D calculateGamma();

    virtual Image2D calculateGamma2_5D(int plane, int slice);

private:
    Image2D calculateGamma2_5DSliceXY(int slice);

    Image2D calculateGamma2_5DSliceXZ(int slice);

    Image2D calculateGamma2_5DSliceYZ(int slice);
};


///////////////////////////////SPIRAL GI SOLVERS///////////////////////////////

class SpiralSolver1D : public Solver1D
{
public:
    SpiralSolver1D(Image1D& reference, Image1D& target, double percentage, double dta);

    virtual Image1D calculateGamma();

protected:
    /// \brief Calculates 1D spiral that contains target points against which the reference point
    ///        is consecutively compared.
    ///
    /// \param left Returns the excursion of the most left point of the spiral
    ///             from the center of the coordinate system.
    ///
    /// \return Ordered vector of Voxels that the spiral consists of.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual vector<Voxel> getIndicesOrder(int& left) const = 0;
};


class SpiralSolver2D : public Solver2D
{
public:
    SpiralSolver2D(Image2D& reference, Image2D& target, double percentage, double dta);

    virtual Image2D calculateGamma();

protected:
    /// \brief Calculates 2D spiral that contains target points against which the reference point
    ///        is consecutively compared.
    ///
    /// \param left Returns the excursion of the most left point of the spiral
    ///             from the center of the coordinate system.
    /// \param top  Returns the excursion of the most top point of the spiral
    ///             from the center of the coordinate system.
    ///
    /// \return Ordered vector of Voxels that the spiral consists of.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual vector<Voxel> getIndicesOrder(int& left, int& top) const = 0;
};


class SpiralSolver3D : public Solver3D
{
public:
    SpiralSolver3D(Image3D& reference, Image3D& target, double percentage, double dta);

    virtual Image3D calculateGamma();

    virtual Image2D calculateGamma2_5D(int plane, int slice);

protected:
    /// \brief Calculates 3D spiral that contains target points against which the reference point
    ///        is consecutively compared.
    ///
    /// \param left  Returns the excursion of the most left point of the spiral
    ///              from the center of the coordinate system.
    /// \param top   Returns the excursion of the most top point of the spiral
    ///              from the center of the coordinate system.
    /// \param front Returns the excursion of the most front point of the spiral
    ///              from the center of the coordinate system.
    ///
    /// \return Ordered vector of Voxels that the spiral consists of.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual vector<Voxel> getIndicesOrder(int& left, int& top, int& front) const = 0;

private:
    Image2D calculateGamma2_5DSliceXY(int slice);

    Image2D calculateGamma2_5DSliceXZ(int slice);

    Image2D calculateGamma2_5DSliceYZ(int slice);
};


class SpiralRectangleSolver1D : public SpiralSolver1D
{
public:
    SpiralRectangleSolver1D(Image1D& reference, Image1D& target, double percentage, double dta);

protected:
    virtual vector<Voxel> getIndicesOrder(int& left) const;
};


class SpiralRectangleSolver2D : public SpiralSolver2D
{
public:
    SpiralRectangleSolver2D(Image2D& reference, Image2D& target, double percentage, double dta);

protected:
    virtual vector<Voxel> getIndicesOrder(int& left, int& top) const;
};


class SpiralRectangleSolver3D : public SpiralSolver3D
{
public:
    SpiralRectangleSolver3D(Image3D& reference, Image3D& target, double percentage, double dta);

protected:
    virtual vector<Voxel> getIndicesOrder(int& left, int& top, int& front) const;
};


class SpiralNoRectangleSolver1D : public SpiralSolver1D
{
public:
    SpiralNoRectangleSolver1D(Image1D& reference, Image1D& target, double percentage, double dta);

protected:
    virtual vector<Voxel> getIndicesOrder(int& left) const;
};


class SpiralNoRectangleSolver2D : public SpiralSolver2D
{
public:
    SpiralNoRectangleSolver2D(Image2D& reference, Image2D& target, double percentage, double dta);

protected:
    virtual vector<Voxel> getIndicesOrder(int& left, int& top) const;
};


class SpiralNoRectangleSolver3D : public SpiralSolver3D
{
public:
    SpiralNoRectangleSolver3D(Image3D& reference, Image3D& target, double percentage, double dta);

protected:
    virtual vector<Voxel> getIndicesOrder(int& left, int& top, int& front) const;
};

#endif
