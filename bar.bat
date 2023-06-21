@echo off

@REM echo CONFIGURING CMAKE FIRST TIME...
@REM mkdir build
@REM cd build
@REM conan install ..
@REM cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
@REM cd ..

echo:
echo CONFIGURING CMAKE...
cd build
@REM set GAMMA_VERSION=SEQUENTIAL
set GAMMA_VERSION=THREADED
set OTHER_OPTIONS=-DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON -DBUILD_PERFORMANCE_TESTING=ON
cmake .. -DGAMMA_VERSION=%GAMMA_VERSION% %OTHER_OPTIONS%

echo:
echo COMPILING...
cmake --build . --config Release
set COMPILE_RESULT=%ERRORLEVEL%
cd ..

if %COMPILE_RESULT% NEQ 0 (
    exit /b %COMPILE_RESULT%
)

echo:
echo RUNNING...
set REF_IMG=original_dose_beam_4.dcm
set EVAL_IMG=logfile_dose_beam_4.dcm

build\tests\performance\Release\perfTest.exe %REF_IMG% %EVAL_IMG% gammaTimes.csv

@REM echo:
@REM echo RUNNING EXAMPLES...
@REM build\examples\Release\gamma2DInterp.exe %REF_IMG% %EVAL_IMG%
@REM echo:
@REM build\examples\Release\gamma25D.exe %REF_IMG% %EVAL_IMG%
@REM echo:
@REM build\examples\Release\gamma3D.exe %REF_IMG% %EVAL_IMG%
@REM echo:
@REM build\examples\Release\gammaImage.exe
