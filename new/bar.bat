@echo off
cd build
echo COMPILING...
cmake --build . --config=Release
set COMPILE_RESULT=%ERRORLEVEL%
cd ..
if %COMPILE_RESULT% EQU 0 (
    echo:
    echo RUNNING...
    build\Release\test.exe
)