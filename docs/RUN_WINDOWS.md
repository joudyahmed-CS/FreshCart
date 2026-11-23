# Build and run on Windows with Visual Studio

This archive contains source code, not a prebuilt Windows executable. The team has verified compilation and GUI launch with Visual Studio 2026 C++ tools and Qt 6.12.1 on Windows. Build it once with the steps below. The GUI is native Qt Widgets and all application logic is C++17; it is not a web page.

## 1. Prerequisites

In Visual Studio Installer, select **Desktop development with C++**, the MSVC C++ x64 compiler, a Windows SDK and C++ CMake tools for Windows.

Install Qt 6 using its installer. Select a desktop **MSVC 2022 64-bit** kit that matches Visual Studio 2022. The kit folder contains `bin`, `include`, and `lib`, for example `C:\Qt\6.8.3\msvc2022_64`. This is an example path; your installed version may differ.

Official downloads and help:

- https://visualstudio.microsoft.com/downloads/
- https://www.qt.io/download-qt-installer-oss
- https://doc.qt.io/qt-6/windows.html

## 2. Easiest build

Extract the ZIP into a normal local folder. Double-click `build_windows.bat` and enter your Qt kit path. If CMake is not on PATH, run the script inside the **x64 Native Tools Command Prompt for VS 2022**.

The script configures an x64 Visual Studio build, compiles both targets, runs core tests and uses `windeployqt` to place Qt runtime files beside the GUI executable.

- GUI: `build-win\Release\FreshCart.exe`
- Console: `build-win\Release\FreshCartConsole.exe`
- Launch again: `run_windows.bat`

Keep all deployed DLLs and the `platforms` folder beside the GUI executable. Moving only the `.exe` is insufficient for a dynamically linked Qt build.

## 3. Open and debug in Visual Studio

After running the build script, open the generated `build-win\FreshCart.slnx` in Visual Studio 2026, or `FreshCart.sln` if that is what your generator produced. Set `FreshCart` as the startup project and select **Release / x64**. Use **Ctrl+F5** to run.

To demonstrate the course console, set `FreshCartConsole` as the startup project and use Ctrl+F5. It shares the same calculation functions.

Alternatively use **File > Open > Folder** on the source folder. In its CMake configuration, add your Qt kit directory to `CMAKE_PREFIX_PATH`. Select the `FreshCart` target to run.

## 4. Manual commands

From the x64 Native Tools Command Prompt (replace the example path):

```bat
cmake -S . -B build-win -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.8.3\msvc2022_64"
cmake --build build-win --config Release
ctest --test-dir build-win -C Release --output-on-failure
"C:\Qt\6.8.3\msvc2022_64\bin\windeployqt.exe" --release --compiler-runtime build-win\Release\FreshCart.exe
build-win\Release\FreshCart.exe
```

If using another Visual Studio version, use its corresponding CMake generator and a compatible Qt MSVC kit. Do not mix MinGW Qt libraries with MSVC output.

## 5. Console without Qt

Open the x64 Native Tools Command Prompt, change to the project folder and run:

```bat
build_console_only.bat
build-console\FreshCartConsole.exe
```

In Visual Studio you may also create an Empty C++ Console Project, add `src/console.cpp` and `src/store.cpp`, add `src/store.h`, and select the C++17 language standard. Do not add `src/main.cpp` to that console project; there must be only one `main` function per application.

## Troubleshooting

| Message | Fix |
|---|---|
| Qt6Config.cmake not found | Point to the desktop kit root, not the Qt installer root or `bin` folder. |
| Qt6Widgets.dll missing | Run the matching kit's `windeployqt` against the built GUI executable. |
| Qt platform plugin could not initialize | Ensure `platforms/qwindows.dll` was deployed; remove an unrelated system `QT_PLUGIN_PATH` override if present. |
| Compiler/linker mismatch | Use the MSVC x64 Qt kit with Visual Studio x64, then rebuild in a fresh build folder. |
| CMake not recognized | Install the Visual Studio CMake component and use its Native Tools prompt. |
| Window does not open from a headless runner | Run on a desktop session. Automated GUI checks use `QT_QPA_PLATFORM=offscreen`. |

The original automated tests used GCC + Qt 6.8.2 on Linux. The team subsequently verified Windows builds and GUI launch with MSVC 2026 + Qt 6.12.1. See WINDOWS_VERIFICATION.md and RUN_VSCODE.md.
