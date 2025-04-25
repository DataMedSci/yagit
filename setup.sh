#!/bin/bash

BUILD_TYPE=Release
BUILD_SHARED_LIBS=OFF

# INSTALL_DEPENDENCIES=OFF
INSTALL_DEPENDENCIES=LOCAL
# INSTALL_DEPENDENCIES=GLOBAL   # requires root privileges
# INSTALL_DEPENDENCIES=CONAN

# GAMMA_VERSION=SEQUENTIAL
GAMMA_VERSION=THREADS
# GAMMA_VERSION=SIMD
# GAMMA_VERSION=THREADS_SIMD

SIMD_EXTENSION=DEFAULT
# SIMD_EXTENSION=AVX2

ENABLE_FMA=OFF

BUILD_EXAMPLES=OFF
BUILD_TESTING=OFF
BUILD_PERFORMANCE_TESTING=OFF

RUN_EXAMPLES=$BUILD_EXAMPLES
RUN_TESTING=$BUILD_TESTING
RUN_PERFORMANCE_TESTING=$BUILD_PERFORMANCE_TESTING

REF_IMG=img_reference.dcm
EVAL_IMG=img_evaluated.dcm

BUILD_DOCUMENTATION=OFF

INSTALL=OFF
INSTALL_DIR=./yagit


# ============================================================
cd "$(dirname "$0")"
mkdir -p build
cd build

# ============================================================
DEPENDENCIES_PATHS=""
TOOLCHAIN_FILE=""

install_lib () {
    # $1 - url to git repository of the library that will be installed
    # $2 - tag or branch of the library
    # $3 - installation mode (LOCAL or GLOBAL)
    # $4 (optional) - additional options for cmake configure

    # extract repository name from url
    repo_name_git=${1##*/}
    repo_name=${repo_name_git%.git}

    if [ ! -d $repo_name ]; then
        # clone git repo
        git clone $1 -b $2 --depth 1 -c advice.detachedHead=false
    fi

    if [[ -d $repo_name && ! -d $repo_name/build ]]; then
        cd $repo_name
        mkdir -p build && cd build

        # configure and build
        cmake .. -DCMAKE_BUILD_TYPE=Release $4
        cmake --build . --config Release -j

        # install
        if [ $3 == LOCAL ]; then
            cmake --install . --prefix ./installed
        elif [ $3 == GLOBAL ]; then
            sudo cmake --install .
        fi

        cd ../..
    fi
}

if [[ $INSTALL_DEPENDENCIES == LOCAL || $INSTALL_DEPENDENCIES == GLOBAL ]]; then
    echo "INSTALLING DEPENDENCIES..."
    mkdir -p deps && cd deps

    install_lib https://github.com/malaterre/GDCM.git v3.0.22 $INSTALL_DEPENDENCIES "-DGDCM_BUILD_DOCBOOK_MANPAGES=OFF"
    install_lib https://github.com/xtensor-stack/xsimd.git 11.1.0 $INSTALL_DEPENDENCIES
    install_lib https://github.com/google/googletest.git v1.13.0 $INSTALL_DEPENDENCIES

    if [ $INSTALL_DEPENDENCIES == LOCAL ]; then
        GDCM_PATH="$(pwd)/GDCM/build/installed"
        XSIMD_PATH="$(pwd)/xsimd/build/installed"
        GTEST_PATH="$(pwd)/googletest/build/installed"
        DEPENDENCIES_PATHS="$GDCM_PATH;$XSIMD_PATH;$GTEST_PATH"
    fi

    cd ..
elif [ $INSTALL_DEPENDENCIES == CONAN ]; then
    echo "INSTALLING DEPENDENCIES..."

    if [ ! -f deps_conan/conan_toolchain.cmake ]; then
        mkdir -p deps_conan && cd deps_conan
        # this command works with conan2 and conan1
        conan install ../.. --output-folder . --build missing
        cd ..
    fi
    TOOLCHAIN_FILE=deps_conan/conan_toolchain.cmake
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
status=$?; if [ $status -ne 0 ]; then exit $status; fi


# ============================================================
echo ""
echo "BUILDING..."
cmake --build . --config $BUILD_TYPE -j
status=$?; if [ $status -ne 0 ]; then exit $status; fi
cd ..


# ============================================================
if [ $RUN_EXAMPLES == ON ]; then
    echo ""
    echo "RUNNING EXAMPLES..."
    echo "GAMMA SIMPLE"
    ./build/examples/gammaSimple
    echo ""; echo "GAMMA 3D"
    ./build/examples/gamma3D "$REF_IMG" "$EVAL_IMG"
    echo ""; echo "GAMMA WITH INTERP"
    ./build/examples/gammaWithInterp "$REF_IMG" "$EVAL_IMG"
fi

if [ $RUN_TESTING == ON ]; then
    echo ""
    echo "RUNNING UNIT TESTS..."
    ctest -C $BUILD_TYPE --test-dir build --output-on-failure
    # ./build/tests/manual/simulatedWendling
fi

if [ $RUN_PERFORMANCE_TESTING == ON ]; then
    echo ""
    echo "RUNNING PERFORMANCE TESTS..."
    echo "GAMMA PERF"
    ./build/tests/performance/gammaPerf "$REF_IMG" "$EVAL_IMG" gammaTimes.csv
    echo ""; echo "INTERP PERF"
    ./build/tests/performance/interpPerf "$EVAL_IMG"
fi


# ============================================================
if [ $BUILD_DOCUMENTATION == ON ]; then
    echo ""
    echo "BUILDING DOCUMENTATION..."
    cd docs
    VERSION=$(git describe --tags --dirty --match "v*")
    (cat Doxyfile; echo PROJECT_NUMBER=$VERSION) | doxygen -
    make html SPHINXOPTS=-Dversion=$VERSION
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
