@echo off
setlocal
cd /d "%~dp0"
echo FreshCart - Windows / Visual Studio build
where cmake >nul 2>nul
if errorlevel 1 (
  echo CMake was not found. Install the Visual Studio Desktop development with C++ workload and C++ CMake tools.
  pause
  exit /b 1
)
set "FRESHCART_QT=%~1"
if not defined FRESHCART_QT set /p "FRESHCART_QT=Enter your Qt MSVC x64 folder (for example C:\Qt\6.8.3\msvc2022_64): "
if not exist "%FRESHCART_QT%\lib\cmake\Qt6\Qt6Config.cmake" (
  echo Qt 6 MSVC installation not found at that path.
  pause
  exit /b 1
)
cmake -S . -B build-win -A x64 "-DCMAKE_PREFIX_PATH=%FRESHCART_QT%"
if errorlevel 1 goto failed
cmake --build build-win --config Release
if errorlevel 1 goto failed
ctest --test-dir build-win -C Release --output-on-failure
if errorlevel 1 goto failed
"%FRESHCART_QT%\bin\windeployqt.exe" --release --compiler-runtime "build-win\Release\FreshCart.exe"
if errorlevel 1 goto failed
echo.
echo Build complete. Open build-win\Release\FreshCart.exe for the GUI.
echo Open build-win\Release\FreshCartConsole.exe for the console version.
pause
exit /b 0
:failed
echo Build failed. See the first error above and read docs\RUN_WINDOWS.md.
pause
exit /b 1
