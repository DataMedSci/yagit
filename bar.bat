@echo off

set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=OFF

@REM set GAMMA_VERSION=SEQUENTIAL
set GAMMA_VERSION=THREADED

set BUILD_EXAMPLES=ON
set BUILD_PERFORMANCE_TESTING=OFF

set REF_IMG=original_dose_beam_4.dcm
set EVAL_IMG=logfile_dose_beam_4.dcm

set INSTALL_DIR=./yagit


@REM ============================================================
if not exist build\CMakeCache.txt (
    echo CONFIGURING CMAKE FIRST TIME...
    mkdir build
    cd build
    conan install ..
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
         -DBUILD_EXAMPLES=%BUILD_EXAMPLES% ^
         -DBUILD_PERFORMANCE_TESTING=%BUILD_PERFORMANCE_TESTING%


@REM ============================================================
echo:
echo COMPILING...
cmake --build . --config %BUILD_TYPE%
set COMPILE_RESULT=%ERRORLEVEL%
cd ..

if %COMPILE_RESULT% NEQ 0 (
    exit /b %COMPILE_RESULT%
)


@REM ============================================================
if %BUILD_EXAMPLES% == ON (
    echo:
    echo RUNNING EXAMPLES...
    build\examples\%BUILD_TYPE%\gamma2DInterp.exe %REF_IMG% %EVAL_IMG%
    echo:
    build\examples\%BUILD_TYPE%\gamma25D.exe %REF_IMG% %EVAL_IMG%
    echo:
    build\examples\%BUILD_TYPE%\gamma3D.exe %REF_IMG% %EVAL_IMG%
    echo:
    build\examples\%BUILD_TYPE%\gammaImage.exe
)

if %BUILD_PERFORMANCE_TESTING% == ON (
    echo:
    echo RUNNING PERFORMANCE TEST...
    build\tests\performance\%BUILD_TYPE%\perfTest.exe %REF_IMG% %EVAL_IMG% gammaTimes.csv
)


@REM ============================================================
echo:
echo INSTALLING...
IF "%INSTALL_DIR%" NEQ "" (
    echo INSTALLING TO %INSTALL_DIR%
    cmake --install build --prefix %INSTALL_DIR%
) else (
    echo INSTALLING TO SYSTEM DIRECTORY
    echo MAKE SURE YOU RUN THIS AS ADMINISTRATOR
    cmake --install build
)
