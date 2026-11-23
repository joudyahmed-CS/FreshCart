# Verified VS Code workflow on Windows

Toolchain used by the team: Visual Studio Community 2026 x64 C++ tools, Qt 6.12.1 MSVC 2022 x64, Microsoft C/C++ and CMake Tools extensions in VS Code.

1. Open the project root (the folder containing CMakeLists.txt) and trust this project folder.
2. Run `CMake: Configure` from Ctrl+Shift+P and select the Visual Studio 2026 amd64/x64 compiler kit when asked. The CMake file contains the verified Qt kit location.
3. Run `CMake: Build`. Close the running application before rebuilding.
4. For a new executable directory, deploy the matching Qt runtime with windeployqt.

The build directory depends on the selected kit/preset. The team's verified Debug executable was under `out/build/Visual Studio Community 2026 Release - amd64/Debug/FreshCart.exe`. A preset's display name containing the word Release does not force a Release build; the selected build configuration decides that.

From a PowerShell terminal, replacing the application path if yours differs:

```powershell
& "C:\Qt\6.12.1\msvc2022_64\bin\windeployqt.exe" ".\out\build\Visual Studio Community 2026 Release - amd64\Debug\FreshCart.exe"
& ".\out\build\Visual Studio Community 2026 Release - amd64\Debug\FreshCart.exe"
```

For the console, run the adjacent `FreshCartConsole.exe`. The console does not require Qt DLLs.

The course requests a Visual Studio demonstration. After configuring and building in a simple folder path, open the generated `FreshCart.slnx` (Visual Studio 2026) or `FreshCart.sln` (other generator versions) from the build folder. Choose Release/x64 if using the batch-built Release tree, or the configuration you actually built. Set FreshCartConsole as the startup project for the mandatory console demonstration.
