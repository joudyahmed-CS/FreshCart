@echo off
cd /d "%~dp0"
if not exist "build-win\Release\FreshCart.exe" (
  echo Run build_windows.bat first. See docs\RUN_WINDOWS.md.
  pause
  exit /b 1
)
start "" "build-win\Release\FreshCart.exe"
