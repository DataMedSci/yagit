@echo off
setlocal

set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=OFF

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
if not exist build\CMakeCache.txt (
    echo CONFIGURING CMAKE FIRST TIME...
    mkdir build
    cd build
    conan install .. --output-folder . --build missing
    cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
    cd ..
)


@REM ============================================================
echo:
echo CONFIGURING CMAKE...
cd build
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
         -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS% ^
         -DGAMMA_VERSION=%GAMMA_VERSION% ^
         -DSIMD_EXTENSION=%SIMD_EXTENSION% ^
         -DENABLE_FMA=%ENABLE_FMA% ^
         -DBUILD_EXAMPLES=%BUILD_EXAMPLES% ^
         -DBUILD_TESTING=%BUILD_TESTING% ^
         -DBUILD_PERFORMANCE_TESTING=%BUILD_PERFORMANCE_TESTING%


@REM ============================================================
echo:
echo COMPILING...
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
    IF "%INSTALL_DIR%" NEQ "" (
        echo INSTALLING IN %INSTALL_DIR%
        cmake --install build --prefix %INSTALL_DIR%
    ) else (
        echo INSTALLING IN SYSTEM DIRECTORY
        echo MAKE SURE YOU RUN THIS AS ADMINISTRATOR
        cmake --install build
    )
)
