@echo off

cd build
echo CONFIGURING CMAKE...
cmake .. -DBUILD_EXAMPLES=ON

echo:
echo COMPILING...
cmake --build . --config=Release
set COMPILE_RESULT=%ERRORLEVEL%
cd ..

if %COMPILE_RESULT% EQU 0 (
    echo:
    echo RUNNING...
    set REF_IMG=original_dose_beam_4.dcm
    set EVAL_IMG=logfile_dose_beam_4.dcm
    build\examples\Release\gamma2DInterp.exe %REF_IMG% %EVAL_IMG%
    echo:
    build\examples\Release\gamma25D.exe %REF_IMG% %EVAL_IMG%
    echo:
    build\examples\Release\gamma3DWendling.exe %REF_IMG% %EVAL_IMG%
    echo:
    build\examples\Release\gammaImage.exe
)
