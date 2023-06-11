# yet Another Gamma Index Tool

Library written in C++ for performing efficient comparisons of DICOM images using 2D, 2.5D and 3D gamma index concept.

## Installation

Build steps:
1. Creating a directory where the library will be built
```
mkdir build && cd build
```

2. Installing packages and running cmake
    * Using Conan package manager
    ```
    conan install ..
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
    ```
    * Using apt package manager
    ```
    sudo apt-get install libgdcm-dev
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

3. Building the library
```
cmake --build . --config Release
```

## Examples

Example usage of the library is located in `examples` directory.  
To build examples, enable `BUILD_EXAMPLES` option during installation.
```
cmake .. -DBUILD_EXAMPLES=ON
```
And to start `gammaImage` example, run
* on Linux
```
./examples/gammaImage
```
* on Windows
```
examples\Release\gammaImage.exe
```
