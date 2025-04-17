@echo off
setlocal
pushd %~dp0

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

set BUILD_EXAMPLES=OFF
set BUILD_TESTING=OFF
set BUILD_PERFORMANCE_TESTING=OFF

set RUN_EXAMPLES=%BUILD_EXAMPLES%
set RUN_TESTING=%BUILD_TESTING%
set RUN_PERFORMANCE_TESTING=%BUILD_PERFORMANCE_TESTING%

set REF_IMG=img_reference.dcm
set EVAL_IMG=img_evaluated.dcm

set BUILD_DOCUMENTATION=OFF

set INSTALL=OFF
set INSTALL_DIR=./yagit


:: ============================================================
mkdir build
cd build

:: ============================================================
set DEPENDENCIES_PATHS=""
set TOOLCHAIN_FILE=""

set "GDCM_PATH=%cd:\=/%/deps/GDCM/build/installed"
set "XSIMD_PATH=%cd:\=/%/deps/xsimd/build/installed"
set "GTEST_PATH=%cd:\=/%/deps/googletest/build/installed"

:: workaround for missing 'or' operator in batch
if %INSTALL_DEPENDENCIES% == LOCAL set INSTALL_LOCAL_GLOBAL=y
if %INSTALL_DEPENDENCIES% == GLOBAL set INSTALL_LOCAL_GLOBAL=y

if DEFINED INSTALL_LOCAL_GLOBAL (
    echo INSTALLING DEPENDENCIES...
    mkdir deps
    cd deps

    call :install_lib https://github.com/malaterre/GDCM.git v3.0.22 %INSTALL_DEPENDENCIES%
    call :install_lib https://github.com/xtensor-stack/xsimd.git 11.1.0 %INSTALL_DEPENDENCIES%
    call :install_lib https://github.com/google/googletest.git v1.13.0 %INSTALL_DEPENDENCIES%

    if %INSTALL_DEPENDENCIES% == LOCAL (
        set DEPENDENCIES_PATHS="%GDCM_PATH%;%XSIMD_PATH%;%GTEST_PATH%"
    )

    cd ..
) else if %INSTALL_DEPENDENCIES% == CONAN (
    echo INSTALLING DEPENDENCIES...

    if not exist deps_conan (
        mkdir deps_conan
        cd deps_conan
        :: this command works with conan2 and conan1
        conan install ../.. --output-folder . --build missing
        cd ..
    )
    set TOOLCHAIN_FILE=deps_conan/conan_toolchain.cmake
)


:: ============================================================
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


:: ============================================================
echo:
echo BUILDING...
cmake --build . --config %BUILD_TYPE% -j
set COMPILE_RESULT=%ERRORLEVEL%
cd ..

if %COMPILE_RESULT% NEQ 0 (
    popd
    exit /b %COMPILE_RESULT%
)


:: ============================================================
if %RUN_EXAMPLES% == ON (
    echo:
    echo RUNNING EXAMPLES...
    echo GAMMA SIMPLE
    build\examples\%BUILD_TYPE%\gammaSimple.exe
    echo: & echo GAMMA 3D
    build\examples\%BUILD_TYPE%\gamma3D.exe %REF_IMG% %EVAL_IMG%
    echo: & echo GAMMA WITH INTERP
    build\examples\%BUILD_TYPE%\gammaWithInterp.exe %REF_IMG% %EVAL_IMG%
)

if %RUN_TESTING% == ON (
    echo:
    echo RUNNING UNIT TESTS...
    ctest -C %BUILD_TYPE% --test-dir build --output-on-failure
    @REM build\tests\manual\%BUILD_TYPE%\simulatedWendling.exe
)

if %RUN_PERFORMANCE_TESTING% == ON (
    echo:
    echo RUNNING PERFORMANCE TEST...
    echo GAMMA PERF
    build\tests\performance\%BUILD_TYPE%\gammaPerf.exe %REF_IMG% %EVAL_IMG% gammaTimes.csv
    echo: & echo INTERP PERF
    build\tests\performance\%BUILD_TYPE%\interpPerf.exe %EVAL_IMG%
)


:: ============================================================
set YAGIT_DIR=%cd:\=/%

:: save git tag to variable
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


:: ============================================================
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


:: ============================================================
popd
goto :eof

:install_lib
    :: %1 - url to git repository of the library that will be installed
    :: %2 - tag or branch of the library
    :: %3 - installation mode (LOCAL or GLOBAL)

    :: extract repository name from url
    for %%A in ("%1") do set REPO_NAME=%%~nA

    if not exist %REPO_NAME% (
        :: clone git repo
        git clone %1 -b %2 --depth 1 -c advice.detachedHead=false
    )

    if not exist %REPO_NAME%/build (
        cd %REPO_NAME%
        mkdir build
        cd build

        :: configure and build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        cmake --build . --config Release -j

        :: install
        if %3 == LOCAL (
            cmake --install . --prefix ./installed
        ) else if %3 == GLOBAL (
            cmake --install .
        )

        cd ../..
    )
    goto :eof
