# yet Another Gamma Index Tool

Application and library for performing efficient comparisons of 2D, 3D DICOM images using gamma index concept.

## Used libraries
* [simdpp](https://github.com/p12tic/libsimdpp) - code vectorization
* [VexCL](https://github.com/ddemidov/vexcl) - OpenCL/CUDA GPU offloading
* [GDCM](http://gdcm.sourceforge.net/) - interacting with DICOM files
* [boost](https://www.boost.org/) - unit testing
* [Qt5](https://www.qt.io/) - GUI

## Building

Building project requires CMake.
To generate project compiler with C++20 support is required.
CMake environment requires Qt5, boost and GDCM libraries to be installed
the project does not pull them automatically.

## C++ standard
Project is written under C++20

## Documentation
[Documentation](http://gi-yagit.readthedocs.io/en/latest/)