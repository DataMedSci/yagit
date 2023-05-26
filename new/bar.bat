@echo off

cd build
echo CONFIGURING CMAKE...
cmake .. -DBUILD_EXAMPLES=OFF

echo:
echo COMPILING...
cmake --build . --config=Release
set COMPILE_RESULT=%ERRORLEVEL%
cd ..

if %COMPILE_RESULT% EQU 0 (
    echo:
    echo RUNNING...
    @REM build\examples\Release\gamma2D.exe original_dose_beam_4.dcm logfile_dose_beam_4.dcm
    @REM build\examples\Release\gamma25D.exe original_dose_beam_4.dcm logfile_dose_beam_4.dcm
    @REM build\examples\Release\gamma3DInterp.exe original_dose_beam_4.dcm logfile_dose_beam_4.dcm
    @REM build\examples\Release\gammaImage.exe
    build\Release\testWendling original_dose_beam_4.dcm logfile_dose_beam_4.dcm
)
