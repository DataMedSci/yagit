Introduction
============

The purpose of the *yet Another Gamma Index Tool* project was to create an open-source tool for performing the Gamma Index comparison of 1D, 2D, 3D images within a very short time. There could be observed a fair amount of enthusiasm among potential clients, as they can experience a shortage of free, fast and convenient applications offering Gamma Index analysis.

Modules
.......

*yAGIT* application consists of 3 modules:
 1. **Core module** - performs a comparison of two images passed as linearised arrays of real numbers - the dose values at given points. This module is completely independent and can be used separately. In order to accelerate the calculations, it has been deprived of all the responsibilities that are not necessary for comparison. Module contains implementation of // TODO versions of Gamma Index algorithm:

     a. classic version using CPU - it is very slow, and was only attached as some kind of reference to the spiral versions
    // TODO

 2. **Wrapper module** - reads images from DICOM files and stores them into linearised tables so that the core module can make use of them. It is also responsible for saving the resulting Gamma Index matrix into an output DICOM file. It uses an external library *Imebra*

 3. **GUI module** - it is a Graphical User Interface created using *wxWidgets* library that allows to select images for comparison, set a wide range of parameters (which can be saved and stored in the form of profiles), perform comparisons and write the output to a DICOM file. It has dependencies to all of the other modules.

Advantages of our solution
..........................
 - **speed** (the most important feature of *yAGIT*) - for all available test data, even for large 3D images (219 x 147 x 351), computation time using the spiral version was less than 1 minute
 - ability to perform different types of analysis (1D, 2D, 3D, 2.5D, 2DFrom3D)
 - no use of interpolation - we decided not to include interpolation in our application for a few reasons:

    - the use of interpolation, makes the program run much slower
    - users do not need to worry about errors associated with the use of interpolation (there is no phenomenon of over-fitting compared images)
    - some users do not want the application to use the interpolation; if one would like to have the images interpolated before performing the Gamma Index comparison, they can interpolate the input image themselves and pass only the modified samples to the *yAGIT* application.

 - portability - application can be used by both Windows and Linux users
 - modularity - the application is divided into modules, users decide how they want to use the application, for example, users can use the API provided by the *core module* or a *wrapper module*, but they can also use the *GUI module*
 - simple and user friendly GUI
 - support for DICOM images
 - support for images represented as linearised floating point C++ arrays
 - support for GPU acceleration
 - supplied test framework // TODO - is it still true?

Limitations
...........
 - lack of built-in visualization tool
 - support only for files of DICOM type (although user can easily write their own wrapper and integrate it with our application)
 - lack of support for images not specified on a regular grid (images with different pixel spacing along one axis)
 - lack of support for DICOMDIR images

