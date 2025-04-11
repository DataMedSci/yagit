@echo off
setlocal

set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=OFF

@REM set INSTALL_DEPENDENCIES=OFF
set INSTALL_DEPENDENCIES=LOCAL
@REM set INSTALL_DEPENDENCIES=GLOBAL   %= requires administrator privileges =%
@REM set INSTALL_DEPENDENCIES=CONAN

@REM set GAMMA_VERSION=SEQUENTIAL
set GAMMA_VERSION=THREADS
@REM set GAMMA_VERSION=SIMD
@REM set GAMMA_VERSION=THREADS_SIMD

set SIMD_EXTENSION=DEFAULT
@REM set SIMD_EXTENSION=AVX2

set ENABLE_FMA=OFF

set BUILD_EXAMPLES=ON
set BUILD_TESTING=OFF
set BUILD_PERFORMANCE_TESTING=OFF

set REF_IMG=img_reference.dcm
set EVAL_IMG=img_evaluated.dcm

set BUILD_DOCUMENTATION=OFF

set INSTALL=OFF
set INSTALL_DIR=./yagit


@REM ============================================================
mkdir build
cd build

@REM ============================================================
set DEPENDENCIES_PATHS=""
set TOOLCHAIN_FILE=""

set "GDCM_PATH=%cd:\=/%/deps/GDCM/build/installed"
set "XSIMD_PATH=%cd:\=/%/deps/xsimd/build/installed"
set "GTEST_PATH=%cd:\=/%/deps/googletest/build/installed"

@REM workaround for missing 'or' operator in batch
if %INSTALL_DEPENDENCIES% == LOCAL set INSTALL_LOCAL_GLOBAL=y
if %INSTALL_DEPENDENCIES% == GLOBAL set INSTALL_LOCAL_GLOBAL=y

if DEFINED INSTALL_LOCAL_GLOBAL (
    echo INSTALLING DEPENDENCIES...
    mkdir deps
    cd deps

    @REM GDCM
    if not exist GDCM (
        git clone https://github.com/malaterre/GDCM.git -b v3.0.22
        call :install GDCM %INSTALL_DEPENDENCIES%
    )

    @REM xsimd
    if not exist xsimd (
        git clone https://github.com/xtensor-stack/xsimd.git -b 11.1.0
        call :install xsimd %INSTALL_DEPENDENCIES%
    )

    @REM GoogleTest
    if not exist googletest (
        git clone https://github.com/google/googletest.git -b v1.13.0
        call :install googletest %INSTALL_DEPENDENCIES%
    )

    if %INSTALL_DEPENDENCIES% == LOCAL (
        set DEPENDENCIES_PATHS="%GDCM_PATH%;%XSIMD_PATH%;%GTEST_PATH%"
    )

    cd ..
) else if %INSTALL_DEPENDENCIES% == CONAN (
    echo INSTALLING DEPENDENCIES...

    if not exist deps_conan (
        mkdir deps_conan
        cd deps_conan
        conan install ../.. --output-folder . --build missing
        cd ..
    )
    set TOOLCHAIN_FILE=deps_conan/conan_toolchain.cmake
)


@REM ============================================================
echo:
echo CONFIGURING CMAKE...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
         -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS% ^
         -DGAMMA_VERSION=%GAMMA_VERSION% ^
         -DSIMD_EXTENSION=%SIMD_EXTENSION% ^
         -DENABLE_FMA=%ENABLE_FMA% ^
         -DBUILD_EXAMPLES=%BUILD_EXAMPLES% ^
         -DBUILD_TESTING=%BUILD_TESTING% ^
         -DBUILD_PERFORMANCE_TESTING=%BUILD_PERFORMANCE_TESTING% ^
         -DCMAKE_PREFIX_PATH=%DEPENDENCIES_PATHS% ^
         -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE%


@REM ============================================================
echo:
echo BUILDING...
cmake --build . --config %BUILD_TYPE% -j
set COMPILE_RESULT=%ERRORLEVEL%
cd ..

if %COMPILE_RESULT% NEQ 0 (
    exit /b %COMPILE_RESULT%
)


@REM ============================================================
if %BUILD_EXAMPLES% == ON (
    echo:
    echo RUNNING EXAMPLES...
    echo GAMMA SIMPLE
    build\examples\%BUILD_TYPE%\gammaSimple.exe
    echo: & echo GAMMA 3D
    build\examples\%BUILD_TYPE%\gamma3D.exe %REF_IMG% %EVAL_IMG%
    echo: & echo GAMMA WITH INTERP
    build\examples\%BUILD_TYPE%\gammaWithInterp.exe %REF_IMG% %EVAL_IMG%
)

if %BUILD_TESTING% == ON (
    echo:
    echo RUNNING UNIT TESTS...
    ctest -C %BUILD_TYPE% --test-dir build --output-on-failure
    @REM build\tests\manual\%BUILD_TYPE%\simulatedWendling.exe
)

if %BUILD_PERFORMANCE_TESTING% == ON (
    echo:
    echo RUNNING PERFORMANCE TEST...
    echo GAMMA PERF
    build\tests\performance\%BUILD_TYPE%\gammaPerf.exe %REF_IMG% %EVAL_IMG% gammaTimes.csv
    echo: & echo INTERP PERF
    build\tests\performance\%BUILD_TYPE%\interpPerf.exe %EVAL_IMG%
)


@REM ============================================================
set YAGIT_DIR=%cd:\=/%

@REM save git tag to variable
for /f %%v in ('git describe --tags --dirty --match "v*"') do set VERSION=%%v

if %BUILD_DOCUMENTATION% == ON (
    echo:
    echo BUILDING DOCUMENTATION...
    cd docs
    (type Doxyfile & echo PROJECT_NUMBER=%VERSION%) | doxygen -
    set SPHINXOPTS=-Dversion=%VERSION%
    call make.bat html
    echo DOCUMENTATION MAIN PAGE: %YAGIT_DIR%/docs/build/html/index.html
    cd ..
)


@REM ============================================================
if %INSTALL% == ON (
    echo:
    echo INSTALLING...
    if "%INSTALL_DIR%" NEQ "" (
        echo INSTALLING IN %INSTALL_DIR%
        cmake --install build --prefix %INSTALL_DIR%
    ) else (
        echo INSTALLING IN SYSTEM DIRECTORY
        echo MAKE SURE YOU RUN THIS AS ADMINISTRATOR
        cmake --install build
    )
)


@REM ============================================================
goto :eof

:install
@REM %1 - path to the library that will be installed
@REM %2 - installation mode (LOCAL or GLOBAL)

cd %1
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j

if %2 == LOCAL (
    cmake --install . --prefix ./installed
) else if %2 == GLOBAL (
    cmake --install .
)
cd ../..
goto :eof
