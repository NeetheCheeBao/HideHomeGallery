@echo off
setlocal
echo Configuring...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo CMake configure failed.
    exit /b %errorlevel%
)

echo Building Release...
cmake --build build --config Release --parallel
if errorlevel 1 (
    echo Build failed.
    exit /b %errorlevel%
)

echo.
echo ========================================
echo Build complete:
echo   build\Release\HideHomeGallery.exe
echo ========================================
