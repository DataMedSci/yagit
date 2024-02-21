#!/bin/bash

BUILD_TYPE=Release
BUILD_SHARED_LIBS=OFF

# INSTALL_DEPENDENCIES=OFF
INSTALL_DEPENDENCIES=LOCAL
# INSTALL_DEPENDENCIES=GLOBAL  # requires root privileges
# INSTALL_DEPENDENCIES=CONAN

# GAMMA_VERSION=SEQUENTIAL
GAMMA_VERSION=THREADS
# GAMMA_VERSION=SIMD
# GAMMA_VERSION=THREADS_SIMD

SIMD_EXTENSION=DEFAULT
# SIMD_EXTENSION=AVX2

ENABLE_FMA=OFF

BUILD_EXAMPLES=ON
BUILD_TESTING=OFF
BUILD_PERFORMANCE_TESTING=OFF

REF_IMG=img_reference.dcm
EVAL_IMG=img_evaluated.dcm

BUILD_DOCUMENTATION=OFF

INSTALL=OFF
INSTALL_DIR=./yagit


# ============================================================
mkdir -p build
cd build

# ============================================================

install () {
    # $1 - path to library to install

    cd $1
    mkdir -p build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release -j

    if [ $INSTALL_DEPENDENCIES == LOCAL ]; then
        cmake --install . --prefix ./installed
    elif [ $INSTALL_DEPENDENCIES == GLOBAL ]; then
        sudo cmake --install .
    fi

    cd ../..
}

DEPENDENCIES_PATHS=""
TOOLCHAIN_FILE=""

if [[ $INSTALL_DEPENDENCIES == LOCAL || $INSTALL_DEPENDENCIES == GLOBAL ]]; then
    echo "INSTALLING DEPENDENCIES..."
    mkdir -p deps && cd deps

    # GDCM
    if [ ! -d GDCM ]; then
        git clone https://github.com/malaterre/GDCM.git -b v3.0.22
        install GDCM
    fi

    # xsimd
    if [ ! -d xsimd ]; then
        git clone https://github.com/xtensor-stack/xsimd.git -b 11.1.0
        install xsimd
    fi

    # GoogleTest
    if [ ! -d googletest ]; then
        git clone https://github.com/google/googletest.git -b v1.13.0
        install googletest
    fi

    if [ $INSTALL_DEPENDENCIES == LOCAL ]; then
        GDCM_PATH="$(pwd)/GDCM/build/installed"
        XSIMD_PATH="$(pwd)/xsimd/build/installed"
        GTEST_PATH="$(pwd)/googletest/build/installed"
        DEPENDENCIES_PATHS="$GDCM_PATH;$XSIMD_PATH;$GTEST_PATH"
    fi

    cd ..
elif [ $INSTALL_DEPENDENCIES == CONAN ]; then
    conan install .. --output-folder . --build missing
    TOOLCHAIN_FILE=conan_toolchain.cmake
fi


# ============================================================
echo ""
echo "CONFIGURING CMAKE..."
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
         -DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS \
         -DGAMMA_VERSION=$GAMMA_VERSION \
         -DSIMD_EXTENSION=$SIMD_EXTENSION \
         -DENABLE_FMA=$ENABLE_FMA \
         -DBUILD_EXAMPLES=$BUILD_EXAMPLES \
         -DBUILD_TESTING=$BUILD_TESTING \
         -DBUILD_PERFORMANCE_TESTING=$BUILD_PERFORMANCE_TESTING \
         -DCMAKE_PREFIX_PATH="$DEPENDENCIES_PATHS" \
         -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE"


# ============================================================
echo ""
echo "COMPILING..."
cmake --build . --config $BUILD_TYPE -j
COMPILE_RESULT=$?
cd ..

if [ $COMPILE_RESULT -ne 0 ]; then
    exit $COMPILE_RESULT
fi


# ============================================================
if [ $BUILD_EXAMPLES == ON ]; then
    echo ""
    echo "RUNNING EXAMPLES..."
    ./build/examples/gamma2DInterp "$REF_IMG" "$EVAL_IMG"
    echo ""
    ./build/examples/gamma25D "$REF_IMG" "$EVAL_IMG"
    echo ""
    ./build/examples/gamma3D "$REF_IMG" "$EVAL_IMG"
    echo ""
    ./build/examples/gammaImage
fi

if [ $BUILD_TESTING == ON ]; then
    echo ""
    echo "RUNNING UNIT TESTS..."
    ctest -C $BUILD_TYPE --test-dir build --output-on-failure
fi

if [ $BUILD_PERFORMANCE_TESTING == ON ]; then
    echo ""
    echo "RUNNING PERFORMANCE TEST..."
    ./build/tests/performance/gammaPerf "$REF_IMG" "$EVAL_IMG" gammaTimes.csv
    echo ""
    ./build/tests/performance/interpPerf "$EVAL_IMG"
fi


# ============================================================
if [ $BUILD_DOCUMENTATION == ON ]; then
    echo ""
    echo "BUILDING DOCUMENTATION..."
    cd docs
    doxygen
    make html
    cd ..
    echo DOCUMENTATION MAIN PAGE: $(pwd)/docs/build/html/index.html
fi


# ============================================================
if [ $INSTALL == ON ]; then
    echo ""
    echo "INSTALLING..."
    if [ -n "$INSTALL_DIR" ]; then
        echo "INSTALLING IN $INSTALL_DIR"
        cmake --install build --prefix "$INSTALL_DIR"
    else
        echo "INSTALLING IN SYSTEM DIRECTORY"
        sudo cmake --install build
    fi
fi
