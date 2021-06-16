@rem
@echo off

set "TAB=	"

set "MISSING_REQUIREMENTS=false"
set "MISSING_REQUIREMENTS_FOUND=false"

CALL :check_requirement git git https://git-scm.com/downloads
CALL :check_requirement msbuild msbuild https://git-scm.com/downloads

IF "%MISSING_REQUIREMENTS%"=="true" (
	PAUSE
	exit /b
) ELSE (
	IF "%MISSING_REQUIREMENTS_FOUND%"=="false" (
	echo ================================================================================
	echo .                         All dependencies are ready.                          .
	echo ================================================================================
	)
)

echo ================================================================================
echo .                              Cloning vcpkg.git                               .
echo ================================================================================

CALL git clone https://github.com/Microsoft/vcpkg.git

echo ================================================================================
echo .                         Running bootstrap-vcpkg.bat                          .
echo ================================================================================

CALL .\vcpkg\bootstrap-vcpkg.bat

echo ================================================================================
echo .                             Installing wxwidgets                             .
echo ================================================================================

CALL .\vcpkg\vcpkg.exe install wxwidgets:x64-windows

echo ================================================================================
echo .                                Installing Qt5                                .
echo ================================================================================

CALL .\vcpkg\vcpkg.exe install Qt5:x64-windows

echo ================================================================================
echo .                            Add dependencies to VS                            .
echo ================================================================================

CALL .\vcpkg\vcpkg.exe integrate install

echo ================================================================================
echo .                        Everything correctly finished.                        .
echo ================================================================================

echo .
echo ================================================================================
echo .        DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake        .
echo ================================================================================

PAUSE

exit /b

rem Ensures that the system has a specific program installed on the PATH.
:check_requirement
set "MISSING_REQUIREMENT=true"
where %1 > NUL 2>&1 && set "MISSING_REQUIREMENT=false"

IF "%MISSING_REQUIREMENT%"=="true" (
	IF "%MISSING_REQUIREMENTS_FOUND%"=="false" (
		echo ================================================================================
		echo .                     Found missing programs/dependencies.                     .
		echo ================================================================================
		echo .    Download and install [make sure they're accessible through commandline]:
		set "MISSING_REQUIREMENTS_FOUND=true"
	)
	echo .        - %2  %TAB%link: %3
	set "MISSING_REQUIREMENTS=true"
)

exit /b