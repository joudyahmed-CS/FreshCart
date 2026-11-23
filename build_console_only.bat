@echo off
setlocal
cd /d "%~dp0"
where cl >nul 2>nul
if errorlevel 1 (
  echo Run this script from the x64 Native Tools Command Prompt for Visual Studio.
  pause
  exit /b 1
)
if not exist build-console mkdir build-console
pushd build-console
cl /nologo /std:c++17 /EHsc /W4 /utf-8 /I..\src ..\src\console.cpp ..\src\store.cpp /Fe:FreshCartConsole.exe
if errorlevel 1 (
  popd
  pause
  exit /b 1
)
popd
echo Ready: build-console\FreshCartConsole.exe
pause
