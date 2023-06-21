@echo off

cd build
echo CONFIGURING CMAKE...
@REM set GAMMA_VERSION=SEQUENTIAL
set GAMMA_VERSION=THREADED
set OTHER_OPTIONS=-DBUILD_EXAMPLES=OFF -DBUILD_PERFORMANCE_TESTING=ON
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

@REM RUN EXAMPLES
@REM build\examples\Release\gamma2DInterp.exe %REF_IMG% %EVAL_IMG%
@REM echo:
@REM build\examples\Release\gamma25D.exe %REF_IMG% %EVAL_IMG%
@REM echo:
@REM build\examples\Release\gamma3DWendling.exe %REF_IMG% %EVAL_IMG%
@REM echo:
@REM build\examples\Release\gammaImage.exe