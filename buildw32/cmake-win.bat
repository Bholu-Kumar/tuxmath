@echo off
REM ============================================================
REM  cmake-win.bat — Build TuxMath on Windows using MSYS2/MinGW
REM  Run this from the MSYS2 MinGW64 shell (or from cmd if cmake
REM  and mingw32-make are on PATH).
REM
REM  Prerequisites (install in MSYS2 MinGW64 shell):
REM    pacman -S mingw-w64-x86_64-cmake
REM    pacman -S mingw-w64-x86_64-SDL3
REM    pacman -S mingw-w64-x86_64-SDL3_image
REM    pacman -S mingw-w64-x86_64-SDL3_ttf
REM    pacman -S mingw-w64-x86_64-SDL3_mixer
REM    pacman -S mingw-w64-x86_64-librsvg
REM    pacman -S mingw-w64-x86_64-libxml2
REM    (optional) pacman -S mingw-w64-x86_64-SDL3_net
REM    (optional) pacman -S mingw-w64-x86_64-espeak-ng
REM
REM  t4kcommon must be built and installed first:
REM    cd ../t4kcommon-master && mkdir build-win && cd build-win
REM    cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
REM    cmake --build . && cmake --install .
REM ============================================================

REM Move to the repo root (one level above buildw32/)
pushd %~dp0..

REM Create and enter the Windows build directory
if not exist build-win mkdir build-win
cd build-win

REM Configure
cmake .. ^
    -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Debug

if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed.
    popd
    exit /b 1
)

REM Build
cmake --build . --parallel

if errorlevel 1 (
    echo.
    echo [ERROR] Build failed.
    popd
    exit /b 1
)

echo.
echo ============================================================
echo  Build complete!  Executable: build-win\TuxMath.exe
echo  Data files are automatically copied next to the exe.
echo ============================================================

popd
