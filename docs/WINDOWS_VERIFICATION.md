# Windows verification record

The following observations come from the team computer during the setup session on 23-24 November 2026.

- Windows 10 x64, Visual Studio Community 2026 MSVC tools 18.9, Qt 6.12.1 MSVC 2022 x64 kit.
- The Visual Studio build script compiled FreshCart, FreshCartConsole and core_tests. Its CTest run reported 100% passing (1 suite).
- VS Code with Microsoft C/C++ and CMake Tools configured the project at `C:/Users/aba/Desktop/FreshCart`.
- The final Debug build compiled all three targets and ended with `Build finished with exit code 0`.
- `windeployqt` copied the Debug Qt runtime files and Windows platform plugin.
- The resulting desktop application launched successfully. `screenshots/08_windows_application.png` is the team's screenshot.
- An earlier LNK1168 attempt failed while the executable was in use; closing the application and rebuilding succeeded. C4458 is a nonfatal local-variable naming warning.

This confirms Windows compilation and application launch. It does not claim that the team ran every automated GUI scenario on Windows or that it completed a Visual Studio IDE viva. The three transaction screenshots in the report are the original automated Qt application runs, captured on Linux. The shared calculation functions are the same C++ source in both interfaces.
