@echo off
setlocal
cd /d "%~dp0build-win\src"
set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if "%~1"=="" (
    start "" "%~dp0build-win\src\TuxMath.exe" --windowed
) else (
    start "" "%~dp0build-win\src\TuxMath.exe" %*
)
