@echo off
cd build
echo COMPILING...
cmake --build . --config=Release
set COMPILE_RESULT=%ERRORLEVEL%
cd ..
if %COMPILE_RESULT% EQU 0 (
    echo:
    echo RUNNING...
    @REM build\Release\test_interp_2d_3d.exe
    @REM build\examples\Release\gammaInterp3D.exe original_dose_beam_4.dcm logfile_dose_beam_4.dcm
    build\examples\Release\gammaImage.exe
)
