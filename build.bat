@echo off
setlocal

set "PROJECT_ROOT=%~dp0"
set "SOURCE_DIR=%PROJECT_ROOT%."
set "BUILD_DIR=%PROJECT_ROOT%build"
set "MSYS2_ROOT=C:\msys64\ucrt64"
set "MSYS2_BIN=C:\msys64\ucrt64\bin"
set "MAKE_CMD="

cd /d "%PROJECT_ROOT%"

if exist "%MSYS2_BIN%\mingw32-make.exe" set "MAKE_CMD=%MSYS2_BIN%\mingw32-make.exe"
if not defined MAKE_CMD if exist "C:\Env\mingw\mingw64\bin\mingw32-make.exe" set "MAKE_CMD=C:\Env\mingw\mingw64\bin\mingw32-make.exe"
if not defined MAKE_CMD (
    for /f "delims=" %%M in ('where mingw32-make 2^>nul') do if not defined MAKE_CMD set "MAKE_CMD=%%M"
)
if not defined MAKE_CMD (
    echo [ERROR] MinGW Make was not found.
    echo Install mingw32-make or open an MSYS2 UCRT64 terminal.
    goto :failed
)

if exist "%MSYS2_BIN%\cmake.exe" (
    set "PATH=%MSYS2_BIN%;%PATH%"
    set "CMAKE_CMD=%MSYS2_BIN%\cmake.exe"
    set "TOOL_ARGS=-DCMAKE_C_COMPILER=%MSYS2_BIN%\gcc.exe -DCMAKE_CXX_COMPILER=%MSYS2_BIN%\c++.exe -DCMAKE_MAKE_PROGRAM=%MAKE_CMD%"
    set "PACKAGE_ARGS=-DCMAKE_PREFIX_PATH=%MSYS2_ROOT% -DSDL2_DIR=%MSYS2_ROOT%\lib\cmake\SDL2 -DSDL2_image_DIR=%MSYS2_ROOT%\lib\cmake\SDL2_image -DSDL2_mixer_DIR=%MSYS2_ROOT%\lib\cmake\SDL2_mixer -DSDL2_ttf_DIR=%MSYS2_ROOT%\lib\cmake\SDL2_ttf"
) else (
    where cmake >nul 2>&1
    if errorlevel 1 (
        echo [ERROR] CMake was not found.
        echo Install CMake or MSYS2 UCRT64, then make sure cmake is on PATH.
        goto :failed
    )
    set "CMAKE_CMD=cmake"
    set "TOOL_ARGS="
    set "PACKAGE_ARGS="
)

echo [1/2] Configuring TankGame...
"%CMAKE_CMD%" -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF %TOOL_ARGS% %PACKAGE_ARGS%
if errorlevel 1 goto :failed

echo [2/2] Building TankGame...
"%CMAKE_CMD%" --build "%BUILD_DIR%" --parallel
if errorlevel 1 goto :failed

if not exist "%BUILD_DIR%\TankGame.exe" (
    echo [ERROR] Build completed but TankGame.exe was not found.
    goto :failed
)

echo.
echo Build succeeded:
echo %BUILD_DIR%\TankGame.exe
choice /c YN /n /m "Run TankGame now? [Y/N] "
if errorlevel 2 goto :done

pushd "%BUILD_DIR%"
TankGame.exe
popd
goto :done

:failed
echo.
echo Build failed. Check the error messages above.
pause
exit /b 1

:done
echo.
pause
exit /b 0
